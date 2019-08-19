import React from 'react';
import logo from './logo.png';
import './App.css';
import Meeting from './components/Meeting';
import Join from './components/Join';
import { Provider } from 'react-redux';
import { Route, Switch } from 'react-router-dom';
import { ConnectedRouter } from 'react-router-redux';
import { history, store } from './reduxRoot';
// import actions from './ducks/actions';
console.log(store);
export default () =>
  (<Provider store={store}>
     {/* <ConnectedRouter history={history}> */}
      <div className="App">
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
        </header>
        {/* <Switch>
          <Route path='/join' component={Join} />
          <Route path='/meeting' component={Meeting} />
        </Switch> */}
      </div>
    {/* </ConnectedRouter> */}
  </Provider>)


// export default App;
