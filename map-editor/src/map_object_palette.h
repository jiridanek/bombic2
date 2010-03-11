/** @file map_object_palette.h
 */

#ifndef MAP_OBJECT_PALETTE_H_GUARD_
#define MAP_OBJECT_PALETTE_H_GUARD_

#include <QWidget>
#include <QString>
#include <QList>
#include <QPoint>
#include <singleton.h>

#define MAP_OBJECT_PALETTE SINGLETON_SHORTCUT(MapObjectPalette)

class QTabWidget;
class QLayout;
class QSignalMapper;

class BombicMapObject;

class MapObjectPalette: public QWidget {

	Q_OBJECT

	SINGLETON_DEFINITION(MapObjectPalette)

	public:
		MapObjectPalette(QWidget * parent = 0);
		~MapObjectPalette();


	private:
		enum Page {
			wallPage, boxPage, creaturePage, floorobjectPage
		};

		void tabsConstruct_();
		void tabsAddPage_(MapObjectPalette::Page pageIndex,
			const QString & tabLabel);

// 		void selectObject(int objectIndex);
// 		void unselectObject();

		typedef struct {
			QTabWidget * widget;

			QList< QLayout * > layouts;
		} Tabs;

		Tabs tabs_;
		QSignalMapper * signalMapper_;
// 		QList<QPixmap> objectPalette_;

// 		int selectedObjectIndex_;

};

#endif
