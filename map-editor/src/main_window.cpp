
#include <QDockWidget>
#include <QVBoxLayout>

#include "main_window.h"

SINGLETON_INIT(MainWindow);

MainWindow::MainWindow() {

	SINGLETON_CONSTRUCT;

	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

}
MainWindow::~MainWindow() {
	SINGLETON_DESTROY;
}

void MainWindow::addMapObjectPalette(QWidget * paletteWidget) {
	QDockWidget * dock =
		new QDockWidget(tr("Map object palette"), this);
	dock->setWidget(paletteWidget);
	addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::addMapView(QWidget * mapViewWidget) {
	setCentralWidget(mapViewWidget);
}

void MainWindow::addMapFieldView(QWidget * mapFieldViewWidget) {
	QDockWidget * dock =
		new QDockWidget(tr("Map field detail"), this);
	dock->setWidget(mapFieldViewWidget);
	addDockWidget(Qt::TopDockWidgetArea, dock);
}

void MainWindow::addWorkingObjectLabel(QWidget * workingObjectLabel) {
	QDockWidget * dock =
		new QDockWidget(tr("Working object"), this);
	QWidget * dockWidget = new QWidget(dock);
	dock->setWidget(dockWidget);
	QLayout * dockLayout = new QVBoxLayout(dockWidget);
	dockLayout->addWidget(workingObjectLabel);

	addDockWidget(Qt::BottomDockWidgetArea, dock);
}

void MainWindow::addZoomWidget(QWidget * zoomWidget) {
	QDockWidget * dock =
		new QDockWidget(tr("Zoom the map"), this);
	dock->setWidget(zoomWidget);
	addDockWidget(Qt::BottomDockWidgetArea, dock);
}


