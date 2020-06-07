import React, { Component } from 'react'
//import { Button } from 'react-bootstrap'
import { Link } from 'react-router-dom'
//import Search from './search'
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
		const r = await fetch('http://localhost:5000/api/cubes')
		const js = await r.json()
		this.setState({cubes: js})


		const r2 = await fetch('http://localhost:5000/api/dims')
		const js2 = await r2.json()
		this.setState({dims: js2})
	}
	render() {
		return <div className="container">
			
			<h1>NumSrv</h1>
			<p className="lead">
				Predictable datasets with every data you may need.
			</p>
			<div className="list-group">
				{this.state.cubes.map(d => <Link key={d.key} to={'/cubes/'+d.key} className="list-group-item list-group-item-action">{transName(d.name)}</Link>)}
			</div>
			<h1>Dims</h1>
			<div className="list-group">
				{this.state.dims.map(d => <Link key={d.key} to={'/dims/'+d.key} className="list-group-item list-group-item-action">{transName(d.name)}</Link>)}
			</div>
		
			<p className="text-muted text-center text-small mt-3">
				<a href="mailto:ignacb@gmail.com">ignacb@gmail.com</a>
			</p>
		</div>
	}
}