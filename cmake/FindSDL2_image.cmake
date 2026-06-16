# FindSDL2_image.cmake
find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h
  PATHS /usr/include/SDL2 /usr/local/include/SDL2 /usr/include /usr/local/include
)

find_library(SDL2_IMAGE_LIBRARY NAMES SDL2_image SDL2_image-2.0
  PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image DEFAULT_MSG SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)

if(SDL2_IMAGE_FOUND)
    set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})
    set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIR})
    message(STATUS "Found SDL2_image: ${SDL2_IMAGE_LIBRARY}")
else()
    message(FATAL_ERROR "SDL2_image not found!")
endif()