/** @file main_window.h
 * Hlavni okno editoru map MainWindow.
 */

#ifndef MAIN_WINDOW_H_GUARD_
#define MAIN_WINDOW_H_GUARD_

#include <QMainWindow>
#include <QDockWidget>
#include <QString>
#include <QHash>

#include <singleton.h>

class QWidget;
class QMenu;
class QAction;
class QCloseEvent;

/// Zkratka, pro pristup k singletonu MainWindow.
#define MAIN_WINDOW SINGLETON_SHORTCUT(MainWindow)

/** Hlavni okno editoru map.
 * Spravuje hlavni menu programu a dokovatelne widgety.
 */
class MainWindow: public QMainWindow {

	Q_OBJECT

	SINGLETON_DEFINITION(MainWindow)

	public:
		/// Konstrukce okna.
		MainWindow();
		/// Destrukce okna.
		~MainWindow();

		/// Identifikatory jednotlivych menu.
		enum Menu {
			FileMenu, MapMenu, LoadMenu,
			ViewMenu, DocksMenu, AboutMenu
		};

		/// Identifikatory jednotlivych akci menu.
		enum Action {
			NewMapAction, OpenMapAction,
			SaveMapAction, SaveMapAsAction,
			MapSizeAction, MapBackgroundAction,
			GeneratedBoxesAction, GeneratedCreaturesAction,
			GeneratedBonusesAction,
			GenerateObjectsAction
		};

		/// Vytvorene menu podle identifikatoru.
		QMenu * menu(Menu menu);
		/// Vytvorena akce podle identifikatoru.
		QAction * action(Action action);

		/// Pridat do okna paletu objektu.
		void addMapObjectPalette(QWidget * paletteWidget);
		/// Pridat do okna pohled na mapu.
		void addMapView(QWidget * mapViewWidget);
		/// Pridat do okna detailni pohled na policko.
		void addMapFieldView(QWidget * mapFieldViewWidget);
		/// Pridat do okna popisek pracovniho objektu.
		void addWorkingObjectLabel(QWidget * workingObjectLabel);
		/// Pridat do okna zoomovaci nastroj.
		void addZoomWidget(QWidget * zoomWidget);

		/// Pridat polozku menu pro nacteni vsech sad.
		QAction * addLoadAllSetsAction();
		/// Pridat polozku menu pro nacteni sady.
		QAction * addLoadSetAction(const QString & setName);

	public slots:
		/// Zobrazit přehled gest a klávesových zkratek.
		void gesturesOverview();
		/// Zobrazit informace o programu.
		void about();

	protected:
		/// Handler zavreni okna.
		virtual void closeEvent(QCloseEvent * event);

	private:
		/// Pridat do okna dokovatelny widget.
		void addDock(const QString & name, QWidget * widget,
				Qt::DockWidgetArea area);
		/// Pridat do menu akci jako polozku.
		void addAction(Action action,
				const QString & name, bool checkable,
				const QKeySequence & shortcut, Menu menu);

		/// Vytvorene menu.
		QHash<Menu, QMenu *> menus_;
		/// Vytvorene akce menu.
		QHash<Action, QAction *> actions_;
};


#endif
