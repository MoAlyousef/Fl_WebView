# Fl_WebView

A webview integration into FLTK proof of concept.

## Building
```
$ git clone https://github.com/moalyousef/fltk-webview
$ cd fltk-webview
$ cmake -B bin && cmake --build bin --parallel
```
This should build fltk along with this lib and the example in the examples directory.

## Usage
```cpp
#include "Fl_WebView.H"
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <cstdlib>


int main(int argc, char** argv) {
    auto main_win = new Fl_Double_Window(800, 600);
    auto wv_win = new Fl_WebView(5, 5, 790, 590);
    main_win->end();
    main_win->resizable(main_win);
    main_win->show();

    wv_win->init();
    wv_win->navigate("https://google.com");

    return Fl::run();
}
```

Building the repo will generate a fltk_webview library which can then be used in your application. Otherwise, you can just copy the necessary files into your project.

Bug fixes and contributions appreciated!