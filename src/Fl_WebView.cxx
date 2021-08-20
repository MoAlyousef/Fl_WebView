#define FL_INTERNALS

#include "Fl_WebView.H"
#include "webview.h"
#include <FL/Fl.H>
#include <FL/platform.H>
#include <stdexcept>


void Fl_WebView::navigate(const char* addr) { webview_navigate(wv, addr); }

#if !(defined(__APPLE__) || defined(_WIN32))
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <gdk/gdkx.h>
#include <glib.h>
#include <gtk/gtk.h>

int check(void*) {
    return Fl::check();
}

void x_init(Display* disp, Window child, Window parent) {
    XMoveWindow(disp, child, 0, 0);
    XReparentWindow(disp, child, parent, 0, 0);
    XFixesChangeSaveSet(disp, child, SetModeInsert, SaveSetRoot, SaveSetUnmap);
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

Fl_WebView::Fl_WebView(int x, int y, int w, int h, const char* title)
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

void Fl_WebView::init() {
    if (!shown())
        throw std::runtime_error("The window needs to be shown.");
    if (!is_gnome) {
        x_init(fl_display, gdk_win_xid, fl_xid(this));
    }
    this->flush();
    g_idle_add(check, NULL);
    Fl::add_idle(+[](void*) { gtk_main(); });
}

void Fl_WebView::draw() {
    if (is_gnome) {
        x_init(fl_display, gdk_win_xid, fl_xid(this));
        usleep(3e5);
    }
    if (wv)
        webview_set_size(wv, w(), h(), 0);
}

#else

Fl_WebView::Fl_WebView(int x, int y, int w, int h, const char* title)
    : Fl_Double_Window(x, y, w, h, title) {
    Fl_Double_Window::end();
}

void Fl_WebView::draw() {
    if (wv)
        webview_set_size(wv, w(), h(), 0);
}

#endif

#if defined(__APPLE__)

extern "C" void make_delegate(void* child, void* parent);

void Fl_WebView::init() {
    if (!shown())
        throw std::runtime_error("The window needs to be shown.");
    auto handle = fl_xid(this);
    wv = webview_create(false, (void*)handle);
    make_delegate((void*)webview_get_window(wv), (void*)handle);
    Fl::add_idle(webview_run, wv);
}

#elif defined(_WIN32)

void Fl_WebView::init() {
    if (!shown())
        throw std::runtime_error("The window needs to be shown.");
    auto temp = fl_xid(this);
    wv = webview_create(false, &temp);
}

#else
// X11 etc
#endif