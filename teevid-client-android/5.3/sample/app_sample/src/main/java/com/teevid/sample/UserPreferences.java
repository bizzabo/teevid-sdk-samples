package com.teevid.sample;

import com.teevid.sdk.constant.CameraProvider;

public interface UserPreferences {

    String getServer();

    void setServer(String server);

    String getRoomId();

    void setRoomId(String roomId);

    String getUsername();

    void setUsername(String username);

    String getInvitationLink();

    void setInvitationLink(String invitationLink);

    @CameraProvider
    int getCamera();

    void setCamera(@CameraProvider int defaultCamera);

    String getPassword();

    void setPassword(String password);

    String getSdkToken();

    void setSdkToken(String sdkToken);
}
