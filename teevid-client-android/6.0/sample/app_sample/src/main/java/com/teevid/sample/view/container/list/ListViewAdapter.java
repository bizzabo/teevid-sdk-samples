package com.teevid.sample.view.container.list;

import android.content.res.Configuration;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.teevid.sample.R;
import com.teevid.sdk.view.VideoView;

import java.util.ArrayList;
import java.util.List;

public class ListViewAdapter extends RecyclerView.Adapter<ListViewAdapter.ViewViewHolder> {

    private List<VideoView> views = new ArrayList<>();
    private RecyclerView recyclerView;

    public ListViewAdapter(RecyclerView recyclerView) {
        this.recyclerView = recyclerView;
    }

    public void addView(VideoView view) {
        ViewGroup.LayoutParams layoutParams = view.getLayoutParams(); // Flushing old params (e.g. moving view from another layout)
        if (layoutParams == null) {
            layoutParams = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.MATCH_PARENT);
        } else {
            layoutParams.width = ViewGroup.LayoutParams.MATCH_PARENT;
            layoutParams.height = ViewGroup.LayoutParams.MATCH_PARENT;
        }
        view.setLayoutParams(layoutParams);

        views.add(view);
        notifyItemInserted(views.size());
    }

    public void removeView(VideoView view) {
        int position = views.indexOf(view);
        views.remove(view);

        ViewGroup parent = (ViewGroup) view.getParent();
        if (parent != null) {
            parent.removeView(view);
        }

        notifyItemRemoved(position);
    }

    public void clear() {
        for (VideoView view : views) {
            ViewGroup parent = (ViewGroup) view.getParent();
            if (parent != null) {
                parent.removeView(view);
            }
        }
        int size = views.size();
        views.clear();
        notifyItemRangeRemoved(0, size);
    }

    public boolean contains(VideoView view) {
        return views.contains(view);
    }

    public VideoView get(int index) {
        return views.get(index);
    }

    public void setLayoutMode(int orientation) {
        int recyclerOrientation;
        if (orientation == Configuration.ORIENTATION_PORTRAIT) {
            recyclerOrientation = RecyclerView.HORIZONTAL;
        } else {
            recyclerOrientation = RecyclerView.VERTICAL;
        }

        LinearLayoutManager layoutManager = ((LinearLayoutManager) recyclerView.getLayoutManager());
        layoutManager.setOrientation(recyclerOrientation);
    }

    public List<VideoView> getVideoViews() {
        return views;
    }

    @NonNull
    @Override
    public ViewViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.item_list_video_holder, parent, false);

        return new ViewViewHolder(view);
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
    }

    @Override
    public int getItemCount() {
        return views.size();
    }

    static class ViewViewHolder extends RecyclerView.ViewHolder {

        public ViewViewHolder(@NonNull View itemView) {
            super(itemView);
        }
    }
}