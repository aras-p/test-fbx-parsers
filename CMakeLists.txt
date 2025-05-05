cmake_minimum_required (VERSION 3.21)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW) # enable MSVC_RUNTIME_LIBRARY
endif()
if(POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW) # avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24+
endif()

# setup FBX SDK library
add_library(fbxsdk STATIC IMPORTED GLOBAL)
set(FBX_ROOT ${CMAKE_SOURCE_DIR}/external/fbxsdk)
if(WIN32)
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
    target_link_libraries(fbxsdk INTERFACE zlib libxml2)
elseif(APPLE)
    set_target_properties(fbxsdk PROPERTIES
        IMPORTED_LOCATION ${FBX_ROOT}/lib/clang/release/libfbxsdk.a
        IMPORTED_LOCATION_DEBUG ${FBX_ROOT}/lib/clang/debug/libfbxsdk.a
        INTERFACE_INCLUDE_DIRECTORIES ${FBX_ROOT}/include
    )
    target_link_libraries(fbxsdk INTERFACE xml2 z iconv "-framework CoreFoundation")
else()
    message(FATAL_ERROR "Don't know how to setup FBX SDK on this platform")
endif()
#

# setup AssImp
include(FetchContent)
FetchContent_Declare(
	assimp
	URL https://github.com/assimp/assimp/archive/refs/tags/v5.4.3.zip # latest (2024 Aug) at time of writing
)
option(BUILD_SHARED_LIBS "Build shared libs" OFF)
option(ASSIMP_NO_EXPORT "" OFF)
option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT "" OFF)
option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT "" OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS "" OFF)
option(ASSIMP_BUILD_TESTS "" OFF)
option(ASSIMP_IGNORE_GIT_HASH "" ON)
option(USE_STATIC_CRT "" ON)
option(ASSIMP_INSTALL "" OFF)
option(ASSIMP_BUILD_ASSIMP_VIEW "" OFF)
option(ASSIMP_BUILD_FBX_IMPORTER "" ON)
option(ASSIMP_BUILD_ZLIB "" ON)
option(ASSIMP_BUILD_USE_CCACHE "" OFF)
FetchContent_MakeAvailable(assimp)

project ("test_fbxsdk")
add_executable (test_fbxsdk main_fbxsdk.cpp)
target_link_libraries(test_fbxsdk PRIVATE fbxsdk)
set_property(TARGET test_fbxsdk PROPERTY CXX_STANDARD 17)
set_property(TARGET test_fbxsdk PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_compile_definitions(test_fbxsdk PRIVATE CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_WARNINGS NOMINMAX)

project ("test_openfbx")
add_executable (test_openfbx main_openfbx.cpp external/OpenFBX/libdeflate.c external/OpenFBX/ofbx.cpp)
set_property(TARGET test_openfbx PROPERTY CXX_STANDARD 17)
set_property(TARGET test_openfbx PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_compile_definitions(test_openfbx PRIVATE CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_WARNINGS NOMINMAX)

project ("test_assimp")
add_executable (test_assimp main_assimp.cpp)
target_include_directories(test_assimp PRIVATE ${assimp_SOURCE_DIR}/include)
target_link_libraries(test_assimp PRIVATE assimp::assimp)
set_property(TARGET test_assimp PROPERTY CXX_STANDARD 17)
set_property(TARGET test_assimp PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_compile_definitions(test_assimp PRIVATE CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_WARNINGS NOMINMAX)

project ("test_ufbx")
add_executable (test_ufbx main_ufbx.cpp external/ufbx/ufbx.c external/ufbx/ufbx.h)
set_property(TARGET test_ufbx PROPERTY CXX_STANDARD 17)
set_property(TARGET test_ufbx PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_compile_definitions(test_ufbx PRIVATE CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_WARNINGS NOMINMAX
    UFBX_NO_SUBDIVISION
    UFBX_NO_TESSELLATION
    UFBX_NO_GEOMETRY_CACHE
    UFBX_NO_SCENE_EVALUATION
    UFBX_NO_SKINNING_EVALUATION
    UFBX_NO_ANIMATION_BAKING
    UFBX_NO_TRIANGULATION
    UFBX_NO_INDEX_GENERATION
    UFBX_NO_FORMAT_OBJ
)
