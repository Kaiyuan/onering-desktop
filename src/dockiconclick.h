#ifndef ONERING_DOCKICONCLICK_H
#define ONERING_DOCKICONCLICK_H

#include <QObject>

class DockIconClickMonitor : public QObject
{
	Q_OBJECT

	public:
		DockIconClickMonitor();
		~DockIconClickMonitor();

		void emitEvent();

	signals:
		void dockIconClicked();

	private:
		class Private;
		Private* d;
};

#endif
