/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#import <ewol/context/MacOs/AppDelegate.h>
#import <ewol/context/MacOs/OpenglView.h>

#include <ewol/debug.h>

@implementation MacOsAppDelegate
@synthesize window=_window;

- (BOOL)application:(MacOsAppDelegate *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	OpenGLView *view=[[OpenGLView alloc]initWithFrame:[[NSScreen mainScreen] bounds]];
	// Override point for customization after application launch.
	[self.window addSubview:view];
	[self.window makeKeyAndVisible];
	return YES;
}

- (void)applicationWillResignActive:(MacOsAppDelegate *)application {
	/*
	 Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	 Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
	 */
	EWOL_INFO("move windows in applicationWillResignActive");
}

- (void)applicationDidEnterBackground:(MacOsAppDelegate *)application {
	/*
	 Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
	 If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
	 */
	EWOL_INFO("move windows in applicationDidEnterBackground");
}

- (void)applicationWillEnterForeground:(MacOsAppDelegate *)application {
	/*
	 Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
	 */
	EWOL_INFO("move windows in applicationWillEnterForeground");
}

- (void)applicationDidBecomeActive:(MacOsAppDelegate *)application {
	/*
	 Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
	 */
	EWOL_INFO("move windows in applicationDidBecomeActive");
}

- (void)applicationWillTerminate:(MacOsAppDelegate *)application {
	/*
	 Called when the application is about to terminate.
	 Save data if appropriate.
	 See also applicationDidEnterBackground:.
	 */
	EWOL_INFO("move windows in applicationWillTerminate");
}

- (void)dealloc {
	[_window release];
	[super dealloc];
}


- (void)sendEvent:(NSEvent *)event {
	EWOL_WARNING(" EVENT ... ");
}


@end



