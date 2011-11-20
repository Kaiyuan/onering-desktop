//
//  OneRingTests.m
//  OneRingTests
//
//  Created by hongqn on 11/20/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "OneRingTests.h"
#include <onering.h>

@implementation OneRingTests

- (void)setUp
{
    [super setUp];
    
    // Set-up code here.
}

- (void)tearDown
{
    // Tear-down code here.
    
    [super tearDown];
}

- (void)testVersion
{
    const char* version = onering_version();
    STAssertEquals(strcmp(version, "WebKitCore 1.0.0"), 0, @"version number mismatch");
}

@end
