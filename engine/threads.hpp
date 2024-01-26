#pragma once

/*
* Emscripten port is massively simplified by not using pthread emulation, so macros are used throughout
*/
#ifndef __EMSCRIPTEN__

    #include <thread>
    #include <mutex>

    #define DECLARE_MUTEX(name) std::mutex name;
    #define DECLARE_EXTERN_MUTEX(name) extern std::mutex name;
    #define LOCK_GUARD(name) std::lock_guard<std::mutex> guard(name)
    #define LAUNCH_THREAD_DETACH(func) auto t = std::thread(func); t.detach()

#else

    #define DECLARE_MUTEX(name) 
    #define DECLARE_EXTERN_MUTEX(name) 
    #define LOCK_GUARD(name) (void)0
    #define LAUNCH_THREAD_DETACH(func) (void)0

#endif

