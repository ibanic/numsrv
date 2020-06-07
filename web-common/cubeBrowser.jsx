import React, { Component } from 'react'
import style from './cubeBrowser.css'
import DimCombo from './dimCombo';
import DimBrowser from './dimBrowser';
import { isIntUnits, currentLang } from './trans'
import { convertUnits, usUnitName } from './units'
import { transName } from './trans'






export default class CubeBrowser extends Component {
	constructor(props) {
		super(props)
		this.state = {
			meta: null,
			dimX: false,
			dimY: false,
			dimItems: [],
			dimItemsX: [],
			dimItemsY: [],
			values: {},  // only numbers
			
			mouseOverX: false,
			mouseOverY: false,
			selecting: false,
			selectionStartX: false,
			selectionStartY: false,
			selectionEndX: false,
			selectionEndY: false,
		}

		this.handleCopy = this.handleCopy.bind(this)
		this.dimItemsChangedX = this.dimItemsChangedX.bind(this)
		this.dimItemsChangedY = this.dimItemsChangedY.bind(this)
	}


	static getDerivedStateFromProps(nextProps, prevState) {
		if( !prevState.meta || nextProps.cubeKey != prevState.meta.key )
			return {meta:null}
		return null
	}


	componentDidUpdate(prevProps) {
		if( this.props.cubeKey != prevProps.cubeKey )
			this.loadMeta()
	}


	componentDidMount() {
		this.loadMeta()
		document.addEventListener('copy', this.handleCopy)
	}


	componentWillUnmount() {
		document.removeEventListener('copy', this.handleCopy)
	}


	async loadMeta() {
		// clear
		this.setState({
			meta: null,
			dimX: false,
			dimY: false,
			dimItems: [],
			dimItemsX: [],
			dimItemsY: [],
			values: {},
			
			mouseOverX: false,
			mouseOverY: false,
			selecting: false,
			selectionStartX: false,
			selectionStartY: false,
			selectionEndX: false,
			selectionEndY: false,
		})
		// load meta
		let r, js
		r = await fetch(API_URL+'/api/cubes/'+this.props.cubeKey)
		js = await r.json()
		const change = { meta: js }
		if( js.dims.length >= 2 ) {
			change.dimY = 1
			change.dimX = 0
		}
		else if( js.dims.length == 1 ) {
			change.dimY = 0
			change.dimX = false
		}
		else {
			change.dimY = false
			change.dimX = false
		}
		change.dimItems = []
		for( let i=0; i<js.dims.length; ++i )
			change.dimItems.push(0)
		this.setState(change)
	}


	setDimItm(dimIdx, itmObj) {
		this.resetSelection()
		this.setState(prevState => {
			const arr = [...prevState.dimItems]
			arr[dimIdx] = itmObj.id
			return { dimItems: arr }
		}, () => {
			this.checkForData()
			.then(() => {
				if( this.props.onViewChange )
					this.props.onViewChange(this.currentView())
			})
		})
	}


	dimItemsChangedX(arr) {
		this.resetSelection()
		this.setState({dimItemsX:arr}, ()=>{
			this.checkForData()
			.then(() => {
				if( this.props.onViewChange )
					this.props.onViewChange(this.currentView())
			})
		})
	}
	dimItemsChangedY(arr) {
		this.resetSelection()
		this.setState({dimItemsY:arr}, ()=>{
			this.checkForData()
			.then(() => {
				if( this.props.onViewChange )
					this.props.onViewChange(this.currentView())
			})
		})
	}


	renderResult(idX, idY) {
		const arr = [...this.state.dimItems]
		if( this.state.dimX !== false )
			arr[this.state.dimX] = idX
		arr[this.state.dimY] = idY
		let n = this.state.values[this.makeIndex(arr)]
		if( n === undefined )
			return ''
		if( !isIntUnits() )
			n = convertUnits(n, this.state.meta.unit)
		const decimals = this.state.meta.integers ? 0 : 2
		return n.toLocaleString(currentLang(), {minimumFractionDigits: decimals, maximumFractionDigits: decimals})
	}


	makeIndex(arr) {
		return arr.join('/')
	}


