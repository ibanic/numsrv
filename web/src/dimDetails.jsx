import React, { Component } from 'react'
//import { Button } from 'react-bootstrap'
import { Link } from 'react-router-dom'
import DimBrowser from '../../web-common/dimBrowser'
import { transName, transDimItemNameShort, isIntUnits } from '../../web-common/trans'

export default class DimDetails extends Component {
	constructor(props) {
		super(props)
		this.state = {
			data: null
		}
	}
	async componentDidMount() {
		const r = await fetch(API_URL+'/api/dims/'+this.props.match.params.dimKey)
		const js = await r.json()
		this.setState({data: js})
	}
	render() {
		if( !this.state.data )
			return <p>Loading ...</p>
		return <div className="container">
			<p><Link to='/'>Home</Link></p>
			<p className="h6">Dimension</p>
			<h1>{transName(this.state.data.name)}</h1>
			<p>{this.state.data.description}</p>
			<DimBrowser dimKey={this.state.data.key} searchPosition='start' />
		</div>
	}
}