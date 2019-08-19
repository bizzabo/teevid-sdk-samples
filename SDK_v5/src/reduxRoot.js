import { createBrowserHistory as createHistory } from 'history';
import { routerMiddleware, routerReducer } from 'react-router-redux';
import { reducer as formReducer } from 'redux-form';
import { applyMiddleware, combineReducers, compose, createStore } from 'redux';
import createSagaMiddleware from 'redux-saga';
import saga from './saga/index';
import hostname from './env.js';
// const TeeVidSdk = window.TeeVidSdk || {};
const TeeVidSdk = require('./sdk/sdk');

export const history = createHistory();
const composeEnhancers = window.__REDUX_DEVTOOLS_EXTENSION_COMPOSE__ || compose;

const sagaMw = createSagaMiddleware({
  onError: (error) => {
    console.error('@ Saga error: ', error);
  }
});

const routerMw = routerMiddleware(history);

export const store =  createStore(
  combineReducers({
    teevid: TeeVidSdk.reducer,
    form: formReducer,
    router: routerReducer,
  }),  composeEnhancers(applyMiddleware(sagaMw, routerMw)));

sagaMw.run(saga);
if(TeeVidSdk.saga)
  sagaMw.run(TeeVidSdk.saga);
if(TeeVidSdk.init)
  TeeVidSdk.init(hostname, store);