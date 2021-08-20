#ifndef __FL_WEBVIEW_H__
#define __FL_WEBVIEW_H__

#include <FL/Enumerations.H>
#include <FL/Fl_Double_Window.H>

using webview_t = void *;

class Fl_WebView : public Fl_Double_Window {
    webview_t wv = NULL;
#if !(defined(__APPLE__) || defined(_WIN32))
    long gdk_win_xid = 0;
    bool is_gnome = false;
#endif

public:
    Fl_WebView(int x, int y, int w, int h, const char* title = 0);
    void init();
    virtual void draw() override;
    void navigate(const char* addr);
};

#endif