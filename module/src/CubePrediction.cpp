#include "CubeWriter.hpp"
#include "Db.hpp"

namespace NumSrv
{
	void exponentalCurve(const std::vector<std::pair<double, double> >& data, double* a, double*b)
	{
		// http://www.codewithc.com/c-program-for-linear-exponential-curve-fitting/
		std::vector<double> Y;
		Y.reserve(data.size());
		double sumx=0,sumy=0,sumxy=0,sumx2=0;
		for( auto p : data )
			Y.push_back(std::log(p.second));
		for( size_t i=0; i<data.size(); ++i )
		{
			auto x = data[i].first;
			sumx=sumx +x;
			sumx2=sumx2 +x*x;
			sumy=sumy +Y[i];
			sumxy=sumxy +x*Y[i];
		}
		double n2 = data.size();
		double A=((sumx2*sumy -sumx*sumxy)*1.0/(n2*sumx2-sumx*sumx)*1.0);
		*b=((n2*sumxy-sumx*sumy)*1.0/(n2*sumx2-sumx*sumx)*1.0);
		*a=std::exp(A);
	}

	void linearCurve(const std::vector<std::pair<double, double> >& data, double* a, double*b)
	{
		// http://www.codewithc.com/c-program-for-linear-exponential-curve-fitting/
		double sumx=0,sumy=0,sumxy=0,sumx2=0;
		for( size_t i=0; i<data.size(); ++i )
	    {
			auto x = data[i].first;
			auto y = data[i].second;
	        sumx=sumx +x;
	        sumx2=sumx2 +x*x;
	        sumy=sumy +y;
	        sumxy=sumxy +x*y;

	    }
		auto n = data.size();
	    *a=((sumx2*sumy -sumx*sumxy)*1.0/(n*sumx2-sumx*sumx)*1.0);
	    *b=((n*sumxy-sumx*sumy)*1.0/(n*sumx2-sumx*sumx)*1.0);
	}

	float calcError(const std::vector<std::pair<double, double>>& dataOriginal, const std::vector<double>& dataPredicted)
	{
		float qSum = 0;
		for( size_t i=0; i<dataOriginal.size(); ++i )
		{
			double num1 = dataOriginal[i].second;
			double num2 = dataPredicted[i];
			double dif = std::abs(num1 - num2);
			float q;
			auto base = std::abs(num1);
			if( base > 1 )
				q = 1 - dif / base;
			else
			{
				if( dif < 1 )
					q = 1;
				else
					q = 0;
			}
			qSum += q;
		}
		return qSum / dataOriginal.size();
	}








	void CubeWriter::predict(std::vector<uint32_t> coord, uint32_t nthDimTime, uint32_t levelForPrediction)
	{
		//
		//auto dimKey = meta().dims[nthDimTime].key;
		//auto& dim = db()->dim(dimKey);
		auto dim = dimCache[nthDimTime];
		auto dimItemsTime = dim->itemsOnLevel(levelForPrediction);

		// make sure coord is full
		assert(coord.size() <= meta().dims.size());
		while( coord.size() < meta().dims.size() ) {
			coord.push_back(0);
		}

		// load values
		//auto coord2 = coord;
		std::vector<std::pair<double, double>> vals;
		for(size_t i=0; i<dimItemsTime.size(); ++i) {
			coord[nthDimTime] = dimItemsTime[i];
			auto c = cell(coord);
			if( !c.q.isComputed() || c.q.value() >= 0.5 ) {
				vals.push_back({i, c.num});
			}
		}

		// prepare values
		if(vals.size() < 3) {
			throw std::runtime_error("predict() not enough data");
		}





		// linear
		double linA, linB;
		float linQ;
		linearCurve(vals, &linA, &linB);
		// calculated data
		{
			std::vector<double> linCalculated(vals.size());
			for( size_t i=0; i<vals.size(); ++i )
			{
				auto x = vals[i].first;
				linCalculated[i] = linA + linB * x;
			}
			linQ = calcError(vals, linCalculated);
		}

		// exponential
		double expA, expB;
		float expQ;
		exponentalCurve(vals, &expA, &expB);
		// calculated data
		{
			std::vector<double> expCalculated(vals.size());
			for( size_t i=0; i<vals.size(); ++i )
			{
				auto x = vals[i].first;
				expCalculated[i] = expA * std::exp(expB * x);
			}
			expQ = calcError(vals, expCalculated);
		}

		if( linQ >= expQ && linQ > 0.3 )
		{
			/*type_ = 0; // ModelType::Linear;
			predictionA_ = linA;
			predictionB_ = linB;
			predictionQ_ = linQ;*/

			for(size_t i=0; i<dimItemsTime.size(); ++i) {
				coord[nthDimTime] = dimItemsTime[i];
				auto idx = calcIdx(coord);
				auto& c = cells_[idx];
				if( c.q.isComputed() && c.q.value() < 0.5 ) {
					c.num = linA + linB * i;
					c.q = Quality::makePredicted();
				}
				else if(c.q.isComputed() && c.q.value() < 1) {
					c.q = Quality::makePredicted();
				}
			}
		}
		else if( expQ >= linQ && expQ > 0.3 )
		{
			/*type_ = 1;  // ModelType::Exponential;
			predictionA_ = expA;
			predictionB_ = expB;
			predictionQ_ = expQ;*/

			for(size_t i=0; i<dimItemsTime.size(); ++i) {
				coord[nthDimTime] = dimItemsTime[i];
				auto idx = calcIdx(coord);
				auto& c = cells_[idx];
				if(c.q.isComputed() && c.q.value() < 0.5) {
					c.num = expA * std::exp(expB * i);
					c.q = Quality::makePredicted();
				}
				else if(c.q.isComputed() && c.q.value() < 1) {
					c.q = Quality::makePredicted();
				}
			}
		}
		else
		{
			//type_ = 2; // ModelType::Static;
			//m_ = 0;
			double num = 0;
			for( auto p : vals ) {
				num += p.second;
			}
			num /= vals.size();

			for(size_t i=0; i<dimItemsTime.size(); ++i) {
				coord[nthDimTime] = dimItemsTime[i];
				auto idx = calcIdx(coord);
				auto& c = cells_[idx];
				if(c.q.isComputed() && c.q.value() < 0.5) {
					c.num = num;
					c.q = Quality::makePredicted();
				}
				else if(c.q.isComputed() && c.q.value() < 1) {
					c.q = Quality::makePredicted();
				}
			}
		}
	}
}
