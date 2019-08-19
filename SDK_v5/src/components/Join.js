import React, { Fragment, useState, useRef } from 'react';
import { connect } from 'react-redux';
// import { push } from 'react-router-redux';
import { withRouter } from 'react-router-dom';
import actions from '../ducks/actions';
import Button from '@material-ui/core/Button';
import { makeStyles } from '@material-ui/core/styles';
import FormControl from '@material-ui/core/FormControl';
import InputLabel from '@material-ui/core/InputLabel';
import OutlinedInput from '@material-ui/core/OutlinedInput';

const useStyles = makeStyles(theme => ({
  container: {
    display: 'flex',
    flexWrap: 'wrap',
  }, 
  formControl: {
    margin: theme.spacing(1),
  },
}));

const Join = (props) => {
  const [username, setName] = useState('Your name');
  const [room, setRoom] = useState('Nas10ka');
  const [labelWidth] = useState(0);
  const handleChange = e => setName(e.target.value);
  const setRoomName = e => setRoom(e.target.value);
  const labelRef = useRef(null);
  const classes = useStyles();
  const join = () => {
    props.connect(username, room, '', true);
    return props.history.push('/meeting');
  };

  return (
    <Fragment>
      <FormControl className={classes.formControl} variant="outlined">
        <InputLabel ref={labelRef} htmlFor="component-outlined">
          Your name
        </InputLabel>
        <OutlinedInput
          value={username}
          onChange={handleChange}
          labelWidth={labelWidth}
          placeholder='Your name'
        />
      </FormControl>
      <FormControl className={classes.formControl} variant="outlined">
        <InputLabel ref={labelRef} htmlFor="component-outlined">
          Room
        </InputLabel>
        <OutlinedInput
          value={room}
          onChange={setRoomName}
          labelWidth={labelWidth}
          placeholder='Room'
        />
      </FormControl>
      <Button variant="contained" color="primary" className='join' onClick={join}>
        Join
      </Button> 
    </Fragment>
  );
}

const mapDispatchToProps = {
  connect: actions.connect,
  goToMeeting: () => push('/meeting')
}
export default connect(null, mapDispatchToProps)(withRouter(Join));