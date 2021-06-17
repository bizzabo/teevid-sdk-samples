package com.teevid.sample;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.StringRes;
import androidx.appcompat.app.AlertDialog;
import androidx.core.util.Consumer;
import androidx.fragment.app.Fragment;

import com.teevid.sdk.TeeVidClient;
import com.teevid.sdk.TeeVidEventListener;
import com.teevid.sdk.api.SdkErrors;
import com.teevid.sdk.data.CameraDeviceInfo;
import com.teevid.sdk.data.HardMuteRequest;
import com.teevid.sdk.data.ParticipantInfo;
import com.teevid.sdk.data.PinRequest;
import com.teevid.sdk.data.RoomInfo;
import com.teevid.sdk.log.LogLevel;
import com.teevid.sdk.view.BaseMeetingView;

import java.util.List;

public class CallFragment extends Fragment {

    private static final String TAG = "CallFragment";

    private TeeVidClient client;

    private ToggleButton btnMicrophone;
    private ToggleButton btnCamera;

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
        btnCamera = view.findViewById(R.id.btn_camera);
        ToggleButton btnLocalVideo = view.findViewById(R.id.btn_local_video);
        ImageButton btnInvite = view.findViewById(R.id.btn_invite);
        btnMicrophone = view.findViewById(R.id.btn_microphone);
        ToggleButton btnScreenSharing = view.findViewById(R.id.btn_screen_sharing);
        ToggleButton btnCameraSwitch = view.findViewById(R.id.btn_camera_switch);
        ImageButton btnShareLogs = view.findViewById(R.id.btn_share_logs);

        UserPreferences preferences = SampleApplication.getInstance().getUserPreferences();
        String server = preferences.getServer();
        String roomId = preferences.getRoomId();
        String username = preferences.getUsername();
        String invitationLink = preferences.getInvitationLink();
        String password = preferences.getPassword();
        String sdkToken = preferences.getSdkToken();
        int defaultCamera = preferences.getCamera();

        client = new TeeVidClient.Builder(getContext(), sdkToken)
                .addListener(getEventListener())
                .setDebugMode(true)
                .setLogLevel(LogLevel.DEBUG)
                .build();
        client.setView(viewTeeVid);
        client.setDefaultCamera(defaultCamera);

        if (!TextUtils.isEmpty(invitationLink)) {
            client.connectWithInvitation(username, invitationLink);
        } else if (!TextUtils.isEmpty(password)) {
            client.connectAsUser(roomId, server, username, password);
        } else {
            client.connect(roomId, server, username);
        }

        btnCamera.setOnClickListener(v -> onCameraButtonClicked(btnCamera));
        btnLocalVideo.setOnClickListener(v -> onLocalVideoButtonClicked(btnLocalVideo));
        btnInvite.setOnClickListener(v -> onInviteButtonClicked());
        btnScreenSharing.setOnClickListener(v -> onScreenSharingButtonClicked(btnScreenSharing));
        btnMicrophone.setOnClickListener(v -> onMicrophoneButtonClicked(btnMicrophone));
        btnCameraSwitch.setOnClickListener(v -> onSwitchCameraButtonClicked());
        btnShareLogs.setOnClickListener(v -> onShareLogsButtonClicked());
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        client.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        client.dispose();
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

    private void onInviteButtonClicked() {
        client.createInvitationLink(this::shareText);
    }

