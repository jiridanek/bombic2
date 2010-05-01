
#include <QDockWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>

#include "main_window.h"

#include "map_view.h"

SINGLETON_INIT(MainWindow);

/** @details
 * Vytvori hlavni okno aplikace, vsechny menu do panelu,
 * Vytvori polozky techto menu.
 */
MainWindow::MainWindow() {

	SINGLETON_CONSTRUCT;

	setWindowTitle(tr("Bombic Map Editor"));


	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	menus_[FileMenu] = menuBar()->addMenu(tr("&File"));
	menus_[MapMenu] = menuBar()->addMenu(tr("&Map"));
	menus_[LoadMenu] = menuBar()->addMenu(tr("&Load"));
	menus_[ViewMenu] = menuBar()->addMenu(tr("&View"));
	menus_[DocksMenu] =
		menus_[ViewMenu]->addMenu(tr("&Docked components"));
	menus_[AboutMenu] = menuBar()->addMenu(tr("&About"));


	addAction(NewMapAction, tr("&New map"),
		false, Qt::CTRL + Qt::Key_N, FileMenu);
	addAction(OpenMapAction, tr("&Open map"),
		false, Qt::CTRL + Qt::Key_O, FileMenu);
	addAction(SaveMapAction, tr("&Save map"),
		false, Qt::CTRL + Qt::Key_S, FileMenu);
	addAction(SaveMapAsAction, tr("Save map &as"),
		false, Qt::CTRL + Qt::SHIFT + Qt::Key_S, FileMenu);

	actions_[MapSizeAction] =
		menus_[MapMenu]->addAction(tr("Map &size"));
	actions_[MapBackgroundAction] =
		menus_[MapMenu]->addAction(tr("Map back&ground"));
	actions_[GeneratedBoxesAction] =
		menus_[MapMenu]->addAction(tr("Generated &boxes"));
	actions_[GeneratedCreaturesAction] =
		menus_[MapMenu]->addAction(tr("Generated &creatures"));
	actions_[GeneratedBonusesAction] =
		menus_[MapMenu]->addAction(tr("Generated bo&nuses"));

	addAction(GenerateObjectsAction, tr("&Generate random objects"),
		true, Qt::CTRL + Qt::Key_G, ViewMenu);

}
MainWindow::~MainWindow() {
	SINGLETON_DESTROY;
}

/**
 * @param paletteWidget paleta objektu
 */
void MainWindow::addMapObjectPalette(QWidget * paletteWidget) {
	addDock(tr("Map object palette"), paletteWidget,
		Qt::RightDockWidgetArea);
}

/**
 * @param mapViewWidget pohled na mapu
 */
void MainWindow::addMapView(QWidget * mapViewWidget) {
	setCentralWidget(mapViewWidget);
}

/**
 * @param mapFieldViewWidget detailni pohled na policko
 */
void MainWindow::addMapFieldView(QWidget * mapFieldViewWidget) {
	addDock(tr("Map field detail"), mapFieldViewWidget,
		Qt::BottomDockWidgetArea );
}

/**
 * @param workingObjectLabel popisek (znazorneni) pracovniho objektu
 */
void MainWindow::addWorkingObjectLabel(QWidget * workingObjectLabel) {
	QWidget * dockWidget = new QWidget(this);
	QLayout * dockLayout = new QVBoxLayout(dockWidget);
	dockLayout->addWidget(workingObjectLabel);

	addDock(tr("Working object"), dockWidget, Qt::BottomDockWidgetArea);
}

/**
 * @param zoomWidget nastroj na zoomovani mapy
 */
void MainWindow::addZoomWidget(QWidget * zoomWidget) {
	addDock(tr("Zoom the map"), zoomWidget, Qt::RightDockWidgetArea);
}

/**
 * @param name nazev (nadpis) dokovaneho widgetu
 * @param widget dokovany widget
 * @param area pozice doku
 */
void MainWindow::addDock(const QString & name, QWidget * widget,
		Qt::DockWidgetArea area) {
	QDockWidget * dock =
		new QDockWidget(name, this);
	dock->setWidget(widget);
	addDockWidget(area, dock);

	menus_[DocksMenu]->addAction(dock->toggleViewAction());
}

/**
 * @return Nove vytvorena polozka menu.
 */
QAction * MainWindow::addLoadAllSetsAction() {
	QAction * act = new QAction(tr("Load &all sets"), this);
	menus_[LoadMenu]->addAction(act);
	return act;
}

/**
 * @param setName nazev nacitane sady objektu
 * @return Nove vytvorena polozka menu.
 */
QAction * MainWindow::addLoadSetAction(const QString & setName) {
	QAction * act = new QAction(tr("Load set ")+setName, this);
	menus_[LoadMenu]->addAction(act);
	return act;
}


/**
 * @param action identifikator akce (polozky menu)
 * @param name nazev akce
 * @param checkable zda ma byt zaklikavaci
 * @param shortcut klavesova zkratka akce
 * @param menu identifikator menu, do ktereho chceme akci pridat
 */
void MainWindow::addAction(Action action, const QString & name,
		bool checkable, const QKeySequence & shortcut, Menu menu) {
	QAction * act = new QAction(name, this);
	act->setShortcut(shortcut);
	act->setCheckable(checkable);
	actions_[action] = act;
	menus_[menu]->addAction(act);
}

/**
 * @param menu identifikator menu, ktere chceme ziskat
 * @return Vytvorene menu.
 * @retval 0 Menu jeste nebylo vytvoreno.
 */
QMenu * MainWindow::menu(Menu menu) {
	if(!menus_.contains(menu)) {
		return 0;
	}
	return menus_[menu];
}

/**
 * @param action identifikator akce, kterou chceme ziskat
 * @return Vytvorena akce.
 * @retval 0 Akce jeste nebyla vytvorena.
 */
QAction * MainWindow::action(Action action) {
	if(!actions_.contains(action)) {
		return 0;
	}
	return actions_[action];
}

/** @details
 * Pokusi se zavrit editovanou mapu.
 * Vyjimku prijme (ukonci program)
 * pouze pokud se podari editovanou mapu zavrit.
 * @param event udalost, ktera handler vyvolala
 */
void MainWindow::closeEvent(QCloseEvent * event) {
	if(MAP_VIEW->closeMap()) {
		event->accept();
	} else {
		event->ignore();
	}
}
