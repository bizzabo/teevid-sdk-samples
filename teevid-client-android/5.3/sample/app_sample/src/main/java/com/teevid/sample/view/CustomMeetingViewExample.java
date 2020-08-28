package com.teevid.sample.view;

import android.content.Context;
import android.content.res.Configuration;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.teevid.sample.R;
import com.teevid.sample.view.container.VideoViewContainer;
import com.teevid.sdk.constant.CameraProvider;
import com.teevid.sdk.view.BaseMeetingView;
import com.teevid.sdk.view.VideoView;

import java.util.List;

public class CustomMeetingViewExample extends BaseMeetingView {

    private static final String TAG = "CustomMeetingViewExampl";

    private VideoView viewPictureInPicture;
    private VideoView viewScreenSharing;
    private LinearLayout layoutLinear;

    private DragTouchListener dragTouchListener;

    private VideoViewContainer<VideoView> viewGrid;
    private VideoViewContainer<VideoView> viewList;
    private int maxViewsInGrid = 4;

    public CustomMeetingViewExample(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);

        LayoutInflater.from(context).inflate(R.layout.view_meeting_custom, this, true);

        viewPictureInPicture = findViewById(R.id.view_picture_in_picture);
        viewScreenSharing = findViewById(R.id.view_screen_share);
        viewGrid = findViewById(R.id.view_grid);
        viewList = findViewById(R.id.view_list);
        layoutLinear = findViewById(R.id.layout_linear);

        viewPictureInPicture.setZOrderMediaOverlay(true);
        viewScreenSharing.setZOrderMediaOverlay(true);
        dragTouchListener = new DragTouchListener(viewPictureInPicture);
        viewPictureInPicture.setOnTouchListener(dragTouchListener);
        makeLocalVideoFullscreen();

