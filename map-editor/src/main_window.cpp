
#include <QDockWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>


#include "main_window.h"

SINGLETON_INIT(MainWindow);

MainWindow::MainWindow() {

	SINGLETON_CONSTRUCT;

	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	menus_[FileMenu] = menuBar()->addMenu(tr("&File"));
	menus_[MapMenu] = menuBar()->addMenu(tr("&Map"));
	menus_[EditMenu] = menuBar()->addMenu(tr("&Edit"));
	menus_[ViewMenu] = menuBar()->addMenu(tr("&View"));
	menus_[DocksMenu] =
		menus_[ViewMenu]->addMenu(tr("&Docked components"));
	menus_[HelpMenu] = menuBar()->addMenu(tr("&Help"));

	actions_[MapSizeAction] =
		menus_[MapMenu]->addAction(tr("Map &size"));
	actions_[MapBackgroundAction] =
		menus_[MapMenu]->addAction(tr("Map back&ground"));
	actions_[GeneratedBoxesAction] =
		menus_[MapMenu]->addAction(tr("Generated &boxes"));
	actions_[GeneratedCreaturesAction] =
		menus_[MapMenu]->addAction(tr("Generated &creatures"));

	addAction(GenerateObjectsAction, tr("&Generate random objects"),
		true, Qt::CTRL + Qt::Key_G, ViewMenu);

}
MainWindow::~MainWindow() {
	SINGLETON_DESTROY;
}

void MainWindow::addMapObjectPalette(QWidget * paletteWidget) {
	addDock(tr("Map object palette"), paletteWidget,
		Qt::RightDockWidgetArea);
}

void MainWindow::addMapView(QWidget * mapViewWidget) {
	setCentralWidget(mapViewWidget);
}

void MainWindow::addMapFieldView(QWidget * mapFieldViewWidget) {
	addDock(tr("Map field detail"), mapFieldViewWidget,
		Qt::BottomDockWidgetArea );
}

void MainWindow::addWorkingObjectLabel(QWidget * workingObjectLabel) {
	QWidget * dockWidget = new QWidget(this);
	QLayout * dockLayout = new QVBoxLayout(dockWidget);
	dockLayout->addWidget(workingObjectLabel);

	addDock(tr("Working object"), dockWidget, Qt::BottomDockWidgetArea);
}

void MainWindow::addZoomWidget(QWidget * zoomWidget) {
	addDock(tr("Zoom the map"), zoomWidget, Qt::RightDockWidgetArea);
}

void MainWindow::addDock(const QString & name, QWidget * widget,
		Qt::DockWidgetArea area) {
	QDockWidget * dock =
		new QDockWidget(name, this);
	dock->setWidget(widget);
	addDockWidget(area, dock);

	menus_[DocksMenu]->addAction(dock->toggleViewAction());
}

void MainWindow::addAction(Action action, const QString & name,
		bool checkable, const QKeySequence & shortcut, Menu menu) {
	QAction * act = new QAction(name, this);
	act->setShortcut(shortcut);
	act->setCheckable(checkable);
	actions_[action] = act;
	menus_[menu]->addAction(act);
}

QMenu * MainWindow::menu(Menu menu) {
	if(!menus_.contains(menu)) {
		return 0;
	}
	return menus_[menu];
}

QAction * MainWindow::action(Action action) {
	if(!actions_.contains(action)) {
		return 0;
	}
	return actions_[action];
}
