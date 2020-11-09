package com.teevid.sample;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.preference.PreferenceManager;

import com.teevid.sdk.constant.CameraProvider;

public class SharedUserPreferences implements UserPreferences {

    private static final String PREF_SERVER = "PREF_SERVER";
    private static final String PREF_ROOM_ID = "PREF_ROOM_ID";
    private static final String PREF_USERNAME = "PREF_USERNAME";
    private static final String PREF_INVITATION_LINK = "PREF_INVITATION_LINK";
    private static final String PREF_CAMERA = "PREF_CAMERA";
    private static final String PREF_PASSWORD = "PREF_PASSWORD";
    private static final String PREF_SDK_TOKEN = "PREF_SDK_TOKEN";

    private SharedPreferences prefs;

    public SharedUserPreferences(Context context) {
        prefs = PreferenceManager.getDefaultSharedPreferences(context);
    }

    @Override
    public String getServer() {
        return prefs.getString(PREF_SERVER, null);
    }

    @Override
    public void setServer(String server) {
        prefs.edit()
                .putString(PREF_SERVER, server)
                .apply();
    }

    @Override
    public String getRoomId() {
        return prefs.getString(PREF_ROOM_ID, null);
    }

    @Override
    public void setRoomId(String roomId) {
        prefs.edit()
                .putString(PREF_ROOM_ID, roomId)
                .apply();
    }

    @Override
    public String getUsername() {
        return prefs.getString(PREF_USERNAME, null);
    }

    @Override
    public void setUsername(String username) {
        prefs.edit()
                .putString(PREF_USERNAME, username)
                .apply();
    }

    @Override
    public String getInvitationLink() {
        return prefs.getString(PREF_INVITATION_LINK, null);
    }

    @Override
    public void setInvitationLink(String invitationLink) {
        prefs.edit()
                .putString(PREF_INVITATION_LINK, invitationLink)
                .apply();
    }

    @Override
    public int getCamera() {
        return prefs.getInt(PREF_CAMERA, CameraProvider.FRONT_FACING);
    }

    @Override
    public void setCamera(int defaultCamera) {
        prefs.edit()
                .putInt(PREF_CAMERA, defaultCamera)
                .apply();
    }

    @Override
    public String getPassword() {
        return prefs.getString(PREF_PASSWORD, null);
    }

    @Override
    public void setPassword(String password) {
        prefs.edit()
                .putString(PREF_PASSWORD, password)
                .apply();
    }

    @Override
    public String getSdkToken() {
        return prefs.getString(PREF_SDK_TOKEN, null);
    }

    @Override
    public void setSdkToken(String sdkToken) {
        prefs.edit()
                .putString(PREF_SDK_TOKEN, sdkToken)
                .apply();
    }
}
