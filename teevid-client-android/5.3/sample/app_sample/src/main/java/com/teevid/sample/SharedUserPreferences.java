package com.teevid.sample;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

public class SharedUserPreferences implements UserPreferences {

    private static final String PREF_SERVER = "PREF_SERVER";
    private static final String PREF_ROOM_ID = "PREF_ROOM_ID";
    private static final String PREF_USERNAME = "PREF_USERNAME";

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
}
