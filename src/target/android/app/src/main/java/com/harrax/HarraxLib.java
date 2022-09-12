package com.harrax;

import android.view.Surface;

public class HarraxLib {

    static {
        System.loadLibrary("harrax");
    }

    public static native void start(Surface surface);
    public static native void end();
    public static native void surfaceChanged(Surface surface);
}
