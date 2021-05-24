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

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE ${fltk_SOURCE_DIR} ${fltk_BINARY_DIR})
target_compile_features(main PRIVATE cxx_std_17)

if(UNIX AND NOT APPLE)
  find_package(PkgConfig)
  pkg_check_modules(WEBKITGTK "gtk+-3.0 webkit2gtk-4.0")
  add_definitions(${WEBKITGTK_CFLAGS} ${WEBKITGTK_CFLAGS_OTHER})
  target_link_libraries(main PRIVATE fltk fltk_gl X11 ${WEBKITGTK_LIBRARIES})
elseif(WIN32)
  set_target_properties(main PROPERTIES VS_CPPWINRT true)
  target_link_libraries(main PRIVATE fltk webview WebView2Loader)
elseif(APPLE)
  target_link_libraries(main PRIVATE fltk objc -framework WebKit)
else()
  message(FATAL_ERROR "Unsupported platform")
endif()
