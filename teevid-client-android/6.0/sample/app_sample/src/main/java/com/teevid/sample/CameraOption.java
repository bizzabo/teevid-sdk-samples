package com.teevid.sample;

import androidx.core.util.Pair;

import com.teevid.sdk.constant.CameraProvider;

public class CameraOption extends Pair<Integer, String> {

    public CameraOption(@CameraProvider int first, String second) {
        super(first, second);
    }

    @Override
    public String toString() {
        return second;
    }
}
