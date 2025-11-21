#include <jni.h>
#include <android/native_window_jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <chrono>
#include "game.h"

static Game* game = nullptr;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
static ANativeWindow* window = nullptr;
static std::chrono::high_resolution_clock::time_point lastTime;
static bool initialized = false;

// Store Java VM and GameView object for callbacks
static JavaVM* g_jvm = nullptr;
static jobject g_gameViewObj = nullptr;

// Helper function to show toast from native code
void showToast(const char* message) {
    if (g_jvm == nullptr || g_gameViewObj == nullptr) return;
    
    JNIEnv* env;
    bool needDetach = false;
    
    int getEnvStat = g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
        if (g_jvm->AttachCurrentThread(&env, nullptr) != 0) {
            return;
        }
        needDetach = true;
    }
    
    jclass gameViewClass = env->GetObjectClass(g_gameViewObj);
    jmethodID showToastMethod = env->GetMethodID(gameViewClass, "showToast", "(Ljava/lang/String;)V");
    
    if (showToastMethod != nullptr) {
        jstring jMessage = env->NewStringUTF(message);
        env->CallVoidMethod(g_gameViewObj, showToastMethod, jMessage);
        env->DeleteLocalRef(jMessage);
    }
    
    env->DeleteLocalRef(gameViewClass);
    
    if (needDetach) {
        g_jvm->DetachCurrentThread();
    }
}

extern "C" {

JNIEXPORT void JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeInit(JNIEnv* env, jobject obj, jobject jSurface) {
    // Store Java VM and GameView object for callbacks
    env->GetJavaVM(&g_jvm);
    if (g_gameViewObj != nullptr) {
        env->DeleteGlobalRef(g_gameViewObj);
    }
    g_gameViewObj = env->NewGlobalRef(obj);
    
    // Clean up any existing resources first
    if (context != EGL_NO_CONTEXT) {
        eglDestroyContext(display, context);
        context = EGL_NO_CONTEXT;
    }
    if (surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
    if (display != EGL_NO_DISPLAY) {
        eglTerminate(display);
        display = EGL_NO_DISPLAY;
    }
    if (game != nullptr) {
        delete game;
        game = nullptr;
    }
    
    window = ANativeWindow_fromSurface(env, jSurface);
    
    // Initialize EGL
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);
    
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    
    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);
    
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(display, surface, surface, context);
    
    // Get screen dimensions
    EGLint width, height;
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
    
    // Initialize game
    game = new Game();
    game->setToastCallback(showToast);
    game->init(width, height);
    
    glViewport(0, 0, width, height);
    
    lastTime = std::chrono::high_resolution_clock::now();
    initialized = true;
}

JNIEXPORT void JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeRender(JNIEnv* env, jobject obj) {
    if (!initialized || !game || display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) return;
    
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        return;
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;
    
    // Limit delta time to prevent large jumps
    if (deltaTime > 0.1f) deltaTime = 0.016f;
    
    game->update(deltaTime);
    game->render();
    
    eglSwapBuffers(display, surface);
}

JNIEXPORT jboolean JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeTouch(JNIEnv* env, jobject obj, jfloat x, jfloat y) {
    if (!initialized || !game) return JNI_FALSE;
    
    return game->handleTouch(x, y) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeGetScore(JNIEnv* env, jobject obj) {
    if (!game) return 0;
    return game->getScore();
}

JNIEXPORT jint JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeGetRound(JNIEnv* env, jobject obj) {
    if (!game) return 0;
    return game->getRound();
}

JNIEXPORT jboolean JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeIsGameOver(JNIEnv* env, jobject obj) {
    if (!game) return JNI_FALSE;
    return game->isGameOver() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeReset(JNIEnv* env, jobject obj) {
    if (game) {
        game->reset();
    }
}

JNIEXPORT jfloat JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeGetBgColorR1(JNIEnv* env, jobject obj) {
    if (!game) return 1.0f;
    return game->getBgColorR1();
}

JNIEXPORT jfloat JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeGetBgColorG1(JNIEnv* env, jobject obj) {
    if (!game) return 1.0f;
    return game->getBgColorG1();
}

JNIEXPORT jfloat JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeGetBgColorB1(JNIEnv* env, jobject obj) {
    if (!game) return 1.0f;
    return game->getBgColorB1();
}

JNIEXPORT jfloat JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeGetBgColorR2(JNIEnv* env, jobject obj) {
    if (!game) return 1.0f;
    return game->getBgColorR2();
}

JNIEXPORT jfloat JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeGetBgColorG2(JNIEnv* env, jobject obj) {
    if (!game) return 1.0f;
    return game->getBgColorG2();
}

JNIEXPORT jfloat JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeGetBgColorB2(JNIEnv* env, jobject obj) {
    if (!game) return 1.0f;
    return game->getBgColorB2();
}

JNIEXPORT void JNICALL
Java_com_rog3rb0t_touchgame_GameView_nativeDestroy(JNIEnv* env, jobject obj) {
    if (game) {
        delete game;
        game = nullptr;
    }
    
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
            context = EGL_NO_CONTEXT;
        }
        
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
            surface = EGL_NO_SURFACE;
        }
        
        eglTerminate(display);
        display = EGL_NO_DISPLAY;
    }
    
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    
    initialized = false;
}

} // extern "C"
