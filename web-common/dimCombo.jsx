import React, { Component } from 'react'
import DimBrowser from './dimBrowser';
import { transDimItemNameShort } from './trans'

export default class DimCombo extends Component {
	constructor(props) {
		super(props)
		this.state = {
			shown: false,
			itemsPreloaded: {},
		}
		this.handleClickOutside = this.handleClickOutside.bind(this)
		this.escFunction = this.escFunction.bind(this)
		this.setWrapperRef = this.setWrapperRef.bind(this)
		this.wrapperRef = null
	}

	componentWillUnmount() {
		document.removeEventListener('mousedown', this.handleClickOutside)
		if( this.wrapperRef )
			this.wrapperRef.removeEventListener('keydown', this.escFunction, false)
	}

	componentDidMount() {
		document.addEventListener('mousedown', this.handleClickOutside)
		this.loadSelected()
	}

	async loadSelected() {
		const id = this.props.itemId
		if( id in this.state.itemsPreloaded )
			return
		this.setState(prevState => ({
			itemsPreloaded: {...prevState.itemsPreloaded, [id]: null}
		}))
		// load selected
		const r = await fetch('/api/dims/'+this.props.dimKey+'/items/'+id)
		const js = await r.json()
		this.setState(prevState => ({
			itemsPreloaded: {...prevState.itemsPreloaded, [js.id]: js}
		}))
	}

	componentDidUpdate(prevProps) {
		if( this.props.dimKey != prevProps.dimKey ) {
			this.setState({itemsPreloaded: {}})
			this.loadSelected()
			return
		}
		if( this.props.itemId != prevProps.itemId ) {
			this.loadSelected()
			return
		}
	}

	toggle() {
		this.setState(prevState => ({shown: !prevState.shown}))
	}

	setWrapperRef(node) {
		if( this.wrapperRef )
			this.wrapperRef.removeEventListener('keydown', this.escFunction, false)
		if( node )
			node.addEventListener('keydown', this.escFunction, false)
		this.wrapperRef = node
	}

	handleClickOutside(event) {
		if (this.wrapperRef && !this.wrapperRef.contains(event.target)) {
			if( this.state.shown )
				this.setState({shown: false})
		}
	}

	escFunction(event){
		if(event.keyCode === 27) {
			// esc
			if( this.state.shown ) {
				event.preventDefault()
				event.stopPropagation()
				this.setState({shown: false})
			}
		}
	}

	change(itmObj) {
		this.setState(prevState => ({
			itemsPreloaded: {...prevState.itemsPreloaded, [itmObj.id]: itmObj},
			shown: false,
		}))
		this.props.onChange(itmObj)
	}

	render() {
		const style = {
			maxHeight: '25rem',
			minWidth: '15rem',
			maxWidth: '100%',
			overflowY: 'auto',
			display: 'block',
			padding: '0.3rem',
			marginTop:'-1px',
		}

		let name = '<loading ...>'
		if( this.props.itemId in this.state.itemsPreloaded && this.state.itemsPreloaded[this.props.itemId] )
			name = transDimItemNameShort(this.state.itemsPreloaded[this.props.itemId])

		return <div className="dropdown" tabIndex="0" ref={this.setWrapperRef}>
			<button className="form-control" onClick={e=>{e.preventDefault();this.toggle();}}>
				<span className="d-flex">
					<span className="flex-fill text-left text-truncate">
						{name}
					</span>
					<span>▼</span>
				</span>
			</button>
			{this.state.shown && <div className="dropdown-menu" style={style}>
				<DimBrowser
					dimKey={this.props.dimKey}
					itemId={this.props.itemId}
					onChange={itmObj=>this.change(itmObj)}
					searchPosition='start'
				/>
			</div>}
		</div>
	}
}