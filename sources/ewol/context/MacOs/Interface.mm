/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#import <Cocoa/Cocoa.h>
#include "ewol/context/MacOs/Interface.h"

#import <ewol/context/MacOs/OpenglView.h>
#import <ewol/context/MacOs/Windows.h>
#import <ewol/context/MacOs/AppDelegate.h>



int mm_main(int argc, const char *argv[]) {
	[NSAutoreleasePool new];
	
	[NSApplication sharedApplication];
	// set the quit policy and all stadard for Mac
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	
	// ---------------------------------------------------------------
	// -- basic menu bar creation :
	// ---------------------------------------------------------------
	// set the basic menu bar (stadard on Mac OSX)
	id menubar = [[NSMenu new] autorelease];
		//add an item
		id appMenuItem = [[NSMenuItem new] autorelease];
		// add the item to the menu bar: 
		[menubar addItem:appMenuItem];
	// set the main menu in the menu bar ...
	[NSApp setMainMenu:menubar];
	
	id appMenu = [[NSMenu new] autorelease];
	id appName = [[NSProcessInfo processInfo] processName];
	// create the label to qui the application :
	id quitTitle = [@"Quit " stringByAppendingString:appName];
	// create the item to quit the appllication with META+q at shortCut
	id quitMenuItem = [ [ [NSMenuItem alloc] initWithTitle:quitTitle
	                      action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
	// add the item to the menu:
	[appMenu addItem:quitMenuItem];
	// set the application menu to the main app menu ...
	[appMenuItem setSubmenu:appMenu];
	
	// ---------------------------------------------------------------
	// -- basic windows creation :
	// -----------------------		----------------------------------------
	// create a windows of size 800/600
#if 1
	id window = [ [ [EwolMainWindows alloc] initWithContentRect:NSMakeRect(0, 0, 800, 600)
				styleMask:(NSTitledWindowMask|NSMiniaturizableWindowMask|NSClosableWindowMask) backing:NSBackingStoreBuffered defer:NO]
				 autorelease];
	[window setAcceptsMouseMovedEvents:YES];
	//id window = [ [MacOsAppDelegate alloc] autorelease];
	
	 // set the windows at a specific position :
	[window cascadeTopLeftFromPoint:NSMakePoint(50,50)];
	// set the windows resizable
	[window setStyleMask:[window styleMask] | NSResizableWindowMask];
	// oposite : [window setStyleMask:[window styleMask] & ~NSResizableWindowMask];
	// set the title
	[window setTitle:appName];
	
	[window setAcceptsMouseMovedEvents:YES];
	// ???
	[window makeKeyAndOrderFront:nil];
	[NSApp activateIgnoringOtherApps:YES];
	
	NSRect window_frame = [window frame];
	
	OpenGLView* view=[[OpenGLView alloc]initWithFrame:window_frame]; //NSMakeRect(0, 0, 800, 600)];
    NSTrackingArea *track = [[NSTrackingArea alloc] initWithRect:window_frame options: NSTrackingMouseMoved | NSTrackingActiveWhenFirstResponder | NSTrackingActiveInKeyWindow
														   owner:window userInfo:nil];
    [view addTrackingArea:track];
	[window setContentView:view];
	[view setAutoresizesSubviews:YES];
	
	// Override point for customization after application launch.
	//[window addSubview:view];
	//[window addChildWindow:view];
	//[window makeKeyAndVisible];
	
	[window setDelegate:view];
#else
    @autoreleasepool
    {
        const ProcessSerialNumber psn = { 0, kCurrentProcess };
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
        SetFrontProcess(&psn);
		
        [MyApplication sharedApplication];
        [NSApp setDelegate: NSApp];
		
        [NSApp run];
    }
#endif
	// return no error
	return 0;
}

int mm_run(void) {
	[NSApp run];
	// return no error
	return 0;
}
