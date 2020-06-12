$(document).ready(function () {
  var roomId = 'ENTER_ROOM_ID',
    username = 'ENTER_YOUR_NAME';

  function showError(err) {
    $('#error').text(err);
  }

  // initialize the SDK with your key
  TeeVid.init('ENTER_SDK_KEY_HERE');
  TeeVid.server = "ENTER_SERVER_ADDRESS_HERE";

  TeeVid.participants.create(roomId, username, null)
    .catch(function (error) {
      showError('connection error - ' + error.code + ' ' + error.message);
    })
    .then(function (connection) {
      if (!connection) return;
      window.connection = connection;

      // event listeners to show others' videos
      connection.addEventListener('stream-subscribed', function (streamEvent) {
        var stream = streamEvent.stream,
          elementId = 'stream_' + stream.getID();

        // check whether this streams is indeed a media stream
        if (!stream.hasAudio() && !stream.hasVideo() && !stream.hasScreen())
          return;

        // create HTML element to contain stream and play it.
        $('<div>').attr({ id: elementId, title: stream.participant.username }).appendTo('#streams');
        stream.play(elementId);
      });
      connection.addEventListener('stream-removed', function (streamEvent) {
        // removing the corresponding container. The video is already stopped.
        $('#stream_' + streamEvent.stream.getID()).remove();
      });

      // create and initialize my local video
      var localStream = TeeVid.Stream({ video: true, audio: true });
      connection.addEventListener('room-connected', function () {
        localStream.init();
      });
      // play locally and publish my video
      localStream.addEventListener('access-accepted', function () {
        $('#stream_local').attr({ title: connection.username });
        localStream.play('stream_local');
        connection.publish(localStream, function (id, error) {
          if (error)
            showError('publish error - ' + error);
        });
      });
      localStream.addEventListener('access-denied', function () {
        showError('unable to access a local camera');
      });

      function d(evt) {
        showError('room disconnected - ' + evt.message);
      }
      connection.addEventListener('room-error', d);
      connection.addEventListener('room-disconnected', d);

      connection.connect();
    });
});
