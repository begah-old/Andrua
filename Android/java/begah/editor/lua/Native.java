package begah.editor.lua;

import android.content.ClipData;
import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLU;
import android.os.Build;
import android.os.Environment;
import android.os.Looper;
import android.util.Log;

import java.io.File;

/**
 * Created by root on 8/5/15.
 */
public class Native {
    private static Lua_Editor Activity;
    private static AssetManager AssetManager;
    public static Context Context;

    public static float Finger_X, Finger_Y;

    private static int FPS = 30, FPS_Count = 0;
    private static long FPSCounter_Start = -1;

    private static boolean JustScrolled, JustPressed, JustReleased, longPressed, backKey, releasedbackKey;
    private static float Scrolled_X, Scrolled_Y;

    public static void SetActivity(Lua_Editor Activity2)
    {
        Activity = Activity2;
        AssetManager = Activity.getResources().getAssets();
    }
    private static native void NSetUp(int Width, int Height, AssetManager assetManager, String DocumentPath);
    private static native void NRender(int FPS);
    private static native void Close();

    // For input
    public static void Java_justScrolled(float x, float y)
    {
        JustScrolled = true;
        Scrolled_X = x;
        Scrolled_Y = y;
    }

    public static void Java_justPressed()
    {
        JustPressed = true;
    }

    public static void Java_justReleased()
    {
        JustReleased = true;
    }

    public static void Java_longPress()
    {
        longPressed = true;
    }

    public static void Java_backKey()
    {
        backKey = true;
    }

    public static void Java_releasedbackKey()
    {
        releasedbackKey = true;
    }

    public static native void justScrolled(float x, float y);
    public static native void justPressed();
    public static native void justReleased();
    public static native void longPress();
    public static native void backKeyPress();
    public static native void backKeyReleased();
    public static native void updateFinger(float x, float y);

    public static void SetUp(int W, int H)
    {
        File docsFolder = new File(Environment.getExternalStorageDirectory() + "/Documents");
        boolean isPresent = true;
        if (!docsFolder.exists()) {
            Log.d("Error", "Creating Document folder");
            isPresent = docsFolder.mkdir();
        }
        if(!isPresent)
        {
            Log.d("Error", "Couldn't create Document folder");
            System.exit(1);
        }

        docsFolder = new File(Environment.getExternalStorageDirectory() + "/Documents/Lua Editor");
        if(!docsFolder.exists()) {
            Log.d("Error", "Creating Lua Editor folder");
            isPresent = docsFolder.mkdir();
        }
        if(!isPresent)
        {
            Log.d("Error", "Couldn't create Lua Editor folder");
            System.exit(1);
        }

        try {
            Looper.prepare();
        } catch(Exception e)
        {
            // If my app gets reloaded, this causes an error
            Log.d("Java", "Normal error on app resumed : " + e.toString());
        }

        NSetUp(W, H, AssetManager, Environment.getExternalStorageDirectory() + "/Documents/Lua Editor");
    }

    public static void Render() {
        if (FPSCounter_Start == -1) {
            FPSCounter_Start = System.nanoTime() / 1000000000;
        }
        updateFinger(Finger_X, Finger_Y);

        if (JustPressed) {
            JustPressed = false;
            justPressed();
        }
        if (JustReleased) {
            JustReleased = false;
            justReleased();
        }
        if (JustScrolled) {
            JustScrolled = false;
            justScrolled(Scrolled_X, Scrolled_Y);
        }
        if (longPressed) {
            longPressed = false;
            longPress();
        }
        if (backKey) {
            backKey = false;
            backKeyPress();
        }
        if (releasedbackKey) {
            releasedbackKey = false;
            backKeyReleased();
        }

        NRender(FPS);

        FPS_Count++;
        if (System.nanoTime() / 1000000000 - FPSCounter_Start >= 1)
        {
            FPSCounter_Start = -1;
            FPS = FPS_Count;
            FPS_Count = 0;
        }
    }

    public static void PrintOpenglError(int ErrorID)
    {
        System.out.println(GLU.gluErrorString(ErrorID));
    }

    public static String getClipboard()
    {
        int sdk = Build.VERSION.SDK_INT;

        if(sdk < Build.VERSION_CODES.HONEYCOMB) {
            android.text.ClipboardManager clipboard = (android.text.ClipboardManager) Activity.getSystemService(Context.CLIPBOARD_SERVICE);
            return clipboard.getText().toString();
        } else {
            try {
                android.content.ClipboardManager clipboard = (android.content.ClipboardManager) Activity.getSystemService(Context.CLIPBOARD_SERVICE);
                if(clipboard.hasPrimaryClip())
                {
                    ClipData data = clipboard.getPrimaryClip();
                    if(data.getItemCount() > 0)
                    {
                        ClipData.Item it = data.getItemAt(0);
                        return it.coerceToText(Context).toString();
                    }
                }
                return " ";
            } catch (Exception e)
            {
                Log.d("Java Error : ", e.toString());
                return  " ";
            }
        }
    }

    public static void setClipboard(final String string)
    {
        int sdk = Build.VERSION.SDK_INT;

        if(sdk < Build.VERSION_CODES.HONEYCOMB) {
            android.text.ClipboardManager clipboard = (android.text.ClipboardManager) Activity.getSystemService(Context.CLIPBOARD_SERVICE);
            clipboard.setText(string);
        } else {
            try {
                android.content.ClipboardManager clipboard = (android.content.ClipboardManager) Activity.getSystemService(Context.CLIPBOARD_SERVICE);
                android.content.ClipData clip = android.content.ClipData.newPlainText("Lua Editor", string);
                clipboard.setPrimaryClip(clip);
            } catch (Exception e)
            {
                Log.d("Java Error : ", e.toString());
            }
        }
    }

    static {
        System.loadLibrary("Lua_Editor");
    }
}
