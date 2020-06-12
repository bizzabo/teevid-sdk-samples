import React from 'react';
import logo from './logo.png';
import './App.css';
import Meeting from './components/Meeting';
import Join from './components/Join';
import { Provider } from 'react-redux';
import { Route, BrowserRouter, Switch } from 'react-router-dom';
import { history, store } from './reduxRoot';

export default () =>
  (<Provider store={store}>
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
      </header>
      <BrowserRouter history={history}>
        <Switch>
          <Route exact path="/" component={Join} />
          <Route exact path='/join' component={Join} />
          <Route exact path='/meeting' component={Meeting} />
        </Switch>
      </BrowserRouter>
    </div>
  </Provider>)
