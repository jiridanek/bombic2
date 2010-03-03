
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

	bool hasSelectedObject();
	const QPixmap & getSelectedObject();

public slots:
	void loadObject();

private slots:
	void objectButtonClicked(int objectPaletteIndex);

private:
	enum Pages {
		walls, boxes, creatures, floorobjects
	};

	void addPage(MapObjectPalette::Pages pageIndex,
		const QString & tabLabel);

	void selectObject(int objectIndex);
	void unselectObject();

	QTabWidget * tabs_;
	QList<QWidget *> tabsWidgets_;
	QSignalMapper * signalMapper_;
	QList<QPixmap> objectPalette_;

	#define MAP_OBJECT_PALETTE_INDEX_INVALID (-1)
	#define MAP_OBJECT_PALETTE_INDEX_IS_VALID(index) \
		(index > MAP_OBJECT_PALETTE_INDEX_INVALID)
	int selectedObjectIndex_;
};


#endif
