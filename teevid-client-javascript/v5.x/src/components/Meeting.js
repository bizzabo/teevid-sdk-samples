import React, { Component } from 'react';
import { connect } from 'react-redux';
import Stream from './Stream';

class Meeting extends Component {
  componentWillUnmount() {
    TeeVidSdk.actions.disconnect();
  }
  render() {
    const { streams } = this.props;

    const renderStreams = Object.entries(streams).map(item => {
        const stream = item[1].stream;
        return (<Stream
          stream={item[1]}
          id={stream.id}
          key={stream.id} />);
      }
    );

    return (
      <div className='meeting'>
        { renderStreams }
      </div>
    );
  }
}

const mapStateToProps = state => ({
  streams: state.teevid.meeting.streams
});

export default connect(mapStateToProps)(Meeting);