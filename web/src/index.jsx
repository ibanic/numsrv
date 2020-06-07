import React, {Component} from 'react'
import ReactDOM from 'react-dom'
import { BrowserRouter, Switch, Route } from 'react-router-dom'
import Home from './home'
//import Bootstrap from 'bootstrap/dist/css/bootstrap.css'
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
	return <BrowserRouter>
		<Switch>
			<Route exact path='/' component={Home} />
			<Route path='/dims/:dimKey' component={DimDetails} />
			<Route path='/cubes/:cubeKey' component={CubeDetails} />
			{/*
			<Route path='/edit/:id' render={(props) => <Edit {...props} preview={false} />} />
			<Route path='/preview/:id' render={(props) => <Edit {...props} preview={true} />} />
			*/}
		</Switch>
	</BrowserRouter>
	}
}

const domContainer = document.querySelector('#app');
ReactDOM.render(<App/>, domContainer);


// router tutorial
// https://medium.com/@pshrmn/a-simple-react-router-v4-tutorial-7f23ff27adf

//<Route path='/edit/:id' component={Edit} preview={false} />
//<Route path='/preview/:id' component={Edit} preview={true} />
