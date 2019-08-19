import types from './actions/types';

export default (state={}, action) => {
  switch (action.type) {
    case types.CONNECT: 
      console.log('Reducer ', action.payload);
      return { ...state, ...action.payload };
    default:
      return state;
  }
}