        setKeepScreenOn(true);

// Reserved for testing purposes
//        Random random = new Random();
//        int minValue = 64;
//        int maxValue = 128;
//        viewPictureInPicture.setOnClickListener(v -> {
//
//            long id = System.currentTimeMillis();
//
//            int color = Color.argb(255, minValue + random.nextInt(maxValue),
//                    minValue + random.nextInt(maxValue),
//                    minValue + random.nextInt(maxValue));
//
//            VideoView view = new VideoView(getContext());
//            view.setTag(id);
//            log.d(TAG, "onClick: create " + view);
//
//            view.setOnClickListener(view1 -> {
//                removeVideoView(id, true);
//            });
//
//            view.setBackgroundColor(color);
//
//            getVideoViews().put(id, view);
//            onAddVideoView(id, view);
//        });
//        viewPictureInPicture.setOnLongClickListener(v -> {
//            int color = Color.argb(255, minValue + random.nextInt(maxValue),
//                    minValue + random.nextInt(maxValue),
//                    minValue + random.nextInt(maxValue));
//
//            int visibility = viewScreenSharing.getVisibility();
//            if (visibility == VISIBLE) {
//                super.removeVideoView(0, false);
//            } else {
//                super.createRemoteVideoSink(0, false, videoSink -> {
//                });
//            }
//
//            viewScreenSharing.setBackgroundColor(color);
//            return true;
//        });
//        viewScreenSharing.setOnClickListener(v -> {
//            int visibility = viewScreenSharing.getVisibility();
//            if (visibility == VISIBLE) {
//                super.removeVideoView(0, false);
//            } else {
//                super.createRemoteVideoSink(0, false, videoSink -> {
//                });
//            }
//        });
    }

    @Override
    public void onOrientationChanged(int orientation) {
        dragTouchListener.onOrientationChanged();
        viewGrid.setLayoutOrientationMode(orientation);
        viewList.setLayoutOrientationMode(orientation);

        int layoutOrientation;

        LinearLayout.LayoutParams paramsGrid = (LinearLayout.LayoutParams) viewGrid.getLayoutParams();
        paramsGrid.weight = 1;

        LinearLayout.LayoutParams paramsScreenSharing = (LinearLayout.LayoutParams)
                viewScreenSharing.getLayoutParams();

        LinearLayout.LayoutParams paramsList = (LinearLayout.LayoutParams) viewList.getLayoutParams();

        if (orientation == Configuration.ORIENTATION_PORTRAIT) {
            layoutOrientation = LinearLayout.VERTICAL;

            paramsGrid.width = ViewGroup.LayoutParams.MATCH_PARENT;
            paramsGrid.height = 0;

            paramsScreenSharing.width = ViewGroup.LayoutParams.MATCH_PARENT;
            paramsScreenSharing.height = ViewGroup.LayoutParams.WRAP_CONTENT;

            paramsList.width = ViewGroup.LayoutParams.MATCH_PARENT;
            paramsList.height = ViewGroup.LayoutParams.WRAP_CONTENT;
        } else {
            layoutOrientation = LinearLayout.HORIZONTAL;

            paramsGrid.width = 0;
            paramsGrid.height = ViewGroup.LayoutParams.MATCH_PARENT;

            paramsScreenSharing.width = ViewGroup.LayoutParams.WRAP_CONTENT;
            paramsScreenSharing.height = ViewGroup.LayoutParams.MATCH_PARENT;

            paramsList.width = ViewGroup.LayoutParams.WRAP_CONTENT;
            paramsList.height = ViewGroup.LayoutParams.MATCH_PARENT;
        }

        layoutLinear.setOrientation(layoutOrientation);
        viewGrid.setLayoutParams(paramsGrid);
        viewScreenSharing.setLayoutParams(paramsScreenSharing);
        viewList.setLayoutParams(paramsList);
    }

    @Override
    public VideoView getLocalVideoView() {
        return viewPictureInPicture;
    }

    @Override
    public VideoView getScreenSharingVideoView(String participantId) {
        viewScreenSharing.setVisibility(View.VISIBLE);
        viewPictureInPicture.setVisibility(View.GONE);
        redistributeVideoViews(); // Move all views from grid to list
        return viewScreenSharing;
    }

    @Override
    public void onRemoveScreenSharingVideoView(String participantId) {
        viewScreenSharing.setVisibility(View.GONE);
        viewPictureInPicture.setVisibility(View.VISIBLE);
        redistributeVideoViews(); // Distribute view from list between grid and list
    }

    @Override
    public void onAddVideoView(String participantId, long streamId, VideoView view) {
        Log.d(TAG, "onAddVideoView: " + participantId + ", " + streamId);
        int count = viewGrid.itemCount() + viewList.itemCount() + 1;

        if (getMode() == MODE_DEFAULT) {
            if (count <= maxViewsInGrid) {
                addViewToGridViewContainer(view);

                if (count == 1) {
                    makeLocalVideoSmall();
                }
            } else {
                addViewToListViewContainer(view);
            }
        } else { // Screen sharing is on, all views should go to list
            addViewToListViewContainer(view);
        }
    }

    @Override
    public void onRemoveVideoView(String participantId, long streamId, VideoView view) {
        Log.d(TAG, "onRemoveVideoView: " + streamId);

        int count = getVideoViewsCount();

        if (viewList.contains(view)) {
            Log.d(TAG, "onRemoveVideoView: " + "remove from list");
            viewList.removeView(view);
        } else {
            Log.d(TAG, "onRemoveVideoView: " + "remove from grid");
            viewGrid.removeView(view);

            // If there's any view in list, move the first one to grid
            if (!viewList.isEmpty()) {
                VideoView listChildView = viewList.get(0);
                viewList.removeView(listChildView);

                addViewToGridViewContainer(listChildView);
            }
        }
        if (count == 0 && getMode() != MODE_SCREEN_SHARE) {
            makeLocalVideoFullscreen();
        }
    }

    @Override
    public void onShowLocalVideo() {
        viewPictureInPicture.setVisibility(View.VISIBLE);
    }

    @Override
    public void onHideLocalVideo() {
        viewPictureInPicture.setVisibility(View.GONE);
    }

    @Override
    public void onSetMaxViewsInGrid(int maxViewsInGrid) {
        this.maxViewsInGrid = maxViewsInGrid;
    }

    @Override
    public void onLocalVideoCameraChanged(@CameraProvider int cameraProvider) {
        Log.d(TAG, "onLocalVideoCameraChanged: " + cameraProvider);
        if (CameraProvider.FRONT_FACING == cameraProvider) {
            viewPictureInPicture.setMirror(true);
        } else {
            viewPictureInPicture.setMirror(false);
        }
    }

    private void addViewToGridViewContainer(VideoView view) {
        Log.d(TAG, "addViewToGridView");
        viewGrid.addView(view);
    }

    private void addViewToListViewContainer(VideoView view) {
        Log.d(TAG, "addViewToListView");
        viewList.addView(view);
    }

    // Removes all views from grid and list and then re-adds them. This is required when screen
    // share is on (all views from grid should be moved to list) and when screen sharing is off (views
    // from list should be distributed between grid and list).
    private void redistributeVideoViews() {
        viewList.clear();
        viewGrid.clear();

        post(() -> {
            List<VideoView> videoViews = getRemoteVideoViews();
            if (videoViews.isEmpty() && getMode() != MODE_SCREEN_SHARE) {
                // A situation when screen sharing view is removed along with the last video view
                makeLocalVideoFullscreen();
            } else {
                for (VideoView videoView : getRemoteVideoViews()) {
                    onAddVideoView(null, 0, videoView);
                }
            }
        });
    }

    private void makeLocalVideoSmall() {
        int width = (int) getResources().getDimension(R.dimen.local_video_small_width);
        int height = (int) getResources().getDimension(R.dimen.local_video_small_height);
        setLocalVideoSize(width, height);

        dragTouchListener.enable();
        dragTouchListener.moveViewToDesiredPosition();
    }

    private void makeLocalVideoFullscreen() {
        int width = ViewGroup.LayoutParams.MATCH_PARENT;
        int height = ViewGroup.LayoutParams.MATCH_PARENT;
        setLocalVideoSize(width, height);

        dragTouchListener.disable();
    }

    private void setLocalVideoSize(int width, int height) {
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(width, height);
        viewPictureInPicture.setLayoutParams(params);
    }
}
