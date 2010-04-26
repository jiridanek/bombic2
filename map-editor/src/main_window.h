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


/**
 */
class MainWindow: public QMainWindow {

	Q_OBJECT

	SINGLETON_DEFINITION(MainWindow)

	public:
		MainWindow();
		~MainWindow();

		enum Menu {
			FileMenu, MapMenu, EditMenu,
			ViewMenu, DocksMenu, HelpMenu
		};

		enum Action {
			NewMapAction, OpenMapAction,
			SaveMapAction, SaveMapAsAction,
			MapSizeAction, MapBackgroundAction,
			GeneratedBoxesAction, GeneratedCreaturesAction,
			GenerateObjectsAction
		};

		QMenu * menu(Menu menu);
		QAction * action(Action action);

		void addMapObjectPalette(QWidget * paletteWidget);
		void addMapView(QWidget * mapViewWidget);
		void addMapFieldView(QWidget * mapFieldViewWidget);
		void addWorkingObjectLabel(QWidget * workingObjectLabel);
		void addZoomWidget(QWidget * zoomWidget);

	protected:
		virtual void closeEvent(QCloseEvent * event);

	private:
		void addDock(const QString & name, QWidget * widget,
				Qt::DockWidgetArea area);
		void addAction(Action action,
				const QString & name, bool checkable,
				const QKeySequence & shortcut, Menu menu);

		QHash<Menu, QMenu *> menus_;

		QHash<Action, QAction *> actions_;
};


#endif
