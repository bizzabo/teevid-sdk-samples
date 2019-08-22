import React, { Component } from 'react';

class Stream extends Component {

  componentDidMount() {
    const { id, stream } = this.props;
    stream.play(id);
  }

  render () {
    const { id } = this.props;
    
    return (
      <div 
        className='stream'  
        id={id}
      >
      </div>
    );
  }
}

export default Stream;