	async checkForData() {
		const load = []
		const loadIdx = []
		for( const objY of this.state.dimItemsY ) {
			if(!objY) {
				continue
			}
			if( this.state.dimX !== false ) {
				for( const objX of this.state.dimItemsX ) {
					if(!objX) {
						continue
					}
					const arr = [...this.state.dimItems]
					arr[this.state.dimX] = objX.id
					arr[this.state.dimY] = objY.id
					const idx = this.makeIndex(arr)
					const n = this.state.values[idx]
					if( n === undefined ) {
						load.push(arr)
						loadIdx.push(idx)
					}
				}
			}
			else {
				const arr = [...this.state.dimItems]
				arr[this.state.dimY] = objY.id
				const idx = this.makeIndex(arr)
				const n = this.state.values[idx]
				if( n === undefined ) {
					load.push(arr)
					loadIdx.push(idx)
				}
			}
		}
		if( load.length == 0 )
			return
		// load
		let r, js
		r = await fetch(API_URL+'/api/cubes/'+this.state.meta.key+'/cellValuesById', {method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(load)})
		js = await r.json()
		this.setState(prevState => {
			const vals = {...prevState.values}
			for( let i=0; i<loadIdx.length; ++i )
				vals[loadIdx[i]] = js[i]
			return { values: vals }
		})
	}


	renderDimSelection(d, idx) {
		if( idx === this.state.dimX )
			return <p>X</p>
		if( idx === this.state.dimY )
			return <p>Y</p>
		return <DimCombo dimKey={d.key} itemId={this.state.dimItems[idx]} onChange={itmObj=>this.setDimItm(idx,itmObj)} />
	}


	selectX(idx) {
		this.setState(prevState => {
			if( prevState.dimX === idx )
				return null
			if( prevState.dimY == idx )
				return {dimX:idx, dimY:prevState.dimX, dimItemsX:[], dimItemsY:[]}
			return {dimX:idx, dimItemsX:[]}
		})
	}


	selectY(idx) {
		this.setState(prevState => {
			if( prevState.dimY === idx )
				return null
			if( prevState.dimX == idx )
				return {dimY:idx, dimX:prevState.dimY, dimItemsX:[], dimItemsY:[]}
			return {dimY:idx, dimItemsY:[]}
		})
	}


	mouseEnter(idxX, idxY) {
		this.setState({mouseOverX: idxX, mouseOverY: idxY})
	}
	mouseLeave(idxX, idxY) {
		this.setState(prevState=>{
			if( prevState.mouseOverX != idxX || prevState.mouseOverY != idxY )
				return null
			return { mouseOverX: false, mouseOverY: false }
		})
	}
	mouseDown(idxX, idxY) {
		this.setState({selecting:true, selectionStartX:idxX, selectionStartY:idxY, selectionEndX:idxX, selectionEndY:idxY})
	}
	mouseUp(idxX, idxY) {
		this.setState(prevState => {
			if( !prevState.selecting )
				return null
			if( prevState.selectionStartX==idxX && prevState.selectionStartY==idxY )
				return {selecting:false, selectionStartX:idxX, selectionStartY:idxY, selectionEndX:idxX, selectionEndY:idxY}
			let startX = prevState.selectionStartX
			let startY = prevState.selectionStartY
			let endX = idxX
			let endY = idxY
			if( startX > endX ) { const tmp=startX; startX=endX; endX=tmp; }
			if( startY > endY ) { const tmp=startY; startY=endY; endY=tmp; }
			return {selecting:false, selectionStartX:startX, selectionStartY:startY, selectionEndX:endX, selectionEndY:endY}
		}, ()=>{
			if( this.props.onSelect ) {
				const dt = this.currentSelection()
				this.props.onSelect(dt)
			}
		})
	}
	mouseMove(idxX, idxY) {
		this.setState(prevState => {
			if( prevState.selecting )
				return { selectionEndX:idxX, selectionEndY: idxY }
			return null
		})
	}
	currentView(startX=0, startY=0, endX=-1, endY=-1) {
		if( endX == -1 )
			endX = this.state.dimItemsX.length-1
		if( endY == -1 )
			endY = this.state.dimItemsY.length-1
		const dt = []
		for( let idxY=startY; idxY<=endY; ++idxY ) {
			const dt2 = []
			for( let idxX=startX; idxX<=endX; ++idxX ) {
				const arr = [...this.state.dimItems]
				if( this.state.dimX !== false )
					arr[this.state.dimX] = this.state.dimItemsX[idxX].id
				arr[this.state.dimY] = this.state.dimItemsY[idxY].id
				const val = this.state.values[this.makeIndex(arr)]
				dt2.push(val)
			}
			dt.push(dt2)
		}

		return {
			data: dt,
			dimItems: this.state.dimItems,
			dimX: this.state.dimX,
			dimY: this.state.dimY,
			dimItemsX: this.state.dimItemsX.slice(startX,endX+1),
			dimItemsY: this.state.dimItemsY.slice(startY,endY+1),
		}
	}
	currentSelection() {
		if( this.state.selectionStartX === false )
			return null

		return this.currentView(
			this.state.selectionStartX,
			this.state.selectionStartY,
			this.state.selectionEndX,
			this.state.selectionEndY
		)
	}
	handleCopy(e) {
		if( this.state.selectionStartX !== false && e.target && e.target.closest('.cube-cell') ) {
			e.preventDefault()

			const isWindows = navigator.platform.indexOf('Win') > -1
			const lineSep = isWindows ? '\r\n' : '\n'

			const dt = this.currentSelection()
			const txt = dt.data.map(arr => arr.map(val => {
				if( val === null || val === undefined )
					return ''
				return val.toLocaleString('fullwide', {useGrouping:false, minimumFractionDigits:(this.state.meta.integers?0:2)})
			}).join('\t')).join(lineSep)

			e.clipboardData.setData('text/plain', txt);
		}
	}
	resetSelection() {
		const selectionChanged = this.state.selectionStartX !== false
		this.setState({selectionStartX:false,selectionStartY:false,selectionEndX:false,selectionEndY:false}, ()=>{
			if( selectionChanged && this.props.onSelect )
				this.props.onSelect(null)
		})
	}


