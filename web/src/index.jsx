import React, {Component} from 'react'
import ReactDOM from 'react-dom'
import { BrowserRouter, Switch, Route } from 'react-router-dom'
import Home from './home'
import style from './style.css'
import DimDetails from './dimDetails'
import CubeDetails from './cubeDetails'
import { setForceUpdate } from '../../web-common/trans'

class App extends Component {
	constructor(props) {
		super(props)
	}
	componentDidMount() {
		setForceUpdate(this.forceUpdate.bind(this))
	}
	componentWillUnmount() {
		setForceUpdate(null)
	}
	render() {
	return <BrowserRouter basename='/numsrv/'>
		<Switch>
			<Route exact path='/' component={Home} />
			<Route path='/dims/:dimKey' component={DimDetails} />
			<Route path='/cubes/:cubeKey' component={CubeDetails} />
		</Switch>
	</BrowserRouter>
	}
}

const domContainer = document.querySelector('#app');
ReactDOM.render(<App/>, domContainer);
