/** @file map_background.h
 */

#ifndef BOMBIC_MAP_BACKGROUND_H_GUARD_
#define BOMBIC_MAP_BACKGROUND_H_GUARD_

#include <QString>
#include <QPixmap>
#include <QColor>
#include <QHash>

class BombicWall;

class BombicMapBackground {

	public:
		BombicMapBackground(const QString & name,
				const QPixmap & texture);
		~BombicMapBackground();

		enum WallSituation {
			TopLeft, TopRight, BottomLeft, BottomRight,
			Top, Bottom, Left, Right
		};

		const QPixmap & texture();
		QColor ambientColor();

		BombicWall * getWall(
				BombicMapBackground::WallSituation wallSituation);
		void setWall(BombicWall * wall,
				BombicMapBackground::WallSituation wallSituation);

	private:
		QString name_;
		QPixmap texture_;

		QHash<WallSituation, BombicWall *> walls_;

};

#endif
