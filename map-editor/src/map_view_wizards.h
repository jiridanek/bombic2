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

/** Pruvodce zmenou rozmeru mapy.
 * Zmena rozmeru mapy je z implementacniho hlediska velmi slozita operace.
 * Pokud mapu zmensujeme, musime navic dbat na to, aby uzivatel neprisel
 * rozdelanou praci, kdyby mapu zmensil vice nez chce.
 * Z tohoto duvodu je zde pruvodce, kde uzivatel nastavi nove rozmery mapy,
 * je vytvorena nova mapa s novymi rozmery a objekty ze stare mapy jsou
 * do nove prekopirovany. Pokud dojde k chybe, uzivatel se o tom dozvi
 * a o starou mapu neprijde. Pokud projde vse hladce, mapy se pouze vymeni.
 */
class MapSizeWizard: public QWizard {

	Q_OBJECT

	public:
		/// Konstrukce pruvodce.
		MapSizeWizard();

		/// Nastavit mapu pro pruvodce.
		void setMap(BombicMap * map);

	public slots:
		/// Potvrdit zmenu mapy.
		virtual void accept();

	signals:
		/// Rozmer mapy zmenen.
		void mapResized(BombicMap * newMap);

	private:
		/// Mapa, pro kterou aktualne delame pruvodce.
		BombicMap * map_;
		/// Policko pro nastaveni sirky mapy.
		QSpinBox * width_;
		/// Policko pro nastaveni vysky mapy.
		QSpinBox * height_;
};

/** Pruvodce zmenou pozadi mapy.
 * Zmena pozadi mapy je velice podobna operace jako zmena rozmeru mapy,
 * protoze ruzna pozadi mohou mit ruzne velke obvodove zdi,
 * takze menime-li pozadi za takove, ktere ma nekterou obvodovou zed vetsi,
 * muze nastat stejny problem jako pri zmensovani mapy.
 * Z tohoto duvodu je zde pruvodce, kde uzivatel nastavi nove pozadi mapy,
 * je vytvorena nova mapa s novym pozadim a objekty ze stare mapy jsou
 * do nove prekopirovany. Pokud dojde k chybe, uzivatel se o tom dozvi
 * a o starou mapu neprijde. Pokud projde vse hladce, mapy se pouze vymeni.
 */
class MapBackgroundWizard: public QWizard {

	Q_OBJECT

	public:
		/// Konstrukce pruvodce.
		MapBackgroundWizard();

		/// Nastavit mapu pro pruvodce.
		void setMap(BombicMap * map);

	public slots:
		/// Potvrdit zmenu mapy.
		virtual void accept();

	signals:
		/// Pozadi mapy zmeneno.
		void mapBackgroundChanged(BombicMap * newMap);

	private slots:
		/// Prochazeni souboru na disku (vyber pozadi).
		void browse();

	private:
		/// Mapa, pro kterou aktualne delame pruvodce.
		BombicMap * map_;
		/// Policko pro nastaveni pozadi mapy.
		QLineEdit * bgName_;
};

#endif
