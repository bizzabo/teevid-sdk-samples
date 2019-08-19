import React, { Component, Fragment } from 'react';
import { connect } from 'react-redux';
import actions from '../ducks/actions';
import Stream from './Stream';

class Meeting extends Component {
  static defaultProps = {
    participants: []
  }
  state = {
    connected: false
  }

  render() {
    const { participants = [] } = this.props;

    const stream = participants.length ? participants.map((item, index) => 
      <Stream {...item} key={`stream_${index}`} />
    ) : <div className="no-remote-streams">
          There is no any stream
        </div>

    return (
      <Fragment>
        <div className='meeting'>
          { stream }
        </div>
      </Fragment>
    );
  }
}

const mapStateToProps = state => ({
  participants: state.participants
});

const mapDispatchToProps = {
  connect: actions.connect
};

export default connect(mapStateToProps, mapDispatchToProps)(Meeting);