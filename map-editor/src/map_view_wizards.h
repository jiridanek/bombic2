/** @file map_view_wizards.h
 * Pohled na mapu @c MapView provadi s mapou akce, pri kterych je treba
 * zmenit nejaky podstatny parametr mapy, objekty v mape okopirovat a
 * mapu vymenit. K tomuto ucelu slouzi @c MapSizeWizard, ktery meni
 * rozmery mapy a @c MapBackgroundWizard, ktery meni pozadi mapy.
 */

#ifndef MAP_VIEW_WIZARDS_H_GUARD_
#define MAP_VIEW_WIZARDS_H_GUARD_

#include <QWizard>

class BombicMap;
class QSpinBox;
class QLineEdit;

class MapSizeWizard: public QWizard {

	Q_OBJECT

	public:
		MapSizeWizard();

		void setMap(BombicMap * map);

	public slots:
		virtual void accept();

	signals:
		void mapResized(BombicMap * newMap);

	private:
		BombicMap * map_;
		QSpinBox * width_;
		QSpinBox * height_;
};

class MapBackgroundWizard: public QWizard {

	Q_OBJECT

	public:
		MapBackgroundWizard();

		void setMap(BombicMap * map);

	public slots:
		virtual void accept();

	signals:
		void mapBackgroundChanged(BombicMap * newMap);

	private slots:
		void browse();

	private:
		BombicMap * map_;
		QLineEdit * bgName_;
};

#endif
