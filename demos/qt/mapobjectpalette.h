
#ifndef MAPOBJECTPALETTE_H_GUARD
#define MAPOBJECTPALETTE_H_GUARD


#include <QWidget>
#include <QTabWidget>
#include <QSignalMapper>

class MapObjectPalette: public QWidget {

	Q_OBJECT

public:
	MapObjectPalette(QWidget * parent);

public slots:
	void loadObject();

private slots:
	void objectButtonToggled(QWidget * buttonWidget);

private:
	enum Pages {
		walls, boxes, creatures, floorobjects
	};

	void addPage(MapObjectPalette::Pages pageIndex,
		const QString & tabLabel);

	QTabWidget * tabs_;
	QSignalMapper * signalMapper_;
};


#endif
