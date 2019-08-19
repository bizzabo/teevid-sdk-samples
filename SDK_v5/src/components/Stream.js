import React, { Component } from 'react';
import id from 'uniqid';

class Stream extends Component {
  constructor(props) {
    super(props);
    this.streamId = `stream_${id()}`
  }

  render () {
    console.log('Stream ', this.props);

    return (
      <div className='stream'  
        id={this.streamId}
        key={this.streamId}
      ></div>
    );
  }
}

export default Stream;