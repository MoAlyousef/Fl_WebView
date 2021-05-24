#import <Cocoa/Cocoa.h>

typedef void *webview_t;

extern void reparent(void *wv, void *win) {
    NSWindow *parent = (NSWindow *)win;
    NSWindow *child = (NSWindow *)wv;
    NSView *this_view = [parent contentView];
    NSView *child_view = [[child contentView] retain];
    [child_view removeFromSuperview];
    [this_view addSubview:child_view positioned:1 relativeTo:nil];
    [child_view acceptsFirstResponder];
    [child close];
}