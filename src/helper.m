#import <Cocoa/Cocoa.h>

void helper_reparent(void *child_win, void *parent_win) {
    NSWindow *parent = (NSWindow *)parent;
    NSWindow *child = (NSWindow *)child;
    [parent setDelegate:(id)child];
    [child makeKeyAndOrderFront:nil];
}