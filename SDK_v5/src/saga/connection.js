
import { put, takeEvery, select, all } from 'redux-saga/effects';
import { push } from 'react-router-redux';
import forEach from 'lodash.foreach';
import types from '../ducks/actions/types';
import actions from '../ducks/actions';
// import { actionTypes } from '../../sdk/sdk';
import TeeVidSdk from '../../sdk/sdk';
const actionTypes = TeeVidSdk.actionTypes;

const createLocalStream = (
  connection,
  mode = 'interactive',
  constraints) => { // checking for audio and video devices
  if (connection
    && connection.state === 2) {
    /** @type {TeeVid.Stream} */
    const localStream = TeeVidSdk.Stream(constraints);
    return initLocalStream(localStream)
      .then(stream => publishStream(stream, connection));
  }
  return Promise.resolve();
};

export const publishStream = (stream, connection) => {
  if (stream)
    return new Promise((resolve, reject) => {
      connection.publish(stream, {}, (id, error) => {
        if (error) {
          console.error('Publish error - ' + error);
          return reject(error);
        }
        return resolve(stream);
      });
    });
  return Promise.reject('no-stream-provided');
};

/**
 * Init and publish local stream
 * @param {TeeVid.Stream} stream 
 * @param {TeeVid.Connection} connection 
 * @param {Array} bandwidth 
 * @returns {Promise<TeeVid.Stream>}
 */
export const initLocalStream = (stream) =>
  new Promise((resolve, reject) => {
    // const { video, audio } = stream;
    stream.addEventListener('access-accepted', () => {
      if (!stream.stream) {
        return reject(new Error({ message: 'Stream is not ready or is already published' }));
      }
      return resolve(stream);
    });
    if (!stream.hasScreen()) {
      stream.addEventListener('access-denied', function (e) {
        return reject(e);
      });
    }
    stream.init();
  });

function* participantWorker({ payload: { room, username, pin } }) {
  if (username && room) {
    yield TeeVidSdk.actions.createParticipant(room, username, pin);
  } else {
    if (!username) TeeVidSdk.actions.generalError({ message: 'connection-username-err' });
    if (!room) TeeVidSdk.actions.generalError({ message: 'connection-room-id-err' });
  }
}

function* connectionWorker() {
  yield TeeVidSdk.actions.createConnection();
}

const connectionEventHandlers = {
  'bandwidth-alert': (e) => {
    // console.log('==>', e);
  }
};

function registerHandlers(connection) {
  forEach(connectionEventHandlers, (handler, eventName) => {
    connection.addEventListener(eventName, handler);
  });
}

export function* connectSaga() {
  // ? maybe use 'take' to make sure it runs only once?
  yield all([

    // create participant
    takeEvery(types.CONNECT, participantWorker),
    // create connection
    takeEvery(actionTypes.CREATE_PARTICIPANT_SUCCESS, connectionWorker),
    // prepare connection: add event handlers and such
    takeEvery(actionTypes.CREATE_CONNECTION_SUCCESS,
      function* ({ payload: connection }) {
        yield TeeVidSdk.actions.connect();
      }),
    // connect
    takeEvery(actionTypes.CONNECTION_READY, function* () {
      const connection = yield select((state) => state.teevid.meeting
        .connection);
      registerHandlers(connection);
      connection.connect();
    }),
    // room is connected
    takeEvery(actionTypes.ROOM_READY, function* () {
      // get constraints
      const connection = yield select((state) => state.teevid.meeting
        .connection);
      const options = yield select((state) => state.teevid.meeting
        .participant.status);
      const room = yield select((state) => state.teevid.meeting.room);
      // create streams
      try {
        yield createLocalStream(connection,
          null, { audio: options.hasMic, video: options.hasCam }, room)
          .then(stream => {
            TeeVidSdk.actions.localStreamAdded(stream);
            TeeVidSdk.actions.setConnectionState(2);
          });
        // publish streams
      } catch (error) {
        TeeVidSdk.actions.disconnect();
        // console.log('<> ERROR', error);
      }
    })

  ]);
}

export function* disconnectSaga() {
  yield takeEvery(types.CONNECTION_DISCONNECT, function* () {
    yield TeeVidSdk.actions.disconnect();
  });
}

export function* disconnectedSaga() {
  yield takeEvery(actionTypes.CONNECTION_DISCONNECTED, function* () {
    yield put(actions.clearRoomAudioContext());
      yield put(push('/join'));
    }
  );
}
