#import <Cocoa/Cocoa.h>
#include <iostream>

class NoteFrame;  // 前向声明

@interface StatusBarMenuDelegate : NSObject
{
    NoteFrame *frame;
}
- (id)initWithFrame:(NoteFrame *)f;
- (void)toggleWindow:(id)sender;
- (void)exitApp:(id)sender;
@end

@implementation StatusBarMenuDelegate
- (id)initWithFrame:(NoteFrame *)f
{
    self = [super init];
    if (self) {
        frame = f;
    }
    return self;
}

- (void)toggleWindow:(id)sender
{
    // 通知 C++ 代码切换窗口可见性
    extern void NotifyToggleWindow();
    NotifyToggleWindow();
}

- (void)exitApp:(id)sender
{
    // 通知 C++ 代码退出应用
    extern void NotifyExitApp();
    NotifyExitApp();
}
@end

NSStatusBar *statusBar = nullptr;
NSStatusItem *statusItem = nullptr;
NSStatusBarButton *statusButton = nullptr;
StatusBarMenuDelegate *delegate = nullptr;

extern "C" void CreateStatusBarIcon(void *framePtr)
{
    @autoreleasepool {
        statusBar = [NSStatusBar systemStatusBar];
        statusItem = [statusBar statusItemWithLength:NSVariableStatusItemLength];
        [statusItem retain];  // 保持引用防止释放
        
        // 设置图标文本
        statusItem.button.title = @"⏰";
        statusItem.button.font = [NSFont systemFontOfSize:14];
        
        // 创建菜单
        NSMenu *menu = [[NSMenu alloc] initWithTitle:@""];
        [menu addItemWithTitle:@"Show/Hide" action:@selector(toggleWindow:) keyEquivalent:@""];
        [menu addItem:[NSMenuItem separatorItem]];
        [menu addItemWithTitle:@"Exit" action:@selector(exitApp:) keyEquivalent:@""];
        
        // 设置菜单的代理
        delegate = [[StatusBarMenuDelegate alloc] initWithFrame:(NoteFrame *)framePtr];
        for (NSMenuItem *item in [menu itemArray]) {
            [item setTarget:delegate];
        }
        
        statusItem.menu = menu;
        statusButton = statusItem.button;
        
        NSLog(@"Status bar icon created");
    }
}

extern "C" void UpdateStatusBarTitle(const char *title)
{
    @autoreleasepool {
        if (statusButton) {
            statusButton.title = [NSString stringWithUTF8String:title];
        }
    }
}

extern "C" void DestroyStatusBar()
{
    @autoreleasepool {
        if (statusItem) {
            [statusBar removeStatusItem:statusItem];
            [statusItem release];
            statusItem = nullptr;
        }
        statusBar = nullptr;
        statusButton = nullptr;
        if (delegate) {
            [delegate release];
            delegate = nullptr;
        }
    }
}
