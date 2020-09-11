package com.teevid.sample.view.container.list;

import android.content.Context;
import android.util.AttributeSet;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.teevid.sample.view.container.VideoViewContainer;
import com.teevid.sdk.view.VideoView;

import java.util.List;

public class ListVideoViewContainer extends RecyclerView implements VideoViewContainer<VideoView> { // Also known as "Carousel"

    private ListViewAdapter adapter;

    public ListVideoViewContainer(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public ListVideoViewContainer(@NonNull Context context, @Nullable AttributeSet attrs,
                                  int defStyleAttr) {

        super(context, attrs, defStyleAttr);

        adapter = new ListViewAdapter(this);
        setLayoutManager(new LinearLayoutManager(getContext(), LinearLayoutManager.HORIZONTAL,
                false));
        setAdapter(adapter);
    }

    @Override
    public void addView(VideoView view) {
        adapter.addView(view);
    }

    @Override
    public void removeView(VideoView view) {
        adapter.removeView(view);
    }

    @Override
    public boolean contains(VideoView view) {
        return adapter.contains(view);
    }

    @Override
    public boolean isEmpty() {
        return adapter.getItemCount() == 0;
    }

    @Override
    public VideoView get(int index) {
        return adapter.get(index);
    }

    @Override
    public List<VideoView> getVideoViews() {
        return adapter.getVideoViews();
    }

    @Override
    public void setLayoutOrientationMode(int orientation) {
        adapter.setLayoutMode(orientation);
    }

    @Override
    public void clear() {
        adapter.clear();
    }

    @Override
    public int itemCount() {
        return adapter.getItemCount();
    }
}
