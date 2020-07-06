package com.teevid.sample.view.container.grid;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.GridView;

import com.teevid.sample.view.container.VideoViewContainer;
import com.teevid.sdk.view.VideoView;

public class GridVideoViewContainer extends GridView implements VideoViewContainer<VideoView> {

    private GridViewAdapter adapter;

    public GridVideoViewContainer(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public GridVideoViewContainer(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        adapter = new GridViewAdapter(this);
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
        return adapter.isEmpty();
    }

    @Override
    public VideoView get(int index) {
        return adapter.get(index);
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
        return adapter.getCount();
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        adapter.updateViews();
    }
}
