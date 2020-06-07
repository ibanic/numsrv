import React, { Component } from 'react'
import style from './dimBrowser.css'
import Search from './search';
import { transDimItemNameShort, transDimItemNameLong } from './trans'
import CountryToFlag from './flags'

function setStatePromise(obj, what) {
	return new Promise((resolve,reject) => {
		obj.setState(what, resolve)
	})
}


export default class DimBrowser extends Component {
	constructor(props) {
		super(props)
		this.state = {
			items: {},
			expanded: [],
			searchQuery: '',
			searchResults: [],
		}
		this.searchCallback = this.searchCallback.bind(this)
	}


	init() {
		this.setState({
			items: {},
			expanded: [],
			searchQuery: '',
			searchResults: [],
		}, ()=>{
			this.loadItems([0])
			.then(() => {
				if( this.state.items[0].childs.length > 0 )
					this.expand(0)
				else
					this.callCallback()
			})
			//this.callCallback()
		})
	}


	componentDidMount() {
		this.init()
	}


	componentDidUpdate(prevProps) {
		if( this.props.dimKey != prevProps.dimKey ) {
			this.init()
		}
	}


	async loadItems(itmIds) {
		let r, js
		const opt = {
			method:'POST',
			body:JSON.stringify(itmIds),
			headers:{'Content-Type':'application/JSON'},
		}
		r = await fetch(API_URL+'/api/dims/'+this.props.dimKey+'/itemsMany', opt)
		if(!r.ok) {
			throw new Error('Error loading dims')
		}
		js = await r.json()
		return setStatePromise(this, prevState => {
			const obj = {...prevState.items}
			itmIds.map((id,i) => obj[id] = js[i])
			return {items: obj}
		})
	}


	expand(itemId) {
		const cs = this.state.items[itemId].childs
		const refresh = () => this.setState(prevState => ({expanded: [...prevState.expanded, itemId]}), ()=>this.callCallback())
		if( cs.length > 0 && !(cs[0] in this.state.items) ) {
			this.loadItems(cs).then(refresh)
		}
		else {
			refresh()
		}
	}


	collapse(itemId) {
		this.setState(prevState => ({expanded: [...prevState.expanded].filter(v => v != itemId)}), ()=>this.callCallback())
	}


	callCallback() {
		if( !this.props.expandCallback )
			return
		let arr
		if( this.state.searchQuery == '' ) {
			arr = []
			this.callCallbackCheck(arr, 0)
		}
		else {
			arr = this.state.searchResults
		}
		this.props.expandCallback(arr)
	}


	callCallbackCheck(arr, itemId) {
		const itm = this.state.items[itemId]
		arr.push({
			id: itemId,
			key: itm ? itm.key : '',
			nameShort: itm ? itm.nameShort : {},
			nameLong: itm ? itm.nameLong : {},
		})
		const exp = this.state.expanded.indexOf(itemId) != -1
		if( exp ) {
			this.state.items[itemId].childs.map(itmId2 => this.callCallbackCheck(arr, itmId2))
		}
	}


	renderItemName(itm) {
		let txt = transDimItemNameShort(itm)
		if( this.props.dimKey == 'geo' || this.props.dimKey.startsWith('geo-') || this.props.dimKey.endsWith('-geo') || this.props.dimKey.indexOf('-geo-')!=-1 ) {
			if( itm.key == 'total' ) {
				txt = '🌍 '+txt
			}
			else if( itm.key.length == 2 ) {
				const f = CountryToFlag(itm.key)
				if(f) {
					txt = f+' '+txt
				}
			}
		}
		return txt
	}


	renderItem(itemId, level) {
		const loaded = itemId in this.state.items
		const exp = this.state.expanded.indexOf(itemId) != -1
		const numChilds = loaded ? this.state.items[itemId].childs.length : 0
		const stl = {}
		stl.cursor = numChilds > 0 || this.props.onChange ? 'pointer' : 'default'
		if( itemId == this.props.itemId )
			stl.borderColor = '#ccc'
		const stlItm = {
			paddingLeft: (level*0.5).toString()+'rem'
		}
		
		//const arr = this.state.items[itemId].childs.map(itmId2 => this.renderItem(itmId2, level+1))
		const itm = <div key={itemId} className='dim-item' style={stlItm}>
			<div className={'dim-item-container '+(this.props.onChange?'selectable':'')}>
				<div style={{width:'1rem'}}>
					{numChilds > 0 && <a href="#" onClick={e=>{e.preventDefault(); exp ? this.collapse(itemId) : this.expand(itemId)}}>
						{exp ? '-' : '+'}
					</a>}
				</div>
				{loaded && <div
					onClick={e=>{e.preventDefault();this.props.onChange ? this.props.onChange(this.state.items[itemId]) : (exp ? this.collapse(itemId) : this.expand(itemId))}}
					className='dim-item-name'
					style={stl}
					title={transDimItemNameLong(this.state.items[itemId])}
				>
					{this.renderItemName(this.state.items[itemId])}
				</div>}
			</div>
		</div>
		if( exp ) {
			const arr = this.state.items[itemId].childs.map(itmId2 => this.renderItem(itmId2, level+1))
			return [itm, ...arr]
		}
		else {
			return [itm]
		}
	}


	renderItemSearch(obj, idx) {
		if(!obj) {
			return <div key={idx}>&nbsp;</div>
		}
		const stl = {}
		stl.cursor = this.props.onChange ? 'pointer' : 'default'
		if( obj.id == this.props.itemId )
			stl.borderColor = '#ccc'
		return <div key={obj.key} className='dim-item'>
			<div className={'dim-item-container '+(this.props.onChange?'selectable':'')}>
				<div onClick={e=>{e.preventDefault(); this.props.onChange ? this.props.onChange(obj) : null}} className='dim-item-name' style={stl}>
					{this.renderItemName(obj)}
				</div>
			</div>
		</div>
	}


	searchCallback(query, results) {
		this.setState({searchQuery: query, searchResults: results}, ()=>this.callCallback())
	}


	render() {
		if(!this.state.items)
			return 'loading ...'
		let search = null
		if( this.props.searchPosition=='start' || this.props.searchPosition=='end' )
			search = <div className='dim-item'>
				<Search
					size='sm'
					dimKey={this.props.dimKey}
					hideResults={true}
					hideIcon={true}
					callback={this.searchCallback}
					multipleQueries={this.props.searchMultiple}  // break query by comma
				/>
			</div>
		if( this.state.searchQuery == '' ) {
			return <div className={this.props.horizontal?'dim-horizontal':''}>
				{this.props.searchPosition=='start' && search}
				{this.renderItem(0, 0)}
				{this.props.searchPosition=='end' && search}
			</div>
		}
		else {
			return <div className={this.props.horizontal?'dim-horizontal':''}>
				{this.props.searchPosition=='start' && search}
				{this.state.searchResults.map((obj,idx) => this.renderItemSearch(obj,idx))}
				{this.props.searchPosition=='end' && search}
			</div>
		}
	}
}