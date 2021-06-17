package com.teevid.sample.view;

import android.animation.AnimatorSet;
import android.animation.ValueAnimator;
import android.graphics.Point;
import android.os.Build;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

public class DragTouchListener implements View.OnTouchListener {

    private static final int DURATION_ANIMATION_CORNER = 500;
    private static final int CLICK_THRESHOLD = 100;
    private static final int LAYOUT_SETTLE_TIME = Build.VERSION.SDK_INT < Build.VERSION_CODES.M
            ? 500 : 1;

    private final View viewDrag;
    private boolean enabled = true;
    private int prevDragX;
    private int prevDragY;

    private int maxMarginLeft;
    private int maxMarginTop;

    private int oldMaxMarginLeft;
    private int oldMaxMarginTop;

    public DragTouchListener(View viewDrag) {
        this.viewDrag = viewDrag;
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (!enabled) {
            return false;
        }

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN: {
                onActionDown(event);
                return true;
            }
            case MotionEvent.ACTION_MOVE: {
                onActionMove(event);
                return true;
            }
            case MotionEvent.ACTION_UP: {
                long duration = event.getEventTime() - event.getDownTime();
                if (duration < CLICK_THRESHOLD) {
                    v.performClick();
                }
                onActionUp();
                return true;
            }
        }
        return false;
    }

    public void onOrientationChanged() {
        viewDrag.postDelayed(() -> { // Need time for layout to settle
            calculateMaxMargins();
            applyMaxMargins();
        }, LAYOUT_SETTLE_TIME);
    }

    public void enable() {
        enabled = true;

        viewDrag.postDelayed(() -> { // Need time for layout to settle
            calculateMaxMargins();
            applyMaxMargins();
        }, LAYOUT_SETTLE_TIME);
    }

    public void disable() {
        enabled = false;
    }

    public void moveViewToDesiredPosition() {
        ViewGroup.MarginLayoutParams layoutParams = (ViewGroup.MarginLayoutParams)
                viewDrag.getLayoutParams();

        layoutParams.leftMargin = maxMarginLeft;
        layoutParams.topMargin = maxMarginTop;

        viewDrag.setLayoutParams(layoutParams);
    }

    private void calculateMaxMargins() {
        View parent = (View) viewDrag.getParent();

        oldMaxMarginLeft = maxMarginLeft;
        oldMaxMarginTop = maxMarginTop;

        ViewGroup.LayoutParams layoutParams = viewDrag.getLayoutParams();
        int viewWidth = layoutParams.width;
        int viewHeight = layoutParams.height;
        if (viewWidth == ViewGroup.LayoutParams.MATCH_PARENT) {
            viewWidth = viewDrag.getWidth();
        }
        if (viewHeight == ViewGroup.LayoutParams.MATCH_PARENT) {
            viewHeight = viewDrag.getHeight();
        }

        maxMarginLeft = parent.getWidth() - viewWidth;
        maxMarginTop = parent.getHeight() - viewHeight;
    }

    private void applyMaxMargins() {
        ViewGroup.MarginLayoutParams layoutParams = (ViewGroup.MarginLayoutParams) viewDrag.getLayoutParams();

        if (layoutParams.leftMargin > maxMarginLeft // Beyond the limits
                || layoutParams.leftMargin == oldMaxMarginLeft) { // Used to be attached to the end before orientation changed
            layoutParams.leftMargin = maxMarginLeft;
        }
        if (layoutParams.topMargin > maxMarginTop
                || layoutParams.topMargin == oldMaxMarginTop) {
            layoutParams.topMargin = maxMarginTop;
        }

        viewDrag.setLayoutParams(layoutParams);
    }

    private void onActionDown(MotionEvent event) {
        prevDragX = (int) event.getRawX();
        prevDragY = (int) event.getRawY();
    }

    private void onActionMove(MotionEvent event) {
        ViewGroup.MarginLayoutParams layoutParams = (ViewGroup.MarginLayoutParams) viewDrag.getLayoutParams();

        int deltaX = (int) event.getRawX() - prevDragX;
        int deltaY = (int) event.getRawY() - prevDragY;

        prevDragX = (int) event.getRawX();
        layoutParams.leftMargin = calculateMargin(deltaX, layoutParams.leftMargin, maxMarginLeft);

        prevDragY = (int) event.getRawY();
        layoutParams.topMargin = calculateMargin(deltaY, layoutParams.topMargin, maxMarginTop);

        viewDrag.setLayoutParams(layoutParams);
    }

    private void onActionUp() {
        Point viewRelativePosition = getRelativePositionInParent(viewDrag);

        int viewCenterX = viewRelativePosition.x + viewDrag.getWidth() / 2;
        int viewCenterY = viewRelativePosition.y + viewDrag.getHeight() / 2;

        View parent = (View) viewDrag.getParent();
        int parentCenterX = parent.getWidth() / 2;
        int parentCenterY = parent.getHeight() / 2;

        int leftMargin;
        int topMargin;
        if (viewCenterX > parentCenterX) { // Right corner
            leftMargin = maxMarginLeft;
        } else { // Left corner
            leftMargin = 0;
        }

        if (viewCenterY < parentCenterY) { // Top corner
            topMargin = 0;
        } else { // Bottom corner
            topMargin = maxMarginTop;
        }

        startMarginChangeAnimation(leftMargin, topMargin);
    }

    private void startMarginChangeAnimation(int leftMargin, int topMargin) {
        ViewGroup.MarginLayoutParams layoutParams = (ViewGroup.MarginLayoutParams) viewDrag.getLayoutParams();

        ValueAnimator animatorMarginLeft = ValueAnimator.ofInt(layoutParams.leftMargin, leftMargin);
        animatorMarginLeft.addUpdateListener(valueAnimator -> {
            layoutParams.leftMargin = (Integer) valueAnimator.getAnimatedValue();
            viewDrag.requestLayout();
        });

        ValueAnimator animatorMarginRight = ValueAnimator.ofInt(layoutParams.topMargin, topMargin);
        animatorMarginRight.addUpdateListener(valueAnimator -> {
            layoutParams.topMargin = (Integer) valueAnimator.getAnimatedValue();
            viewDrag.requestLayout();
        });

        AnimatorSet animatorSet = new AnimatorSet();
        animatorSet.playTogether(animatorMarginLeft, animatorMarginRight);
        animatorSet.setDuration(DURATION_ANIMATION_CORNER);
        animatorSet.start();
    }

    private int calculateMargin(int delta, int oldMargin, int maxMargin) {
        int newMargin = oldMargin + delta;
        if (newMargin < 0) {
            newMargin = 0;
        } else if (newMargin > maxMargin) {
            newMargin = maxMargin;
        }
        return newMargin;
    }

    private Point getRelativePositionInParent(View view) {
        int[] locationView = new int[2];
        int[] locationParent = new int[2];

        view.getLocationOnScreen(locationView);
        View parent = (View) view.getParent();
        parent.getLocationOnScreen(locationParent);

        float relativeX = locationView[0] - locationParent[0];
        float relativeY = locationView[1] - locationParent[1];

        return new Point((int) relativeX, (int) relativeY);
    }
}
