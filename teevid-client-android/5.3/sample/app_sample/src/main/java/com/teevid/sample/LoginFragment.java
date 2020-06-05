package com.teevid.sample;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

public class LoginFragment extends Fragment {

    private static final int RC_PERMISSIONS = 1;

    private Navigation navigation;

    private Button btnConnect;
    private EditText etServer;
    private EditText etRoom;
    private EditText etName;

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

        btnConnect.setOnClickListener(v -> onConnectClicked());

        UserPreferences preferences = SampleApplication.getInstance().getUserPreferences();
        etServer.setText(preferences.getServer());
        etRoom.setText(preferences.getRoomId());
        etName.setText(preferences.getUsername());
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

        UserPreferences preferences = SampleApplication.getInstance().getUserPreferences();
        preferences.setServer(server);
        preferences.setRoomId(roomId);
        preferences.setUsername(username);

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
}
