/** @file map_object.h
 * Obecny predek vsech objektu mapy BombicMapObject.
 */

#ifndef BOMBIC_MAP_OBJECT_H_GUARD_
#define BOMBIC_MAP_OBJECT_H_GUARD_

#include <QString>
#include <QPixmap>
#include <QPointF>
#include <QRect>
#include <QSize>

#include "map.h"

class QGraphicsItem;
class QGraphicsPixmapItem;

/** Objekt mapy.
 * Obecny abstraktni predek vsech objektu mapy.
 * Objekt mapy ma obrazek a rozmery v polickach, ktera zabira.
 * Ma dany typ, drzi graficky prvek zobrazitelny ve scene.
 * Definuje, ktere akce s nim lze provadet.
 */
class BombicMapObject {

	public:
		/// Inicializace objektu.
		BombicMapObject(const QString & name, const QPixmap & pixmap);
		/// Okopirovani dat objektu.
		BombicMapObject(BombicMapObject & srcObject);
		/// Destrukce objektu.
		~BombicMapObject();

		/// Vytvoreni kopie objektu (se stejnym typem).
		virtual BombicMapObject * createCopy() =0;

		/// Vyctovy typ objektu.
		enum Type {
			Floorobject, Wall, Box, Creature, Player, Bonus
		};

		/// Typ objektu.
		virtual BombicMapObject::Type type() =0;

		/// Graficky prvek zobrazitelny ve scene.
		virtual QGraphicsItem * graphicsItem();
		/// Umistit graficky prvek na konkretni pozici ve scene.
		virtual QGraphicsItem * situateGraphicsItem(
				const QPointF & position) =0;

		/// Nazev objektu.
		const QString & name();
		/// Obrazek objektu.
		const QPixmap & pixmap();
		/// Nahled obrazku objektu.
		const QPixmap & thumbnail();

		/// Obdelnik (v polickach), ktery objekt zabira.
		const QRect & rect();
		/// Policko (leve horni), na kterem je objekt umisten.
		BombicMap::Field field();
		/// Nastavit policko (leve horni), na kterem je objekt umisten.
		void setField(const BombicMap::Field & field);
		/// Velikost obdelniku (v polickach), kterou objekt zabira.
		QSize size();
		/// Pocet policek, o ktera objekt prevysuje svoji plochu.
		virtual int toplapping();

		/// Jestli muze byt objekt na policku s jinym objektem.
		virtual bool canBeWith(BombicMapObject * object);
		/// Jestli objekt blokuje generovani beden.
		virtual bool blocksBoxGenerating();
		/// Jestli objekt blokuje generovani priser.
		virtual bool blocksCreatureGenerating();
		/// Jestli objekt odblokovava generovani bonusu.
		virtual bool unblocksBonusGenerating();
		/// Jestli muze byt objekt premisten.
		virtual bool canBeDragged();
		/// Jestli muze byt objekt odstranen z mapy.
		virtual bool canBeRemoved();
		/// Jestli ma byt objekt na policku razen.
		virtual bool sortOnField();

	protected:
		/// Nazev.
		QString name_;
		/// Obrazek.
		QPixmap pixmap_;
		/// Nahled (zmensenina obrazku).
		QPixmap thumbnail_;
		/// Graficky prvek do sceny.
		QGraphicsPixmapItem * graphicsItem_;
		/// Velikost v polickach (kolik zabira policek).
		QRect rect_;
		/// Pocet presahujicich policek (na ose y).
		int toplapping_;
};

#endif
