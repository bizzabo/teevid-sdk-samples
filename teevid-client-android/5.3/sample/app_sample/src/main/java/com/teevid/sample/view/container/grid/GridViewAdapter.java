package com.teevid.sample.view.container.grid;

import android.content.res.Configuration;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;

import com.teevid.sdk.view.VideoView;

import java.util.ArrayList;
import java.util.List;

class GridViewAdapter extends BaseAdapter {

    private List<VideoView> views = new ArrayList<>();
    private int viewHeight;
    private GridView gridView;
    private int orientation = Configuration.ORIENTATION_PORTRAIT;

    private Runnable commandUpdate = () -> {
        updateGridViewParameters(orientation == Configuration.ORIENTATION_PORTRAIT);
        notifyDataSetChanged();
    };

    public GridViewAdapter(GridView gridView) {
        this.gridView = gridView;
    }

    public void addView(VideoView view) {
        view.setLayoutParams(new GridView.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                viewHeight));

        views.add(view);

        updateViews();
    }

    public void removeView(VideoView view) {
        view.setVisibility(View.GONE);
        views.remove(view);
        updateViews();
    }

    public void clear() {
        views.clear();
        updateViews();
    }

    public boolean contains(VideoView view) {
        return views.contains(view);
    }

    public VideoView get(int index) {
        return views.get(index);
    }

    public void setLayoutMode(int orientation) {
        this.orientation = orientation;
    }

    public void updateViews() {
        gridView.getHandler().removeCallbacks(commandUpdate);
        gridView.post(commandUpdate);
    }

    private void updateGridViewParameters(boolean portrait) {
        int viewCount = getCount();
        int numColumns;
        int viewHeight;

        if (portrait) {
            if (viewCount == 0) {
                numColumns = 1;
                viewHeight = gridView.getHeight();
            } else if (viewCount <= 3) {
                numColumns = 1;
                viewHeight = gridView.getHeight() / viewCount;
            } else {
                numColumns = 2;
                viewHeight = gridView.getHeight() / 2;
            }
        } else {
            if (viewCount == 0) {
                numColumns = 1;
                viewHeight = gridView.getHeight();
            } else if (viewCount <= 3) {
                numColumns = viewCount;
                viewHeight = gridView.getHeight();
            } else {
                numColumns = 2;
                viewHeight = gridView.getHeight() / 2;
            }
        }

        if (numColumns != gridView.getNumColumns()) {
            gridView.setNumColumns(numColumns);
        }

        if (viewCount == 0 && gridView.getVisibility() == View.VISIBLE) {
            gridView.setVisibility(View.GONE);
        } else if (viewCount != 0 && gridView.getVisibility() == View.GONE) {
            gridView.setVisibility(View.VISIBLE);
        }

        this.viewHeight = viewHeight;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View view = views.get(position);

        ViewGroup.LayoutParams layoutParams = view.getLayoutParams();
        layoutParams.height = viewHeight;
        view.setLayoutParams(layoutParams);

        return view;
    }

    @Override
    public int getCount() {
        return views.size();
    }

    @Override
    public Object getItem(int position) {
        return views.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }
}
