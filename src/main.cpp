#define FL_INTERNALS

#include "webview.h"
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/platform.H>
#include <stdexcept>

#if defined(__linux__)
#include <X11/Xlib.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

class Fl_WebView : public Fl_Window {
  webview_t wv = NULL;
  long gdk_win_xid = 0;

public:
  Fl_WebView(int x, int y, int w, int h, const char *title = 0)
      : Fl_Window(x, y, w, h, title) {
    wv = webview_create(false, NULL);
    auto wv_gtk_win = webview_get_window(wv);
    auto gdk_win = gtk_widget_get_window(GTK_WIDGET(wv_gtk_win));
    gdk_win_xid = GDK_WINDOW_XID(gdk_win);
    Fl_Window::end();
  }
  void init() {
    if (!shown())
      throw std::runtime_error("The window needs to be shown.");
    auto wv_win_xid = fl_xid(this);
    auto main_win_xid = fl_xid(this->top_window());
    XMapWindow(fl_display, gdk_win_xid);
    XReparentWindow(fl_display, gdk_win_xid, main_win_xid, x(), y());
    XFlush(fl_display);
    Fl::add_idle(+[](void *) { gtk_main_iteration(); });
  }
  virtual void draw() override { webview_set_size(wv, w(), h(), 0); }
  void navigate(const char *addr) { webview_navigate(wv, addr); }
};

#elif defined(__APPLE__)
#include <CoreGraphics/CoreGraphics.h>
#include <objc/objc-runtime.h>

extern "C" void helper_reparent(void *child, void *parent);

class Fl_WebView : public Fl_Window {
  webview_t wv = NULL;

public:
  Fl_WebView(int x, int y, int w, int h, const char *title = 0)
      : Fl_Window(x, y, w, h, title) {
    wv = webview_create(false, NULL);
    Fl_Window::end();
  }
  void init() {
    if (!shown())
      throw std::runtime_error("The window needs to be shown.");
    webview_set_size(wv, w(), h(), 0);
    helper_reparent((void *)webview_get_window(wv), (void *)fl_xid(this));
    Fl::add_idle(webview_run, wv);
  }
  virtual void draw() override { webview_set_size(wv, w(), h(), 0); }
  void navigate(const char *addr) { webview_navigate(wv, addr); }
};

#elif defined(_WIN32)
#include <windows.h>

class Fl_WebView : public Fl_Window {
  webview_t wv = NULL;

public:
  Fl_WebView(int x, int y, int w, int h, const char *title = 0)
      : Fl_Window(x, y, w, h, title) {
    Fl_Window::end();
  }
  void init() {
    if (!shown())
      throw std::runtime_error("The window needs to be shown.");
    wv = webview_create(false, (HWND *)fl_xid(this));
  }
  virtual void draw() override { webview_set_size(wv, w(), h(), 0); }
  void navigate(const char *addr) { webview_navigate(wv, addr); }
};
#else
#error "Unsupported platform"
#endif

int main(int argc, char **argv) {
  auto main_win = new Fl_Window(800, 600);
  auto wv_win = new Fl_WebView(5, 5, 790, 590);
  main_win->end();
  main_win->resizable(main_win);
  main_win->show();
  main_win->callback(+[](Fl_Widget *, void *data) {
    if (Fl::event() == FL_CLOSE)
      std::exit(0);
  });

  wv_win->init();
  wv_win->navigate("https://google.com");

  return Fl::run();
}
