/** eslint-disable */
import { put, takeEvery, select, all } from 'redux-saga/effects';
import { push } from 'react-router-redux';
import types from '../ducks/actions/types';
const actionTypes = TeeVidSdk.actionTypes;
const sdkActions = TeeVidSdk.cleanActions;

export const Mute = {
  NO_DEVICE: -1,
  UNMUTED: 0,
  MUTED: 1,
  MUTED_BY_MODER: 2,
};

const createLocalStream = (
  connection,
  constraints) => { // checking for audio and video devices
  if (connection && connection.state === 2) {
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
    yield put(TeeVidSdk.actions.createParticipant(room, username, pin));
  } else {
    if (!username) TeeVidSdk.actions.generalError({ message: 'connection-username-err' });
    if (!room) TeeVidSdk.actions.generalError({ message: 'connection-room-id-err' });
  }
}

// there might be some additional connection info in the options
function* connectionWorker({ payload: { options } }) {
  let connOptions = {};
  if (Object.keys(options?.iceServers || {}).length) {
    connOptions.iceServers = options?.iceServers;
  }
  yield TeeVidSdk.actions.createConnection(connOptions);
}

function* moderationConnect({ payload: { room } }) {
  if (room) {
    yield TeeVidSdk.actions.connectToModeration(room);
  } else {
    if (!room) TeeVidSdk.actions.generalError({ message: 'connection-room-id-err' });
  }
}

export function* connectSaga() {
  yield all([
    // connect to the moderation socket
    takeEvery(types.CONNECT, moderationConnect),
    // socket connected, request join
    takeEvery(actionTypes.MODERATION_SOCKET_CREATED,
      function* () {
        const userName = 'username';
        yield put(sdkActions.requestJoin(userName, '', {}));
      }),
    // join accepted? create connection
    takeEvery(actionTypes.SDK_JOIN_ACCEPTED, connectionWorker),
    // prepare connection: add event handlers and such
    takeEvery(actionTypes.CREATE_CONNECTION_SUCCESS,
      function* ({ payload: connection }) {
        yield put(TeeVidSdk.cleanActions.connect());
      }),
    // connect
    takeEvery(actionTypes.CONNECTION_READY, function* () {
      const connection = yield select((state) => state.teevid.meeting.connection);
      connection.connect();
    }),
    // room is connected
    takeEvery(actionTypes.ROOM_READY, function* () {
      // get constraints
      const connection = yield select((state) => state.teevid.meeting.connection);
      const options = yield select((state) => state.teevid.meeting.participant.status);
      const room = yield select((state) => state.teevid.meeting.room);
      // create streams
      try {
        const stream = yield createLocalStream(connection,
          {
            audio: options.audio !== Mute.NO_DEVICE,
            video: options.video !== Mute.NO_DEVICE
          }, room)
        yield all([
          put(sdkActions.localStreamAdded(stream)),
          put(sdkActions.setConnectionState(2)),
        ]);
        // publish streams
      } catch (error) {
        console.error(error);
        TeeVidSdk.actions.disconnect();
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
    yield put(push('/join'));
  }
  );
}
