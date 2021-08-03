#import <Cocoa/Cocoa.h>

void make_delegate(void *child_win, void *parent_win) {
    NSWindow *parent = (NSWindow *)parent_win;
    NSWindow *child = (NSWindow *)child_win;
    [parent setDelegate:(id)child];
    [child makeKeyAndOrderFront:nil];
}