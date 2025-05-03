cmake_minimum_required (VERSION 3.21)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW) # enable MSVC_RUNTIME_LIBRARY
endif()

# setup FBX SDK library bits
add_library(fbxsdk STATIC IMPORTED GLOBAL)
set(FBX_ROOT ${CMAKE_SOURCE_DIR}/external/fbxsdk)
set_target_properties(fbxsdk PROPERTIES
    IMPORTED_LOCATION ${FBX_ROOT}/lib/x64/release/libfbxsdk-mt.lib
    IMPORTED_LOCATION_DEBUG ${FBX_ROOT}/lib/x64/debug/libfbxsdk-mt.lib
    INTERFACE_INCLUDE_DIRECTORIES ${FBX_ROOT}/include
    IMPORTED_IMPLIB ${FBX_ROOT}/lib/x64/release/libfbxsdk-mt.lib
    IMPORTED_IMPLIB_DEBUG ${FBX_ROOT}/lib/x64/debug/libfbxsdk-mt.lib
)
add_library(libxml2 STATIC IMPORTED GLOBAL)
set_target_properties(libxml2 PROPERTIES
    IMPORTED_LOCATION ${FBX_ROOT}/lib/x64/release/libxml2-mt.lib
    IMPORTED_LOCATION_DEBUG ${FBX_ROOT}/lib/x64/debug/libxml2-mt.lib
    INTERFACE_INCLUDE_DIRECTORIES ${FBX_ROOT}/include/libxml2
    IMPORTED_IMPLIB ${FBX_ROOT}/lib/x64/release/libxml2-mt.lib
    IMPORTED_IMPLIB_DEBUG ${FBX_ROOT}/lib/x64/debug/libxml2-mt.lib
)
target_compile_definitions(libxml2 INTERFACE LIBXML_STATIC)    
add_library(zlib STATIC IMPORTED GLOBAL)
set_target_properties(zlib PROPERTIES
    IMPORTED_LOCATION ${FBX_ROOT}/lib/x64/release/zlib-mt.lib
    IMPORTED_LOCATION_DEBUG ${FBX_ROOT}/lib/x64/debug/zlib-mt.lib
    IMPORTED_IMPLIB ${FBX_ROOT}/lib/x64/release/zlib-mt.lib
    IMPORTED_IMPLIB_DEBUG ${FBX_ROOT}/lib/x64/debug/zlib-mt.lib
)
target_link_libraries(fbxsdk INTERFACE zlib)
target_link_libraries(fbxsdk INTERFACE libxml2)
#


project ("test_fbxsdk")
add_executable (test_fbxsdk main_fbxsdk.cpp)
target_link_libraries(test_fbxsdk PRIVATE fbxsdk)
set_property(TARGET test_fbxsdk PROPERTY CXX_STANDARD 17)
set_property(TARGET test_fbxsdk PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_compile_definitions(test_fbxsdk PRIVATE CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_WARNINGS NOMINMAX)
