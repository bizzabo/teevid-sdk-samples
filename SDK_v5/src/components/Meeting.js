import React, { Component, Fragment } from 'react';
import { connect } from 'react-redux';
import Stream from './Stream';

class Meeting extends Component {
  static defaultProps = {
    participants: []
  }

  render() {
    const { participant = [], remote } = this.props;
    console.log(this.props);
    
    const streams = Object.keys(remote).length ? Object.values(remote).map(
      (item, index) => <Stream id={item} key={`stream_remote_${index}`} />
    ) : null;

    const localStream = participant ? Object.values(participant).map((item, index) => 
      <Stream id={item} key={`stream_${index}`} />
    ) : <div className="no-remote-streams">
          There is no any stream
        </div>

    return (
      <Fragment>
        <div className='meeting'>
          { localStream }
          { streams }
        </div>
      </Fragment>
    );
  }
}

const mapStateToProps = state => ({
  participant: state.teevid.meeting.participant,
  remote: state.teevid.meeting.remoteParticipants
});

export default connect(mapStateToProps)(Meeting);