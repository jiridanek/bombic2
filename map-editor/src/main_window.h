/** @file main_window.h
 * Hlavni okno editoru map MainWindow.
 */

#ifndef MAIN_WINDOW_H_GUARD_
#define MAIN_WINDOW_H_GUARD_

#include <QMainWindow>
#include <QDockWidget>

#include <singleton.h>

class QWidget;
class QString;

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

		void addMapObjectPalette(QWidget * paletteWidget);
		void addMapView(QWidget * mapViewWidget);
		void addMapFieldView(QWidget * mapFieldViewWidget);
		void addWorkingObjectLabel(QWidget * workingObjectLabel);
		void addZoomWidget(QWidget * zoomWidget);

	private:
};


#endif
