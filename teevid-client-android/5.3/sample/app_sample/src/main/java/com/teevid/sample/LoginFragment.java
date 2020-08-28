package com.teevid.sample;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.Spinner;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import com.teevid.sdk.constant.CameraProvider;

public class LoginFragment extends Fragment {

    private static final int RC_PERMISSIONS = 1;

    private Navigation navigation;

    private Button btnConnect;
    private EditText etServer;
    private EditText etRoom;
    private EditText etName;
    private EditText etInvitationLink;
    private EditText etPassword;
    private EditText etSdkToken;
    private Spinner spinnerCamera;

    public LoginFragment() {

    }

    @Override
    public void onAttach(@NonNull Context context) {
        super.onAttach(context);
        if (context instanceof Navigation) {
            navigation = (Navigation) context;
        } else {
            throw new IllegalStateException("Host activity must implement Navigation interface.");
        }
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_login, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        btnConnect = view.findViewById(R.id.btn_connect);
        etServer = view.findViewById(R.id.et_server);
        etRoom = view.findViewById(R.id.et_room);
        etName = view.findViewById(R.id.et_name);
        spinnerCamera = view.findViewById(R.id.spinner_camera);
        etInvitationLink = view.findViewById(R.id.et_invitation_link);
        etPassword = view.findViewById(R.id.et_password);
        etSdkToken = view.findViewById(R.id.et_sdk_token);
        RadioGroup rgConnectAs = view.findViewById(R.id.rg_connect_as);

        btnConnect.setOnClickListener(v -> onConnectClicked());
        rgConnectAs.setOnCheckedChangeListener((group, checkedId) -> onConnectAsToggled(checkedId));

        UserPreferences preferences = SampleApplication.getInstance().getUserPreferences();
        etServer.setText(preferences.getServer());
        etRoom.setText(preferences.getRoomId());
        etName.setText(preferences.getUsername());
        etInvitationLink.setText(preferences.getInvitationLink());
        etSdkToken.setText(preferences.getSdkToken());

        CameraOption[] cameraOptions = new CameraOption[]{
                new CameraOption(CameraProvider.FRONT_FACING, getString(R.string.camera_front)),
                new CameraOption(CameraProvider.BACK_FACING, getString(R.string.camera_back))
        };

        ArrayAdapter<CameraOption> adapter = new ArrayAdapter<>(getContext(),
                R.layout.item_spinner, cameraOptions);
        spinnerCamera.setAdapter(adapter);
        for (int i = 0; i < cameraOptions.length; i++) { // Finding selected default camera
            CameraOption cameraOption = cameraOptions[i];
            if (cameraOption.first == preferences.getCamera()) {
                spinnerCamera.setSelection(i);
                break;
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {

        switch (requestCode) {
            case RC_PERMISSIONS:
                onConnectClicked();
                break;
            default:
                super.onRequestPermissionsResult(requestCode, permissions, grantResults);
                break;
        }
    }

    private void onConnectClicked() {
        String[] permissions = new String[]{Manifest.permission.CAMERA,
                Manifest.permission.RECORD_AUDIO};
        if (arePermissionGranted(Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO)) {
            connectToRoom();
        } else {
            requestPermissions(permissions, RC_PERMISSIONS);
        }
    }

    private void connectToRoom() {
        String server = etServer.getText().toString();
        String roomId = etRoom.getText().toString();
        String username = etName.getText().toString();
        String invitationLink = etInvitationLink.getText().toString();
        String password = etPassword.getText().toString();
        String sdkToken = etSdkToken.getText().toString();
        int camera = ((CameraOption) spinnerCamera.getSelectedItem()).first;

        UserPreferences preferences = SampleApplication.getInstance().getUserPreferences();
        preferences.setServer(server);
        preferences.setRoomId(roomId);
        preferences.setUsername(username);
        preferences.setCamera(camera);
        preferences.setInvitationLink(invitationLink);
        preferences.setPassword(password);
        preferences.setSdkToken(sdkToken);

        Fragment fragment = new CallFragment();
        navigation.showFragment(fragment, true);
    }

    private boolean arePermissionGranted(String... permissions) {
        for (String permission : permissions) {
            if (ContextCompat.checkSelfPermission(getContext(), permission)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    private void onConnectAsToggled(int checkedId) {
        if (checkedId == R.id.rb_connect_as_guest) {
            etPassword.setVisibility(View.GONE);
            etInvitationLink.setVisibility(View.VISIBLE);
        } else {
            etPassword.setVisibility(View.VISIBLE);
            etInvitationLink.setVisibility(View.GONE);
        }
    }
}
