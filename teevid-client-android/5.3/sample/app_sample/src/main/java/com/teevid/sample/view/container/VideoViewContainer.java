package com.teevid.sample.view.container;

import android.view.ViewGroup;

public interface VideoViewContainer<T> {

    void addView(T view);

    void removeView(T view);

    boolean contains(T view);

    boolean isEmpty();

    T get(int index);

    void setLayoutOrientationMode(int orientation);

    ViewGroup.LayoutParams getLayoutParams();

    void setLayoutParams(ViewGroup.LayoutParams params);

    void clear();

    int itemCount();
}
