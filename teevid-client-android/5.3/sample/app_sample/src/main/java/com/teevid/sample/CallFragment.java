package com.teevid.sample;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.core.util.Consumer;
import androidx.fragment.app.Fragment;

import com.teevid.sdk.TeeVidClient;
import com.teevid.sdk.TeeVidEventListener;
import com.teevid.sdk.api.SdkErrors;
import com.teevid.sdk.log.LogLevel;
import com.teevid.sdk.view.BaseMeetingView;

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

        BaseMeetingView viewTeeVid = view.findViewById(R.id.view_meeting);
        ToggleButton btnCamera = view.findViewById(R.id.btn_camera);
        ToggleButton btnLocalVideo = view.findViewById(R.id.btn_local_video);
        ToggleButton btnMicrophone = view.findViewById(R.id.btn_microphone);
        ToggleButton btnCameraSwitch = view.findViewById(R.id.btn_camera_switch);

        UserPreferences preferences = SampleApplication.getInstance().getUserPreferences();
        String server = preferences.getServer();
        String roomId = preferences.getRoomId();
        String username = preferences.getUsername();
        int defaultCamera = preferences.getCamera();

        client = new TeeVidClient.Builder(getContext(), "token") // TODO Token
                .addListener(getEventListener())
                .setLogLevel(LogLevel.DEBUG)
                .build();
        client.setView(viewTeeVid);
        client.setDefaultCamera(defaultCamera);

        client.connect(roomId, server, username);

        btnCamera.setOnClickListener(v -> onCameraButtonClicked(btnCamera));
        btnLocalVideo.setOnClickListener(v -> onLocalVideoButtonClicked(btnLocalVideo));
        btnMicrophone.setOnClickListener(v -> onMicrophoneButtonClicked(btnMicrophone));
        btnCameraSwitch.setOnClickListener(v -> onSwitchCameraButtonClicked());
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

    private void onSwitchCameraButtonClicked() {
        client.switchCamera();
    }

    private void showEnterPinDialog(Consumer<String> pinConsumer) {
        View view = LayoutInflater.from(getContext()).inflate(R.layout.dialog_edit_text, null);
        EditText editText = view.findViewById(R.id.et_input);
        editText.setHint(R.string.enter_pin);

        AlertDialog dialog = new AlertDialog.Builder(getContext())
                .setTitle(R.string.enter_pin_title)
                .setPositiveButton(R.string.dialog_btn_connect, (dialog1, which) -> {
                    String pin = editText.getText().toString();
                    pinConsumer.accept(pin);
                })
                .setNegativeButton(R.string.dialog_btn_cancel, null)
                .setCancelable(false)
                .setView(view)
                .create();

        dialog.show();
    }

    private TeeVidEventListener getEventListener() {
        return new TeeVidEventListener() {

            @Override
            public void onConnect() {
                Log.d(TAG, "onConnect");
            }

            @Override
            public void onDisconnect() {
                Log.d(TAG, "onDisconnect");
            }

            @Override
            public void onAddParticipant(String participantId) {
                Log.d(TAG, "onAddParticipant: " + participantId);
            }

            @Override
            public void onRemoveParticipant(String participantId) {
                Log.d(TAG, "onRemoveParticipant: " + participantId);
            }

            @Override
            public void onReceiveError(Throwable throwable) {
                Log.e(TAG, "onReceiveError: ", throwable);
                Toast.makeText(getContext(), throwable.getMessage(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onRequestAccessPin(String reason, Consumer<String> pinConsumer) {
                Log.d(TAG, "onRequestAccessPin: " + reason);
                if (SdkErrors.BAD_PIN.equals(reason)) {
                    Toast.makeText(getContext(), R.string.invalid_pin, Toast.LENGTH_LONG).show();
                }
                showEnterPinDialog(pinConsumer);
            }

            @Override
            public void onChangeVoiceActivationState(boolean activated) {
                Log.d(TAG, "onChangeVoiceActivationState: " + activated);
            }
        };
    }
}
