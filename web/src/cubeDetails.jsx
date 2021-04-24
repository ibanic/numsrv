import React, { Component } from 'react'
import { Link } from 'react-router-dom'
import CubeBrowser from '../../web-common/cubeBrowser'
import loadJs from './loadJs'
import { transName, transDimItemNameShort, isIntUnits } from '../../web-common/trans'
import { usUnitName } from '../../web-common/units'
import Settings from '../../web-common/settings'




const chartOpt = {
	elements: {
		line: {
			tension: 0.13,
		},
	},
	scales: {
		yAxes: [{
			display: true,
			ticks: {
				suggestedMin: 0
			},
		}],
	},
}

export default class CubeDetails extends Component {
	constructor(props) {
		super(props)

		this.state = {
			data: null,
			error: null,
			chartData: false,
			valueFull: {},
			selection: null,
		}
	}


	async componentDidMount() {
		try {
			const r = await fetch(API_URL+'/api/cubes/'+this.props.match.params.cubeKey)
			if( r.status == 404 ) {
				this.setState({error: 'Cube not found'})
				return
			}
			if( !r.ok ) {
				this.setState({error: 'Unknown error'})
				return
			}
			const js = await r.json()
			this.setState({data: js})
		}
		catch(ex) {
			this.setState({error: new String(ex)})
		}
	}


	selectionChanged(dt) {
		// reset
		this.setState({selection:dt, chartData:null})

		if( !dt )
			return

		if( dt.dimItemsX.length > 1 || dt.dimItemsY.length > 1 ) {
			const chartData = {
				labels: dt.dimItemsX.map(o=>transDimItemNameShort(o)),
				datasets: dt.dimItemsY.map((o,idx) => ({
					label: transDimItemNameShort(o),
					data: dt.data[idx],
				}))
			}
			let prms = Promise.resolve()
			if( !window.ReactChartjs2 ) {
				prms = prms
				.then(() => loadJs('https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.8.0/Chart.min.js'))
				.then(() => loadJs('https://cdnjs.cloudflare.com/ajax/libs/react-chartjs-2/2.7.6/react-chartjs-2.min.js'))
			}
			prms.then(()=>this.setState({chartData: chartData}))
		}
		else {
			// load cell
			const arr = [...dt.dimItems]
			if( dt.dimX !== false )
				arr[dt.dimX] = dt.dimItemsX[0].id
			arr[dt.dimY] = dt.dimItemsY[0].id
			const idx = arr.join('/')
			const cell = this.state.valueFull[idx]
			if( cell === undefined ) {
				// load
				this.setState(prevState => ({valueFull:{...prevState.valueFull, [idx]:null}}))
				const pth = arr.map(n => n.toLocaleString('fullwide', {useGrouping:false})).join('/')
				fetch(API_URL+'/api/cubes/'+this.state.data.key+'/cellById/'+pth)
				.then(r => r.json())
				.then(js => this.setState(prevState => ({valueFull:{...prevState.valueFull, [idx]:js}})))
			}
		}
	}


	renderCellInfo() {
		if( !this.state.selection || this.state.selection.dimItemsX.length > 1 || this.state.selection.dimItemsY.length > 1 )
			return
		const arr = [...this.state.selection.dimItems]
		if( this.state.selection.dimX !== false )
			arr[this.state.selection.dimX] = this.state.selection.dimItemsX[0].id
		arr[this.state.selection.dimY] = this.state.selection.dimItemsY[0].id
		const idx = arr.join('/')
		const cell = this.state.valueFull[idx]
		if( !cell )
			return <p>Loading cell info ...</p>
		return <div>
			value: {cell.value.toLocaleString()} <br/>
			desc: {cell.desc} <br/>
			formula: {cell.formula}
		</div>
	}


	render() {
		if( !this.state.data && !this.state.error ) {
			return <p>Loading ...</p>
		}
		if( this.state.error ) {
			return <div>
				{this.state.error}
			</div>
		}
		let chart = null
		if( this.state.chartData ) {
			if( window.ReactChartjs2 )
				chart = React.createElement(window.ReactChartjs2.Line, {data:this.state.chartData, options:chartOpt})
			else
				chart = <p>Loading chart ...</p>
		}
		return <div className="container">
			<p><Link to='/'>Home</Link></p>
			<div className="d-flex">
				<p className="h6 flex-fill">Cube</p>
				<Settings/>
			</div>
			<h1>{transName(this.state.data.name)}</h1>
			<p>{transName(this.state.data.desc)}</p>
			{this.state.data.unit.length>0 && <div>
				Unit: {isIntUnits() ? this.state.data.unit : usUnitName(this.state.data.unit)}
			</div>}
			<CubeBrowser cubeKey={this.state.data.key} onSelect={r=>this.selectionChanged(r)} />
			{this.state.chartData && <div style={{width:'40rem'}}>
				<button className="btn btn-secondary" onClick={()=>this.setState({chartData:null})}>
					X
				</button>
				{chart}
			</div>}
			{this.renderCellInfo()}
		</div>
	}
}