//
//  OneRingWindowController.h
//  OneRing
//
//  Created by hongqn on 11/20/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>

@interface OneRingWindowController : NSObject {
    WebView *theWebView;
}

@property (assign) IBOutlet WebView *theWebView;

@end
