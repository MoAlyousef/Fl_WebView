#include "Fl_WebView.H"
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

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
