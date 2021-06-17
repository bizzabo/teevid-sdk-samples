package com.teevid.sample.view.container;

import android.view.ViewGroup;

import java.util.List;

public interface VideoViewContainer<T> {

    void addView(T view);

    void removeView(T view);

    boolean contains(T view);

    boolean isEmpty();

    T get(int index);

    List<T> getVideoViews();

    void setLayoutOrientationMode(int orientation);

    ViewGroup.LayoutParams getLayoutParams();

    void setLayoutParams(ViewGroup.LayoutParams params);

    void clear();

    int itemCount();

    void setVisibility(int visibility);
}
