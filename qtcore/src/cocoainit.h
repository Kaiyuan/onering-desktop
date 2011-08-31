#ifndef ONERING_COCOAINIT_H
#define ONERING_COCOAINIT_H

class CocoaInitializer
{
	public:
		CocoaInitializer();
		~CocoaInitializer();

	private:
		class Private;
		Private* d;
};

#endif
