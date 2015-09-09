package begah.editor.lua;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;

/**
 * Created by root on 8/5/15.
 */
public class Lua_Editor extends Activity implements GestureDetector.OnGestureListener,
        GestureDetector.OnDoubleTapListener {

    private static final String DEBUG_TAG = "Gestures";

    private Opengl_View GL_View;
    public GestureDetector Detector;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Native.SetActivity(this);

        GL_View = new Opengl_View(this);
        setContentView(GL_View);

        GL_View.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View view, MotionEvent event) {

                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    Native.Java_justPressed();
                }
                else if (event.getAction() == MotionEvent.ACTION_UP) {
                    Native.Java_justReleased();
                }

                onTouchEvent(event);
                return true;
            }
        });

        Detector = new GestureDetector(this,this);
        Detector.setOnDoubleTapListener(this);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            Native.Java_backKey();
        }
        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            Native.Java_releasedbackKey();
        }
        return true;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event){
        this.Detector.onTouchEvent(event);
        // Be sure to call the superclass implementation
        Native.Finger_X = event.getX();
        Native.Finger_Y = event.getY();
        return super.onTouchEvent(event);
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent motionEvent) {
        //Log.d(DEBUG_TAG, "onSingleTapConfirmed: " + motionEvent.toString());
        Native.Finger_X = motionEvent.getX();
        Native.Finger_Y = motionEvent.getY();
        return true;
    }

    @Override
    public boolean onDoubleTap(MotionEvent motionEvent) {
        //Log.d(DEBUG_TAG, "onDoubleTap: " + motionEvent.toString());
        Native.Finger_X = motionEvent.getX();
        Native.Finger_Y = motionEvent.getY();
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent motionEvent) {
        //Log.d(DEBUG_TAG, "onDoubleTapEvent: " + motionEvent.toString());
        Native.Finger_X = motionEvent.getX();
        Native.Finger_Y = motionEvent.getY();
        return true;
    }

    @Override
    public boolean onDown(MotionEvent motionEvent) {
        //Log.d(DEBUG_TAG, "onDown: " + motionEvent.toString());
        Native.Finger_X = motionEvent.getX();
        Native.Finger_Y = motionEvent.getY();
        return true;
    }

    @Override
    public void onShowPress(MotionEvent motionEvent) {
        //Log.d(DEBUG_TAG, "onShowPress: " + motionEvent.toString());
        Native.Finger_X = motionEvent.getX();
        Native.Finger_Y = motionEvent.getY();
    }

    @Override
    public boolean onSingleTapUp(MotionEvent motionEvent) {
        //Log.d(DEBUG_TAG, "onSingleTapUp: " + motionEvent.toString());
        Native.Finger_X = motionEvent.getX();
        Native.Finger_Y = motionEvent.getY();
        return true;
    }

    @Override
    public boolean onScroll(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {
        //Log.d(DEBUG_TAG, "onScroll: " + motionEvent.toString() + motionEvent1.toString());
        Native.Finger_X = motionEvent.getX();
        Native.Finger_Y = motionEvent.getY();

        float x = motionEvent1.getX() - motionEvent.getX();
        float y = motionEvent1.getY() - motionEvent.getY();
        if(x > 0) x = 2;
        else if(x < 0) x = -2;
        if(y > 0) y = 2;
        else if(y < 0) y = -2;

        Native.Java_justScrolled(x, y);
        return true;
    }

    @Override
    public void onLongPress(MotionEvent motionEvent) {
        //Log.d(DEBUG_TAG, "onLongPress: " + motionEvent.toString());
        Native.Finger_X = motionEvent.getX();
        Native.Finger_Y = motionEvent.getY();

        Native.Java_longPress();
    }

    @Override
    public boolean onFling(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {
        //Log.d(DEBUG_TAG, "onFling: " + motionEvent.toString()+motionEvent1.toString());
        Native.Finger_X = motionEvent.getX();
        Native.Finger_Y = motionEvent.getY();

        return true;
    }
}
