/** @file map_background.h
 * Pozadi mapy (pozadi policek a obvodove zdi).
 */

#ifndef BOMBIC_MAP_BACKGROUND_H_GUARD_
#define BOMBIC_MAP_BACKGROUND_H_GUARD_

#include <QString>
#include <QPixmap>
#include <QColor>
#include <QHash>

class BombicWall;

/** Pozadi mapy.
 * Pozadi mapy definuje jake pozadi bude mit kazde policko
 * a jak budou vypadat obvodove zdi.
 * Obvodovych zdi je osm druhu (viz BombicMapBackground::WallSituation),
 * jsou to ctyri rohove zdi a ctyry druhy zdi,
 * ktere se opakuji po kazde strane.
 */
class BombicMapBackground {

	public:
		/// Konstrukce pozadi.
		BombicMapBackground(const QString & name,
				const QPixmap & texture);
		/// Destrukce pozadi.
		~BombicMapBackground();

		/// Pozice obvodove zdi.
		enum WallSituation {
			TopLeft, TopRight, BottomLeft, BottomRight,
			Top, Bottom, Left, Right
		};

		/// Obrazek pozadi policka.
		const QPixmap & texture();
		/// Barva pozadi okolo mapy.
		QColor ambientColor();

		/// Zed podle pozice.
		BombicWall * getWall(
				BombicMapBackground::WallSituation wallSituation);
		/// Nastavit zed podle pozice.
		void setWall(BombicWall * wall,
				BombicMapBackground::WallSituation wallSituation);

	private:
		/// Nazev pozadi.
		QString name_;
		/// Obrazek pozadi policka.
		QPixmap texture_;
		/// Obvodove zdi podle pozice.
		QHash<WallSituation, BombicWall *> walls_;

};

#endif
