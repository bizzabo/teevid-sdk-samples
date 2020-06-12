import { all } from 'redux-saga/effects';

import { 
  connectSaga, 
  disconnectedSaga, 
  disconnectSaga,
} from './connection';

function* rootApiSaga() {
  yield all ([
    connectSaga(),
    disconnectSaga(),
    disconnectedSaga()
  ]);
}

export default rootApiSaga;