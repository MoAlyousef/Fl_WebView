#define FL_INTERNALS

#include "webview.h"
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/platform.H>
#include <stdexcept>

#if defined(__linux__)
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <glib.h>

int check(void *) {
  return Fl::check();
}

void x_init(Display *disp, Window child, Window parent) {
  XMoveWindow(disp, child, 0, 0);

  XReparentWindow(disp, child, parent, 0, 0);

  XFixesChangeSaveSet(disp, child, SetModeInsert, SaveSetRoot,
                      SaveSetUnmap);

  XEvent client_event;
  XWindowAttributes childAttributes;
  XWindowAttributes parentAttributes;
  XGetWindowAttributes(disp, child, &childAttributes);
  XGetWindowAttributes(disp, parent, &parentAttributes);

  client_event.type = ConfigureNotify;
  client_event.xconfigure.send_event = True;
  client_event.xconfigure.display = disp;
  client_event.xconfigure.event = child;
  client_event.xconfigure.window = child;
  client_event.xconfigure.width = childAttributes.width;
  client_event.xconfigure.height = childAttributes.height;
  client_event.xconfigure.border_width = 0;
  client_event.xconfigure.above = None;
  client_event.xconfigure.override_redirect = True;

  XSendEvent(disp, child, False, StructureNotifyMask, &client_event);
}

class Fl_WebView : public Fl_Double_Window {
  webview_t wv = NULL;
  long gdk_win_xid = 0;
  bool is_gnome = false;

public:
  Fl_WebView(int x, int y, int w, int h, const char *title = 0)
      : Fl_Double_Window(x, y, w, h, title) {
    wv = webview_create(false, NULL);
    auto wv_gtk_win = webview_get_window(wv);
    auto gdk_win = gtk_widget_get_window(GTK_WIDGET(wv_gtk_win));
    gdk_win_xid = GDK_WINDOW_XID(gdk_win);
    Fl_Double_Window::end();
    // doesn't have gnome-shell
    // TODO: Check WM_NAME thru Xlib
    is_gnome = !system("gnome-shell --help > /dev/null");
  }
  void init() {
    if (!shown())
      throw std::runtime_error("The window needs to be shown.");
    if (!is_gnome) {
      x_init(fl_display, gdk_win_xid, fl_xid(this));
    }
    this->flush();
    g_idle_add(check, NULL);
    Fl::add_idle(+[](void *) { gtk_main(); });
  }
  virtual void draw() override {
    if (is_gnome) {
      x_init(fl_display, gdk_win_xid, fl_xid(this));
      usleep(3e5);
    }
    webview_set_size(wv, w(), h(), 0);
   }
  void navigate(const char *addr) { webview_navigate(wv, addr); }
};

#elif defined(__APPLE__)

extern "C" void make_delegate(void *child, void *parent);

class Fl_WebView : public Fl_Double_Window {
  webview_t wv = NULL;

public:
  Fl_WebView(int x, int y, int w, int h, const char *title = 0)
      : Fl_Double_Window(x, y, w, h, title) {
    Fl_Double_Window::end();
  }
  void init() {
    if (!shown())
      throw std::runtime_error("The window needs to be shown.");
    auto handle = fl_xid(this);
    wv = webview_create(false, (void *)handle);
    make_delegate((void *)webview_get_window(wv), (void *)handle);
    Fl::add_idle(webview_run, wv);
  }
  virtual void draw() override { webview_set_size(wv, w(), h(), 0); }
  void navigate(const char *addr) { webview_navigate(wv, addr); }
};

#elif defined(_WIN32)
#include <windows.h>

class Fl_WebView : public Fl_Double_Window {
  webview_t wv = NULL;

public:
  Fl_WebView(int x, int y, int w, int h, const char *title = 0)
      : Fl_Double_Window(x, y, w, h, title) {
    Fl_Double_Window::end();
  }
  void init() {
    if (!shown())
      throw std::runtime_error("The window needs to be shown.");
    wv = webview_create(false, (void *)fl_xid(this));
  }
  virtual void draw() override { webview_set_size(wv, w(), h(), 0); }
  void navigate(const char *addr) { webview_navigate(wv, addr); }
};
#else
#error "Unsupported platform"
#endif

int main(int argc, char **argv) {
  auto main_win = new Fl_Double_Window(800, 600);
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
