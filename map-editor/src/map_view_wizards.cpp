
#include "map_view_wizards.h"

#include <QWizardPage>
#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QSize>

#include "bombic/map.h"
#include "main_window.h"



/******************* MapSizeWizard ****************/

MapSizeWizard::MapSizeWizard():
		map_(0) {
	QWizardPage * page = new QWizardPage;

	setOptions(QWizard::NoBackButtonOnStartPage);
	setWindowTitle(
		tr("Map size") + " | " + MAIN_WINDOW->windowTitle() );
	page->setTitle(tr("Map size"));
	page->setSubTitle(
		tr("Change map width or height.\n"
			" If you cut down any proportion, you have to remove"
			" objects in the cut area." ) );

	addPage(page);

	QGridLayout * layout = new QGridLayout(page);

	width_ = new QSpinBox(this);
	width_->setMinimum(3);
	QLabel * wLabel = new QLabel(tr("Map &width:"), this);
	wLabel->setBuddy(width_);

	height_ = new QSpinBox(this);
	height_->setMinimum(4);
	QLabel * hLabel = new QLabel(tr("Map &height:"), this);
	hLabel->setBuddy(height_);

	layout->addWidget(wLabel, 0, 0);
	layout->addWidget(width_, 0, 1);
	layout->addWidget(hLabel, 1, 0);
	layout->addWidget(height_, 1, 1);

}

void MapSizeWizard::setMap(BombicMap * map) {
	map_ = map;
	width_->setValue(map_->fieldsRect().width());
	height_->setValue(map_->fieldsRect().height());
}

void MapSizeWizard::accept() {
	QWizard::accept();

	QSize newSize(width_->value(), height_->value());

	if(map_->fieldsRect().size() == newSize) {
		return;
	}

	BombicMap * newMap = map_->createCopy(
		newSize.width(), newSize.height());
	if(newMap) {
		emit mapResized(newMap);
	}
}
