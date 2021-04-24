import React, { Component } from 'react'
import { Link } from 'react-router-dom'
import { transName } from '../../web-common/trans'

export default class Home extends Component {
	constructor(props) {
		super(props)
		this.state = {
			cubes: [],
			dims: [],
		}
	}
	async componentDidMount() {
		const r = await fetch(API_URL+'/api/cubes')
		const js = await r.json()
		this.setState({cubes: js})


		const r2 = await fetch(API_URL+'/api/dims')
		const js2 = await r2.json()
		this.setState({dims: js2})
	}
	render() {
		return <div className="container">
			
			<h1>NumSrv</h1>
			<p className="lead">
				Drill down to data you need. Select city or country and time.
			</p>
			<div className="list-group" style={{maxWidth:'16rem'}}>
				{this.state.cubes.map(d => <Link key={d.key} to={'/cubes/'+d.key} className="list-group-item list-group-item-action">{transName(d.name)}</Link>)}
			</div>
			<h5 className="mt-4">Dims</h5>
			<div>
				{this.state.dims.map(d => <Link key={d.key} to={'/dims/'+d.key} className="btn btn-outline-secondary mr-3 btn-sm" role="button">{transName(d.name)}</Link>)}
			</div>
		
			<p className="text-muted text-left mt-3" style={{fontSize:'0.6rem'}}>
				Made by: ignacb@gmail.com<br/>
				Data source: wikipedia, ISO country codes, geonames.org<br/>
				Source code: <a href="https://github.com/ibanic/numsrv" target="_blank">github.com/ibanic/numsrv</a>
			</p>
		</div>
	}
}