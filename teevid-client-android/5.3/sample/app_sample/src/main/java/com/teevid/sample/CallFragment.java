package com.teevid.sample;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.teevid.sdk.TeeVidClient;
import com.teevid.sdk.TeeVidEventListener;
import com.teevid.sdk.log.LogLevel;
import com.teevid.sdk.view.TeeVidView;

public class CallFragment extends Fragment {

    private static final String TAG = "CallFragment";

    private TeeVidClient client;

    public CallFragment() {

    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_call, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        TeeVidView viewTeeVid = view.findViewById(R.id.view_teevid);
        ToggleButton btnCamera = view.findViewById(R.id.btn_camera);
        ToggleButton btnLocalVideo = view.findViewById(R.id.btn_local_video);
        ToggleButton btnMicrophone = view.findViewById(R.id.btn_microphone);

        UserPreferences preferences = SampleApplication.getInstance().getUserPreferences();
        String server = preferences.getServer();
        String roomId = preferences.getRoomId();
        String username = preferences.getUsername();

        client = new TeeVidClient.Builder(getContext(), "token") // TODO Token
                .addListener(getEventListener())
                .setLogLevel(LogLevel.DEBUG)
                .build();
        client.setView(viewTeeVid);

        client.connect(roomId, server, username);

        btnCamera.setOnClickListener(v -> onCameraButtonClicked(btnCamera));
        btnLocalVideo.setOnClickListener(v -> onLocalVideoButtonClicked(btnLocalVideo));
        btnMicrophone.setOnClickListener(v -> onMicrophoneButtonClicked(btnMicrophone));
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        client.disconnect();
    }

    private void onCameraButtonClicked(ToggleButton button) {
        boolean enabled = button.isChecked(); // Button's state changes before onClick is fired
        if (enabled) {
            client.resumeVideo();
        } else {
            client.stopVideo();
        }
    }

    private void onLocalVideoButtonClicked(ToggleButton button) {
        boolean enabled = button.isChecked();
        if (enabled) {
            client.showLocalVideo();
        } else {
            client.hideLocalVideo();
        }
    }

    private void onMicrophoneButtonClicked(ToggleButton button) {
        boolean enabled = button.isChecked();
        if (enabled) {
            client.unmute();
        } else {
            client.mute();
        }
    }

    private TeeVidEventListener getEventListener() {
        return new TeeVidEventListener() {

            @Override
            public void onReceiveError(Throwable throwable) {
                Log.e(TAG, "onReceiveError: ", throwable);
                Toast.makeText(getContext(), throwable.getMessage(), Toast.LENGTH_LONG).show();
            }
        };
    }
}