    private void onScreenSharingButtonClicked(ToggleButton button) {
        boolean enabled = button.isChecked();
        if (enabled) {
            client.stopScreenSharing();
        } else {
            client.startScreenSharing(this);
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

    private void shareText(String text) {
        Intent intent = new Intent(android.content.Intent.ACTION_SEND);
        intent.setType("text/plain");
        intent.putExtra(android.content.Intent.EXTRA_TEXT, text);
        startActivity(intent);
    }

    private void onShareLogsButtonClicked() {
        Uri uri = client.getLogsFileUri();
        shareFileUsingUri(uri);
    }

    private void showEnterPinDialog(PinRequest pinRequest) {
        Context context = getContext();
        if (context == null) {
            return;
        }

        View view = LayoutInflater.from(getContext()).inflate(R.layout.dialog_edit_text, null);
        EditText editText = view.findViewById(R.id.et_input);
        editText.setHint(R.string.enter_pin);
        editText.setInputType(EditorInfo.TYPE_TEXT_VARIATION_PASSWORD);

        AlertDialog dialog = new AlertDialog.Builder(context)
                .setTitle(R.string.enter_pin_title)
                .setPositiveButton(R.string.dialog_btn_connect, (dialog1, which) -> {
                    String pin = editText.getText().toString();
                    pinRequest.accept(pin);
                })
                .setNegativeButton(R.string.dialog_btn_cancel, null)
                .setCancelable(false)
                .setView(view)
                .create();

        dialog.show();
    }

    private void showUnmuteAudioRequestDialog(Consumer<Boolean> resultConsumer) {
        AlertDialog alertDialog = new AlertDialog.Builder(getContext())
                .setTitle(R.string.dialog_unmute_audio_title)
                .setPositiveButton(R.string.dialog_unmute_positive, (dialog, which) -> {
                    resultConsumer.accept(true);
                    btnMicrophone.setVisibility(View.VISIBLE);
                    if (btnMicrophone.isChecked()) {
                        btnMicrophone.toggle();
                    }
                    showToast(R.string.dialog_unmute_microphone_unmuted);
                })
                .setNegativeButton(R.string.dialog_unmute_negative, (dialog, which) ->
                        resultConsumer.accept(false))
                .create();

        alertDialog.show();
    }

    private void showUnmuteVideoRequestDialog(Consumer<Boolean> resultConsumer) {
        AlertDialog alertDialog = new AlertDialog.Builder(getContext())
                .setTitle(R.string.dialog_unmute_video_title)
                .setPositiveButton(R.string.dialog_unmute_positive, (dialog, which) -> {
                    resultConsumer.accept(true);
                    btnCamera.setVisibility(View.VISIBLE);
                    if (!btnCamera.isChecked()) {
                        btnCamera.toggle();
                    }
                    showToast(R.string.dialog_unmute_camera_unmuted);
                })
                .setNegativeButton(R.string.dialog_unmute_negative, (dialog, which) ->
                        resultConsumer.accept(false))
                .create();

        alertDialog.show();
    }

    private void showToast(@StringRes int resId) {
        Toast.makeText(getContext(), resId, Toast.LENGTH_SHORT).show();
    }

    private void shareFileUsingUri(Uri uri) {
        Intent intent = new Intent(Intent.ACTION_SEND);
        intent.setType("text/plain");
        intent.putExtra(Intent.EXTRA_STREAM, uri);
        startActivity(intent);
    }

    private void selectNeededCameraDevice(TeeVidClient client) {
        List<CameraDeviceInfo> cameraDevices = client.getAvailableCameraDevices();
        CameraDeviceInfo selectedDevice = cameraDevices.get(1);
        client.setCameraById(selectedDevice.getId());
    }

    private void showDisconnectDialog() {
        Context context = getContext();
        if (context == null) {
            return;
        }

        AlertDialog dialog = new AlertDialog.Builder(context)
                .setMessage(R.string.disconnected_from_room)
                .setPositiveButton(android.R.string.ok,
                        (dialog1, which) -> getActivity().onBackPressed())
                .create();
        dialog.show();
    }

    private TeeVidEventListener getEventListener() {
        return new TeeVidEventListener() {

            @Override
            public void onConnect(RoomInfo roomInfo) {
                Log.d(TAG, "onConnect");
            }

            @Override
            public void onDisconnect() {
                Log.d(TAG, "onDisconnect");
                showDisconnectDialog();
            }

            @Override
            public void onAddParticipant(ParticipantInfo participantInfo) {
                Log.d(TAG, "onAddParticipant: " + participantInfo);
            }

            @Override
            public void onRemoveParticipant(ParticipantInfo participantInfo) {
                Log.d(TAG, "onRemoveParticipant: " + participantInfo);
            }

            @Override
            public void onReceiveError(Throwable throwable) {
                Log.e(TAG, "onReceiveError: ", throwable);
                Toast.makeText(getContext(), throwable.getMessage(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onRequestAccessPin(String reason, PinRequest pinRequest) {
                Log.d(TAG, "onRequestAccessPin: " + reason);
                if (SdkErrors.BAD_PIN.equals(reason)) {
                    Toast.makeText(getContext(), R.string.invalid_pin, Toast.LENGTH_LONG).show();
                }
                showEnterPinDialog(pinRequest);
            }

            @Override
            public void onMuteAudioByModerator() {
                Log.d(TAG, "onMutedByModerator");
                btnMicrophone.setVisibility(View.GONE);
            }

            @Override
            public void onMuteVideoByModerator() {
                Log.d(TAG, "onMuteVideoByModerator");
                btnCamera.setVisibility(View.GONE);
            }

            @Override
            public void onReceiveUnmuteAudioRequest(Consumer<Boolean> resultConsumer) {
                Log.d(TAG, "onReceiveUnmuteAudioRequest");
                showUnmuteAudioRequestDialog(resultConsumer);
            }

            @Override
            public void onReceiveUnmuteVideoRequest(Consumer<Boolean> resultConsumer) {
                Log.d(TAG, "onReceiveUnmuteVideoRequest");
                showUnmuteVideoRequestDialog(resultConsumer);
            }

            @Override
            public void onReceiveHardMuteRequest(HardMuteRequest hardMuteRequest) {
                Log.d(TAG, "onReceiveHardMuteRequest");
                if (hardMuteRequest.isRequired()) {
                    client.stopVideo();
                    client.mute();

                    btnCamera.setChecked(false);
                    btnMicrophone.setChecked(false);
                }
                hardMuteRequest.proceed();
            }

            @Override
            public void onDisconnectByModerator() {
                Log.d(TAG, "onDisconnectByModerator");
                showToast(R.string.disconnected_from_room);
            }

            @Override
            public void onChangeVoiceActivationState(boolean activated) {
                Log.d(TAG, "onChangeVoiceActivationState: " + activated);
            }
        };
    }
}
