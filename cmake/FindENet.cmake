# - Find ENet
# Find the native ENet includes and libraries
#
#  ENET_INCLUDE_DIR - where to find ENet headers.
#  ENET_LIBRARIES   - List of libraries when using libenet.
#  ENET_FOUND       - True if libenet found.

if(ENET_INCLUDE_DIR)
    # Already in cache, be silent
    set(ENET_FIND_QUIETLY TRUE)
endif(ENET_INCLUDE_DIR)

find_path(ENET_INCLUDE_DIR enet/enet.h)

IF(_WIN32)
	message("Finding enet for WIN32")
	set(ENET_LIBRARY ${PROJECT_SOURCE_DIR}/windows_libraries/libenet.a)
  set(ENET_FOUND TRUE)
  set(ENET_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/windows_libraries)
ELSE()
find_library(ENET_LIBRARY NAMES enet enet_static libenet libenet_static)
ENDIF(_WIN32)


# Handle the QUIETLY and REQUIRED arguments and set ENET_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ENet DEFAULT_MSG
    ENET_INCLUDE_DIR ENET_LIBRARY)

if(ENET_FOUND)
  set(ENET_LIBRARIES ${ENET_LIBRARY})
else(ENET_FOUND)
  set(ENET_LIBRARIES)
endif(ENET_FOUND)

mark_as_advanced(ENET_INCLUDE_DIR ENET_LIBRARY)
