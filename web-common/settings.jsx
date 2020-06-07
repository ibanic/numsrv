import React, { Component } from 'react'
import { currentLang, setCurrentLang, isIntUnits, setIntUnits } from './trans'

export default class Settings extends Component {
	constructor(props) {
		super(props)
		this.state = {open:false}
	}
	render() {
		return <div style={{position:'relative'}}>
			<button className="btn btn-outline-secondary" onClick={()=>this.setState(prevState=>({open:!prevState.open}))}>
				⚙️
			</button>
			{this.state.open && <div style={{position:'absolute', top:'2.5rem', right: 0, padding: '0.5rem', width:'7rem', backgroundColor:'#fff', border:'1px solid #999'}}>
				Language:
				<br/>
				<select value={currentLang()} onChange={e=>setCurrentLang(e.target.value)}>
					<option value='en'>EN</option>
					<option value='de'>DE</option>
					<option value='fr'>FR</option>
					<option value='sl'>SL</option>
				</select>
				<br/>
				Units:
				<br/>
				<select value={isIntUnits()?'int':'us'} onChange={e=>setIntUnits(e.target.value=='int')}>
					<option value='int'>International</option>
					<option value='us'>US</option>
				</select>
			</div>}
		</div>
	}
}