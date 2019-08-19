import types from './types';

export default Object.freeze({
  connect: payload => ({
    type: types.CONNECT,
    payload
  }),

});