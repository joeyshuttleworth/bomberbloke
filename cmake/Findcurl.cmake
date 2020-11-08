# - Find Curl
# Find the native Curl includes and libraries
#
#  CURL_INCLUDE_DIR - where to find Curl headers.
#  CURL_LIBRARIES   - List of libraries when using libcurl.
#  CURL_FOUND       - True if libcurl found.

if(CURL_INCLUDE_DIR)
    # Already in cache, be silent
    set(CURL_FIND_QUIETLY TRUE)
endif(CURL_INCLUDE_DIR)

find_path(CURL_INCLUDE_DIR curl/curl.h)
find_library(CURL_LIBRARY NAMES curl curl_static libcurl libcurl_static)

if(_WIN32)
	set(CURL_INCLUDE_DIR /usr/include/curl)
	set(CURL_LIBRARY ${PROJECT_SOURCE_DIR}/windows_libraries/libcurl.a)
	set(CURL_FOUND TRUE)
endif(_WIN32)

# Handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CURL DEFAULT_MSG
    CURL_INCLUDE_DIR CURL_LIBRARY)

if(CURL_FOUND)
  set(CURL_LIBRARIES ${CURL_LIBRARY})
else(CURL_FOUND)
  set(CURL_LIBRARIES)
endif(CURL_FOUND)

mark_as_advanced(CURL_INCLUDE_DIR CURL_LIBRARY)
