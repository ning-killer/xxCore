#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "jsoncpp_lib" for configuration "Release"
set_property(TARGET jsoncpp_lib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(jsoncpp_lib PROPERTIES
  IMPORTED_LOCATION_RELEASE "/home/xiong/workspace/test/3dr/EmxThirdParty/source/jsoncpp-1.9.3/build/x86_64/lib/libjsoncpp.so.1.9.3"
  IMPORTED_SONAME_RELEASE "libjsoncpp.so.24"
  )

list(APPEND _IMPORT_CHECK_TARGETS jsoncpp_lib )
list(APPEND _IMPORT_CHECK_FILES_FOR_jsoncpp_lib "/home/xiong/workspace/test/3dr/EmxThirdParty/source/jsoncpp-1.9.3/build/x86_64/lib/libjsoncpp.so.1.9.3" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
