cmake_minimum_required(VERSION 3.15)
project(webview)
set(FLTK_BUILD_TEST OFF CACHE BOOL " " FORCE)
include(FetchContent)

FetchContent_Declare( 
  FLTK
  GIT_REPOSITORY https://github.com/FLTK/FLTK
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(FLTK)

option(BUILD_EXAMPLE "Builds the example" ON)

if(APPLE)
add_library(fltk_webview src/Fl_WebView.cxx src/helper.m)
else()
add_library(fltk_webview src/Fl_WebView.cxx)
endif()
target_include_directories(fltk_webview PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/webview/include ${fltk_SOURCE_DIR} ${fltk_BINARY_DIR})
target_compile_features(fltk_webview PRIVATE cxx_std_17)

if(UNIX AND NOT APPLE)
  find_package(PkgConfig)
  pkg_check_modules(WEBKITGTK "gtk+-3.0 webkit2gtk-4.0")
  add_definitions(${WEBKITGTK_CFLAGS} ${WEBKITGTK_CFLAGS_OTHER})
  target_link_libraries(fltk_webview PRIVATE fltk fltk_gl X11 ${WEBKITGTK_LIBRARIES})
elseif(WIN32)
  set_target_properties(fltk_webview PROPERTIES VS_CPPWINRT true)
  target_link_libraries(fltk_webview PRIVATE fltk gdiplus version ${CMAKE_SOURCE_DIR}/webview/lib/WebView2LoaderStatic.lib)
elseif(APPLE)
  target_link_libraries(fltk_webview PRIVATE fltk "-framework WebKit" "-framework Cocoa")
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

if (${BUILD_EXAMPLE})
  add_executable(example1 examples/example1.cpp)
  target_include_directories(example1 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/webview/include ${fltk_SOURCE_DIR} ${fltk_BINARY_DIR})
  target_link_libraries(example1 PRIVATE fltk_webview)
endif()