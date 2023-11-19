package com.example.vulkanandroidrenderer

class NativeLib {

    /**
     * A native method that is implemented by the 'vulkanandroidrenderer' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'vulkanandroidrenderer' library on application startup.
        init {
            System.loadLibrary("vulkanandroidrenderer")
        }
    }
}