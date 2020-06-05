package com.teevid.sample;

import android.app.Application;

import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;
import com.google.android.gms.security.ProviderInstaller;

import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;

import javax.net.ssl.SSLContext;

public class SampleApplication extends Application {

    private static SampleApplication instance;
    private UserPreferences userPreferences;

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;
        installSecurity();

        userPreferences = new SharedUserPreferences(this);
    }

    // Required for Android 4.4
    private void installSecurity() {
        try {
            ProviderInstaller.installIfNeeded(getApplicationContext());
            SSLContext sslContext;
            sslContext = SSLContext.getInstance("TLSv1.2");
            sslContext.init(null, null, null);
            sslContext.createSSLEngine();
        } catch (GooglePlayServicesRepairableException | GooglePlayServicesNotAvailableException
                | NoSuchAlgorithmException | KeyManagementException e) {
            e.printStackTrace();
        }
    }

    public static SampleApplication getInstance() {
        return instance;
    }

    public UserPreferences getUserPreferences() {
        return userPreferences;
    }
}
