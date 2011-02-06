#include <Cocoa/Cocoa.h>
#include <QDebug>
#include "dockiconclick.h"

@interface DockIconClickEventHandler : NSObject
{
@public
	DockIconClickMonitor* monitor;
}
- (void)handle:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent;
@end

@implementation DockIconClickEventHandler
- (void)handle:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent {
	if (monitor)
		monitor->emitEvent();
}
@end

class DockIconClickMonitor::Private
{
	public:
		DockIconClickEventHandler* handler;
};

DockIconClickMonitor::DockIconClickMonitor()
{
	d = new DockIconClickMonitor::Private();
	d->handler = [[DockIconClickEventHandler alloc] init];
	d->handler->monitor = this;

	[[NSAppleEventManager sharedAppleEventManager]
	 setEventHandler:d->handler
	 andSelector:@selector(handle:withReplyEvent:)
	 forEventClass:kCoreEventClass
	 andEventID:kAEReopenApplication];
}

DockIconClickMonitor::~DockIconClickMonitor()
{
	delete d;
}

void DockIconClickMonitor::emitEvent()
{
	emit dockIconClicked();
}
