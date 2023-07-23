The Vulkan library is present on all devices supporting API level 24 or later, but apps must check 
at runtime that the necessary GPU hardware support is available. Devices without Vulkan support 
will return zero devices from vkEnumeratePhysicalDevices.

[https://developer.android.com/ndk/guides/stable_apis]