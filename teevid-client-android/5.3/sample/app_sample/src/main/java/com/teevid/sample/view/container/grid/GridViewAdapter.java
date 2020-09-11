package com.teevid.sample.view.container.grid;

import android.content.res.Configuration;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.teevid.sample.R;
import com.teevid.sdk.view.VideoView;

import java.util.ArrayList;
import java.util.List;

public class GridViewAdapter extends RecyclerView.Adapter<GridViewAdapter.ViewViewHolder> {

    private RecyclerView recyclerView;
    private GridLayoutManager gridLayoutManager;
    private List<VideoView> views = new ArrayList<>();
    private int viewHeight;
    private int viewWidth;
    private int orientation = Configuration.ORIENTATION_PORTRAIT;

    public GridViewAdapter(RecyclerView recyclerView) {
        this.recyclerView = recyclerView;

        RecyclerView.LayoutManager layoutManager = recyclerView.getLayoutManager();
        if (layoutManager instanceof GridLayoutManager) {
            this.gridLayoutManager = (GridLayoutManager) layoutManager;
        } else {
            throw new RuntimeException("GridLayoutManager is not set to RecyclerView");
        }
    }

    @NonNull
    @Override
    public ViewViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.item_grid_video_holder, parent, false);

        return new GridViewAdapter.ViewViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewViewHolder holder, int position) {
        VideoView videoView = views.get(position);
        ViewGroup viewGroup = ((ViewGroup) holder.itemView);

        ViewGroup parent = (ViewGroup) videoView.getParent();
        if (parent != null) {
            parent.removeView(videoView);
        }

        viewGroup.addView(videoView);

        ViewGroup.LayoutParams layoutParams = viewGroup.getLayoutParams();
        layoutParams.width = viewWidth;
        layoutParams.height = viewHeight;
        viewGroup.setLayoutParams(layoutParams);
    }

    @Override
    public int getItemCount() {
        return views.size();
    }

    public void addView(VideoView view) {
        views.add(view);

        calculateViewSizeAndColumns();
        applySizeToViews();

        notifyItemInserted(views.size());
    }

    public void removeView(VideoView view) {
        int position = views.indexOf(view);
        views.remove(view);

        calculateViewSizeAndColumns();
        applySizeToViews();

        notifyItemRemoved(position);
    }

    public boolean contains(VideoView view) {
        return views.contains(view);
    }

    public boolean isEmpty() {
        return getItemCount() == 0;
    }

    public VideoView get(int index) {
        return views.get(index);
    }

    public void setLayoutOrientationMode(int orientation) {
        this.orientation = orientation;
        calculateViewSizeAndColumns();
        applySizeToViews();
    }

    public void clear() {
        int count = views.size();
        views.clear();

        notifyItemRangeRemoved(0, count);
    }

    public void onSizeChanged() {
        calculateViewSizeAndColumns();
        applySizeToViews();
    }

    public List<VideoView> getItems() {
        return views;
    }

    private void calculateViewSizeAndColumns() {
        int viewCount = views.size();
        int numColumns;
        int viewHeight;
        int viewWidth;
        boolean portrait = orientation == Configuration.ORIENTATION_PORTRAIT;

        if (portrait) {
            if (viewCount == 0) { // 0
                numColumns = 1;
                viewWidth = recyclerView.getWidth();
                viewHeight = recyclerView.getHeight();
            } else if (viewCount <= 3) { // 1-3
                numColumns = 1;
                viewWidth = recyclerView.getWidth();
                viewHeight = recyclerView.getHeight() / viewCount;
            } else { // 4+
                numColumns = 2;
                viewWidth = recyclerView.getWidth() / 2;
                viewHeight = recyclerView.getHeight() / 2;
            }
        } else {
            if (viewCount == 0) { // 0
                numColumns = 1;
                viewWidth = recyclerView.getWidth();
                viewHeight = recyclerView.getHeight();
            } else if (viewCount <= 3) { // 1-3
                numColumns = viewCount;
                viewWidth = recyclerView.getWidth() / viewCount;
                viewHeight = recyclerView.getHeight();
            } else { // 4+
                numColumns = 2;
                viewWidth = recyclerView.getWidth() / 2;
                viewHeight = recyclerView.getHeight() / 2;
            }
        }

        if (numColumns != gridLayoutManager.getSpanCount()) {
            gridLayoutManager.setSpanCount(numColumns);
        }

        if (viewCount == 0 && recyclerView.getVisibility() == View.VISIBLE) {
            recyclerView.setVisibility(View.GONE);
        } else if (viewCount != 0 && recyclerView.getVisibility() == View.GONE) {
            recyclerView.setVisibility(View.VISIBLE);
        }

        this.viewHeight = viewHeight;
        this.viewWidth = viewWidth;
    }

    private void applySizeToViews() {
        for (VideoView view : views) {
            ViewGroup parent = (ViewGroup) view.getParent();
            if (parent != null) {
                ViewGroup.LayoutParams layoutParams = parent.getLayoutParams();
                layoutParams.width = viewWidth;
                layoutParams.height = viewHeight;
                parent.setLayoutParams(layoutParams);
            }
        }
    }

    static class ViewViewHolder extends RecyclerView.ViewHolder {

        public ViewViewHolder(@NonNull View itemView) {
            super(itemView);
        }
    }
}
