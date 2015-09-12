package com.andrua;

import android.content.Context;
import android.opengl.GLSurfaceView;

/**
 * Created by root on 8/5/15.
 */
public class Opengl_View extends GLSurfaceView {

    public final Opengl_Renderer Renderer;

    public static Opengl_View View;

    public Opengl_View(Context context){
        super(context);

        View = this;

        // Create an OpenGL ES 2.0 context
        setEGLContextClientVersion(2);

        Renderer = new Opengl_Renderer();

        // Set the Renderer for drawing on the GLSurfaceView
        setRenderer(Renderer);
    }
}
