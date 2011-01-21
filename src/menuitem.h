#include <QObject>
#include <QAction>

class MenuItem : public QObject
{
Q_OBJECT

public:
	MenuItem(QAction *action, QObject* parent=0);
	~MenuItem();

	Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut);
	void setShortcut(const QString &shortcut);
	QString shortcut() const;

	Q_PROPERTY(QObject* action READ action);
	QObject* action();

private:
	QAction* _action;

};
