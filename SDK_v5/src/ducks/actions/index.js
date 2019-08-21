import types from './types';

export default Object.freeze({
  connect: (username, room, pin, connectAnyway) => ({
    type: types.CONNECT,
    payload: { username, room, pin, connectAnyway }
  }),
});