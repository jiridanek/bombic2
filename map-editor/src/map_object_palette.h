/** @file map_object_palette.h
 */

#ifndef MAP_OBJECT_PALETTE_H_GUARD_
#define MAP_OBJECT_PALETTE_H_GUARD_

#include <QWidget>
#include <QString>
#include <QHash>
#include <QPoint>
#include <singleton.h>

#define MAP_OBJECT_PALETTE SINGLETON_SHORTCUT(MapObjectPalette)

class QTabWidget;
class QLayout;
class QSignalMapper;
class QPushButton;

class BombicMapObject;

class MapObjectPalette: public QWidget {

	Q_OBJECT

	SINGLETON_DEFINITION(MapObjectPalette)

	public:
		MapObjectPalette(QWidget * parent = 0);
		~MapObjectPalette();

		BombicMapObject * getObject(const QString & objectName);

		void addObject(BombicMapObject * object);

	signals:
		void objectUnselected();
		void objectSelected(BombicMapObject *);

	public slots:
		void unselectObject();

	private slots:
		void objectButtonClicked(int objectPaletteIndex);

	private:
		enum Page {
			wallPage, boxPage, creaturePage, floorobjectPage
		};

		void tabsConstruct_();
		void tabsAddPage_(MapObjectPalette::Page pageIndex,
			const QString & tabLabel);

		void selectObject(int objectPaletteIndex);

		QPushButton * getObjectButton(int objectPaletteIndex);

		typedef struct {
			QTabWidget * widget;

			QHash<Page, QLayout *> layouts;
		} Tabs;

		Tabs tabs_;
		QSignalMapper * signalMapper_;

		QList<BombicMapObject *> objectPalette_;
		QHash<QString, BombicMapObject *> objectsByName_;

		int selectedObjectIndex_;

};

#endif
