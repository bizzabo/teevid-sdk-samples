package com.teevid.sample.view.container.grid;

import android.content.Context;
import android.util.AttributeSet;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.teevid.sample.view.container.VideoViewContainer;
import com.teevid.sdk.view.VideoView;

import java.util.List;

public class GridVideoViewContainer extends RecyclerView implements VideoViewContainer<VideoView> {

    private GridViewAdapter adapter;

    public GridVideoViewContainer(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public GridVideoViewContainer(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        GridLayoutManager layoutManager = new GridLayoutManager(context, 1) {
            @Override
            public boolean canScrollVertically() {
                return false;
            }
        };
        setLayoutManager(layoutManager);
        setItemAnimator(null); // Disable animation

        adapter = new GridViewAdapter(this);
        setAdapter(adapter);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        adapter.onSizeChanged();
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
        return adapter.isEmpty();
    }

    @Override
    public VideoView get(int index) {
        return adapter.get(index);
    }

    @Override
    public List<VideoView> getVideoViews() {
        return adapter.getItems();
    }

    @Override
    public void setLayoutOrientationMode(int orientation) {
        adapter.setLayoutOrientationMode(orientation);
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
