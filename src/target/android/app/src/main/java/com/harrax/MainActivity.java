package com.harrax;

import android.app.Activity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class MainActivity extends Activity implements SurfaceHolder.Callback {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.main);
        SurfaceView surfaceView = findViewById(R.id.surface_view);
        surfaceView.getHolder().addCallback(this);
    }

    @Override protected void onStart() {
        super.onStart();
    }

    @Override protected void onPause() {
        super.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        HarraxLib.start(holder.getSurface());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder,
                               int format, int width, int height) {
        HarraxLib.surfaceChanged(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        HarraxLib.end();
    }
}