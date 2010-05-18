
#include <QDockWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QApplication>
#include <QMessageBox>

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

	connect(menus_[FileMenu]->addAction(tr("&Quit")),
		SIGNAL(triggered()),
		this, SLOT(close()) );

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


	menus_[AboutMenu]->addAction(tr("&Shortcuts and gestures"),
		this, SLOT(gesturesOverview()) );
	menus_[AboutMenu]->addAction(tr("&About editor"),
		this, SLOT(about()) );
	menus_[AboutMenu]->addAction(tr("About &Qt"),
		qApp, SLOT(aboutQt()) );

	QToolBar * fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(actions_[OpenMapAction]);
	fileToolBar->addAction(actions_[SaveMapAction]);

	QToolBar * viewToolBar = addToolBar(tr("View"));
	viewToolBar->addAction(actions_[GenerateObjectsAction]);

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

/** @details
 * Zobrazi informace o programu,
 */
void MainWindow::about() {
	QMessageBox::about(this, tr("About Bombic map editor"),
		tr("The <b>Bombic map editor</b> provides to create new "
		"or modify any existing map for Bombic game. "
		"To open new empty map select <em>New map</em>, "
		"for editing existing map select <em>Open map</em> and find "
		"its definition file on the disk." ) );
}

/** @details
 * souhrnne zobrazuje vsechny klavesove zkratky
 * a ovladaci gesta.
 */
void MainWindow::gesturesOverview() {
	QMessageBox::about(this,
		tr("Shortcuts and gestures | Bombic map editor"),
		tr("<h3>Overview of program shortcuts and gestures</h3>"
		"<h4>Gestures</h4>"
		"You can do the map editing by mouse. Following mouse gestures "
		"applyed on the map scene or fields detail view "
		" do the noted actions."
		"<table><tr>"
			"<th>Gesture</th>"
			"<th>Map scene</th><th>Field detail</th>"
		"</tr><tr>"
			"<td>Left click</td>"
			"<td>insert working object (if any)</td><td>nothing</td>"
		"</tr><tr>"
			"<td>Left double click</td>"
			"<td>delete object on top of field</td>"
			"<td>delete object under mouse pointer</td>"
		"</tr><tr>"
			"<td>Drag (left press and move)</td>"
			"<td>start moving object on top of field</td>"
			"<td>start moving object under mouse pointer</td>"
		"</tr><tr>"
			"<td>Drop (after drag the release)</td>"
			"<td>place dragged object on target field</td>"
			"<td>nothing</td>"
		"</tr></table>"
		"In the object palette are objects on buttons. "
		"Pressing the button you check it and set the object as working "
		"and you can insert it to the map scene. "
		"Pressing another object will be the old one unchecked "
		"and the pressed will be checked and set as working. "
		"To unselect working object you must uncheck the checked button. "
		"<h4>Shortcuts</h4>"
		"You can use menu to provide some action with map or editor. "
		"Some menu items has own shortcuts that do the same:"
		"<table><tr>"
			"<th>Action</th><th>Shortcut</th>"
		"</tr><tr>"
			"<td>New map</td><td>Ctrl + N</td>"
		"</tr><tr>"
			"<td>Open map</td><td>Ctrl + O</td>"
		"</tr><tr>"
			"<td>Save map</td><td>Ctrl + S</td>"
		"</tr><tr>"
			"<td>Save map as</td><td>Ctrl + Shift + S</td>"
		"</tr><tr>"
			"<td>Generate random objects</td><td>Ctrl + G</td>"
		"</tr></table>" ) );
}
