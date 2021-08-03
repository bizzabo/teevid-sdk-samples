import React, { useState, useRef } from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom';
import actions from '../ducks/actions';
import Button from '@material-ui/core/Button';
import { makeStyles } from '@material-ui/core/styles';
import FormControl from '@material-ui/core/FormControl';
import InputLabel from '@material-ui/core/InputLabel';
import OutlinedInput from '@material-ui/core/OutlinedInput';
import Paper from '@material-ui/core/Paper';

const useStyles = makeStyles(theme => ({
  container: {
    display: 'flex',
    flexWrap: 'wrap',
  },
  formControl: {
    margin: theme.spacing(1)
  },
  root: {
    padding: theme.spacing(3, 2),
    display: 'flex',
    position: 'absolute',
    top: '50%',
    left: '50%',
    transform:' translate(-50%, -50%)',
    background: '#192d47bf',
    color: 'white',
    fontWeight: 'bold'
  }
}));

const Join = (props) => {
  const [username, setName] = useState('');
  const [room, setRoom] = useState('');
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
    <Paper className={classes.root}>
      <FormControl className={`${classes.formControl} join__inputs`} variant="outlined">
        <InputLabel ref={labelRef} htmlFor="component-outlined">
          Participant name
        </InputLabel>
        <OutlinedInput
          value={username}
          onChange={handleChange}
          labelWidth={labelWidth}
          placeholder='Participant Name'
        />
      </FormControl>
      <FormControl className={`${classes.formControl} join__inputs`} variant="outlined">
        <InputLabel ref={labelRef} htmlFor="component-outlined">
          Existing room ID
        </InputLabel>
        <OutlinedInput
          value={room}
          onChange={setRoomName}
          labelWidth={labelWidth}
          placeholder='Existing room ID'
        />
      </FormControl>
      <Button variant="contained" color="primary" className='join' onClick={join}>
        Join
      </Button>
    </Paper>
  );
}

const mapDispatchToProps = {
  connect: actions.connect
}
export default connect(null, mapDispatchToProps)(withRouter(Join));