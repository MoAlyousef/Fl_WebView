#import <Cocoa/Cocoa.h>

void helper_reparent(void *child_win, void *parent_win) {
    NSWindow *parent = (NSWindow *)parent_win;
    NSWindow *child = (NSWindow *)child_win;
    NSView *this_view = [parent contentView];
    NSView *child_view = [[child contentView] retain];
    [child_view removeFromSuperview];
    [this_view addSubview:child_view positioned:NSWindowAbove relativeTo:nil];
    [child_view acceptsFirstResponder];
    [child close];
}