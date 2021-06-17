package com.teevid.sample;

import androidx.fragment.app.Fragment;

public interface Navigation {

    void showFragment(Fragment fragment, boolean addToBackStack);
}
