
#ifndef MAPOBJECTPALETTE_H_GUARD
#define MAPOBJECTPALETTE_H_GUARD


#include <QWidget>
#include <QTabWidget>
#include <QSignalMapper>
#include <QList>
#include <QPixmap>

class MapObjectPalette: public QWidget {

	Q_OBJECT

public:
	MapObjectPalette(QWidget * parent);

public slots:
	void loadObject();

private slots:
	void objectButtonToggled(int objectPaletteIndex);

private:
	enum Pages {
		walls, boxes, creatures, floorobjects
	};

	void addPage(MapObjectPalette::Pages pageIndex,
		const QString & tabLabel);

	QTabWidget * tabs_;
	QList<QWidget *> tabsWidgets_;
	QSignalMapper * signalMapper_;
	QList<QPixmap> objectPalette_;
};


#endif
