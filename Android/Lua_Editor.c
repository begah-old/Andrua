#include "SharedInterface.h"

#include "Application/Application.h"

struct Window *Window;

clock_t Super_long_press = -1, Double_Click_Timer = -1;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    Java_VM = vm;
    return JNI_VERSION_1_6;
}

void Java_begah_editor_lua_Native_NSetUp(JNIEnv* env, jobject thiz, jint Width, jint Height, jobject assetManager, jstring DocumentPath)
{
    Game_Width = (int)Width;
    Game_Height = (int)Height;

    Java_AssetManager = (*env)->NewGlobalRef(env, assetManager);

    Game_FPS = 60;

    const char *Temp = (*env)->GetStringUTFChars(env, DocumentPath, NULL);
    Executable_Path = malloc(sizeof(char) * (String_length(Temp) + 2));
    memcpy(Executable_Path, Temp, sizeof(char) * (String_length(Temp)));
    Executable_Path[String_length(Temp)] = '/';
    Executable_Path[String_length(Temp) + 1] = '\0';

    (*env)->ReleaseStringUTFChars(env, DocumentPath, Temp);

    __android_log_print(ANDROID_LOG_ERROR, "Native", "Document path : %s", Executable_Path);

    Window_Create(&Window, "Lua Editor", Game_Width, Game_Height);
    Util_Init(Window);

    Application_Init();
}

void Java_begah_editor_lua_Native_NRender(JNIEnv* env, jobject thiz, jint FPS)
{
    Game_FPS = FPS;
    double DistanceX = Mouse.x - Mouse.prevX, DistanceY = Mouse.y - Mouse.prevY;
    DistanceX = DistanceX < 0 ? DistanceX * -1 : DistanceX;
    DistanceY = DistanceY < 0 ? DistanceY * -1 : DistanceY;

    double Distance = sqrt(DistanceX + DistanceY);

    /* Super long press logic */
    if(Mouse.isLongedPressed)
    {
        if(Distance > 1.0)
            Super_long_press = -1;
        else if(!Mouse.isSuper_longPress && Super_long_press > 0)
        {
            float diff = (((float)clock() - (float)Super_long_press) / CLOCKS_PER_SEC ) * 10;
            if(diff > 3.0f)
            {
                Mouse.isSuper_longPress = true;
                Super_long_press = -1;
            }
        }
    }

    if(Double_Click_Timer > 0)
    {
        float diff = (((float)clock() - (float)Double_Click_Timer) / CLOCKS_PER_SEC) * 10;

        if(diff > 1.0f)
            Double_Click_Timer = -1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(OnScreen_Keyboard) Engine_renderKeyboard();

    Application_Render();

    Util_Flush();

    Mouse.justPressed = Mouse.justReleased = Mouse.justLongPressedReleased = Mouse.justScrolled =
    Keyboard.justTyped = Keyboard.justPressed = Mouse.justQuickPressed =
    Mouse.justSuper_longReleased = Keyboard.justReleased = false;
    Mouse.prevX = Mouse.x;
    Mouse.prevY = Mouse.y;
}

void Java_begah_editor_lua_Native_Close(JNIEnv* env, jobject thiz)
{
    Application_Free();
    Util_Free(&Window);
}

void Java_begah_editor_lua_Native_justScrolled(JNIEnv* env, jobject thiz, float x, float y)
{
    Mouse.justScrolled = true;
    Mouse.scrollX = x;
    Mouse.scrollY = y;
    __android_log_print(ANDROID_LOG_ERROR, "Native", "Just Scrolled");
}

void Java_begah_editor_lua_Native_justPressed(JNIEnv* env, jobject thiz)
{
    Mouse.justPressed = true;
    Mouse.isPressed = true;
    __android_log_print(ANDROID_LOG_ERROR, "Native", "Just Pressed");
}

void Java_begah_editor_lua_Native_justReleased(JNIEnv* env, jobject thiz)
{
    if(Mouse.isLongedPressed) Mouse.justLongPressedReleased = true;
    Mouse.isLongedPressed = false;
    Mouse.justReleased = true;
    Mouse.isPressed = false;

    if(Mouse.isSuper_longPress)
        Mouse.justSuper_longReleased = true;
    Mouse.isSuper_longPress = false;
    Super_long_press = -1;

    if(!Mouse.justLongPressedReleased) {
        Mouse.justQuickPressed = true;

        if(Double_Click_Timer == -1)
            Double_Click_Timer = -1;
        else {
            Mouse.just_DoubleClicked = true;
            Double_Click_Timer = -1;
        }
    }

    __android_log_print(ANDROID_LOG_ERROR, "Native", "Just released : %i", Mouse.justLongPressedReleased);
}

void Java_begah_editor_lua_Native_longPress(JNIEnv* env, jobject thiz)
{
    Mouse.isLongedPressed = true;
    Mouse.isPressed = true;
    Super_long_press = clock();
    __android_log_print(ANDROID_LOG_ERROR, "Native", "Just long Pressed");
}

void Java_begah_editor_lua_Native_backKeyPress(JNIEnv* env, jobject thiz)
{
    Keyboard.justPressed = true;
    Keyboard.Key.key = GLFW_KEY_ESCAPE;
    Keyboard.Key.mods = 0;

    __android_log_print(ANDROID_LOG_ERROR, "Native", "back key Pressed");
}

void Java_begah_editor_lua_Native_backKeyReleased(JNIEnv* env, jobject thiz)
{
    Keyboard.justReleased = true;
    Keyboard.Key.key = GLFW_KEY_ESCAPE;
    Keyboard.Key.mods = 0;

    __android_log_print(ANDROID_LOG_ERROR, "Native", "back key Released");
}

void Java_begah_editor_lua_Native_updateFinger(JNIEnv* env, jobject thiz, float x, float y)
{
    Mouse.prevX = Mouse.x;
    Mouse.prevY = Mouse.y;
    Mouse.x = x;
    Mouse.y = Game_Height - y;
}