	renderCell(objX, objY, idxX, idxY) {
		const stl = {}
		if( this.state.selectionStartX !== false ) {
			let inRangeX, inRangeY
			if( this.state.selectionStartX <= this.state.selectionEndX && idxX >= this.state.selectionStartX && idxX <= this.state.selectionEndX )
				inRangeX = true
			if( this.state.selectionStartX > this.state.selectionEndX && idxX <= this.state.selectionStartX && idxX >= this.state.selectionEndX )
				inRangeX = true
			if( this.state.selectionStartY <= this.state.selectionEndY && idxY >= this.state.selectionStartY && idxY <= this.state.selectionEndY )
				inRangeY = true
			if( this.state.selectionStartY > this.state.selectionEndY && idxY <= this.state.selectionStartY && idxY >= this.state.selectionEndY )
				inRangeY = true
			if( inRangeX && inRangeY )
				stl.backgroundColor = 'rgba(0,0,0,0.15)'
		}
		else {
			if( this.state.mouseOverX === idxX || this.state.mouseOverY === idxY ) {
				stl.backgroundColor = 'rgba(0,0,0,0.08)'
			}
		}
		return <td
			key={objX?objX.id:0}
			onMouseDown={()=>this.mouseDown(idxX,idxY)}
			onMouseUp={()=>this.mouseUp(idxX,idxY)}
			onMouseMove={()=>this.mouseMove(idxX,idxY)}
			onMouseEnter={()=>this.mouseEnter(idxX,idxY)}
			onMouseLeave={()=>this.mouseLeave(idxX,idxY)}
			style={stl}
		>
			{this.renderResult(objX?objX.id:0, objY.id)}
		</td>
	}



	render() {
		if(!this.state.meta)
			return 'loading ...'
		
		return <div className='cube'>
			<div className="cube-dims">
				{this.state.meta.dims.map((d,idx) => <div key={idx}>
					<div>
						<strong>{transName(d.name)}</strong>
						{this.state.dimX !== false && <>
							<a href="#" onClick={e=>{e.preventDefault();this.selectX(idx);}} className="ml-2">X</a>
							<a href="#" onClick={e=>{e.preventDefault();this.selectY(idx);}} className="ml-2">Y</a>
						</>}
					</div>
					{this.renderDimSelection(d,idx)}
				</div>)}
			</div>
			<table>
				<tbody>
					<tr>
						<td style={{minWidth:'5rem'}}></td>
						<td>
							{this.state.dimX !== false && <DimBrowser
								dimKey={this.state.meta.dims[this.state.dimX].key}
								horizontal={true}
								expandCallback={this.dimItemsChangedX}
								searchPosition='end'
								searchMultiple={true}
							/>}
						</td>
					</tr>
					<tr>
						<td>
							{this.state.dimY !== false && <DimBrowser
								dimKey={this.state.meta.dims[this.state.dimY].key}
								expandCallback={this.dimItemsChangedY}
								searchPosition='end'
								searchMultiple={true}
							/>}
						</td>
						<td>
							<table className='cube-cell'>
								<tbody>
									{this.state.dimItemsY.map((objY,idxY) => <tr key={objY ? objY.id : idxY}>
									{this.state.dimX!==false && this.state.dimItemsX.map((objX,idxX) => {
										if( !objX || !objY ) {
											return <td key={idxX}></td>
										}
										return this.renderCell(objX,objY,idxX,idxY)
									})}
									{this.state.dimX===false && this.renderCell(null,objY,0,idxY)}
									</tr>)}
								</tbody>
							</table>
						</td>
					</tr>
				</tbody>
			</table>
			
		</div>
	}
}