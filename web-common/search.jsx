import React, { Component } from 'react'
import style from './search.css'
import { currentLang } from './trans';


// props to update on change
const updateOnChange = [
	'dimKey',		// false or dimKey or list of allowed dims
	'multipleQueries',	// true or false - break query by comma and threat everything separate
]

export default class Search extends Component {
	constructor(props) {
		super(props)
		this.state = {
			query: '',
			results: [],
			status: 0,  // 0-ok, 1-loading, 2-error
		}
		this.controller = null
		this.timer = null
		this.handleTimeCB = this.handleTimeCB.bind(this)
	}

	componentDidUpdate(prevProps) {
		if( this.state.query == '' )
			return
		const changed = updateOnChange.find(opt => {
			//return this.props[opt] != prevProps[opt]  not working properly when comapring array or object
			if( this.props[opt] != prevProps[opt] ) {
				if( this.props[opt] && prevProps[opt] && typeof this.props[opt] == 'object' && typeof prevProps[opt] == 'object' ) {
					return JSON.stringify(this.props[opt]) != JSON.stringify(prevProps[opt])
				}
				return true
			}
			return false
		})
		if( changed )
			this.setState({query: '', results: [], status:0})
	}

	async set(query, results, status) {
		return new Promise((resolve, reject)=>{
			this.setState({query: query, results: results, status: status}, ()=>{
				if( this.props.callback )
					this.props.callback(query, results)
				resolve()
			})
		})
	}

	async handle(q) {
		if( this.controller ) {
			this.controller.abort()
			this.controller = null
		}
		if(this.timer) {
			clearTimeout(this.timer)
			this.timer = null
		}

		await this.set(q, [], 1)
		if( q == '' )
			return
		this.timer = setTimeout(this.handleTimeCB, 100)
	}

	async handleTimeCB() {
		const q = this.state.query
		this.controller = new AbortController()
		const signal = this.controller.signal
		const queryObj = {
			query: q,
			lang: currentLang(),
		}
		if( this.props.multipleQueries ) {
			if( q.indexOf(',') != -1 ) {
				queryObj.multipleQueries = true
			}
		}
		let r, data
		try {
			r = await fetch('/api/dims/'+this.props.dimKey+'/search', {
				method:		'POST',
				headers:	{'Content-Type':'application/json'},
				body:		JSON.stringify(queryObj),
				signal:		signal,
			})
			if( !r.ok )
				return Promise.reject()
			data = await r.json()
		}
		catch(err) {
			console.log('aborted by user or error',err)
			if( this.controller )
				this.set(q, [], 2)
			return
		}
		if( !this.controller || !data )
			return
		this.set(q, data, 0)
	}

	renderSearchDimItem(r) {
		// todo check that
		return <a href="#" key={r.uniqueId}>
			<span className='d-block'>
				{r.itemName}
				{r.itemName!=r.itemKey && <span className="ml-3 text-muted">{r.itemKey}</span>}
			</span>
			<small className='d-block text-muted'>dim: {r.dimKey} TODO trans name</small>
		</a>
	}
	
	render() {
		return <div className='search-container'>
			<div className={'input-group mb-3 '+(this.props.size&&'input-group-'+this.props.size)}>
				{!this.props.hideIcon && <div className="input-group-prepend">
					<span className="input-group-text">🔍 {/*EN*/}</span>
				</div>}
				<input type="search" className="form-control" placeholder="Search ..." value={this.state.query} onChange={e=>this.handle(e.target.value)} />
			</div>
			
			{this.state.query.length>0 && !this.props.hideResults && this.state.status==2 && <p>Error</p>}
			{this.state.query.length>0 && !this.props.hideResults && this.state.status==1 && <p>Loading ...</p>}
			{this.state.query.length>0 && !this.props.hideResults && this.state.status==0 && <div className='search-results'>
				{this.state.results.length>0 ? this.state.results.map((r,idx) => {
					if(r.type == 'dimItem')
						return this.renderSearchDimItem(r)
				}) : <span>No results</span>}
			</div>}
		</div>
	}
}