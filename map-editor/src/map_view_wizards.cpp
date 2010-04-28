
#include "map_view_wizards.h"

#include <QWizardPage>
#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

#include <constants.h>

#include "bombic/map.h"
#include "bombic/map_background.h"
#include "main_window.h"

#include "resource_handlers/resource_handler_functions.h"

/******************* MapSizeWizard ****************/

/** @details
 * Vytvori pruvodce zmenou rozmeru mapy,
 * vytvori jedinou stranku tohoto pruvodce a policka pro
 * zmenu rozmeru mapy.
 */
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

/** @details
 * Nastavi policka pro zmenu rozmeru mapy na aktualni rozmery mapy @p map.
 * @param map mapa, pro kterou chceme pruvodce
 */
void MapSizeWizard::setMap(BombicMap * map) {
	map_ = map;
	width_->setValue(map_->fieldsRect().width());
	height_->setValue(map_->fieldsRect().height());
}

/** @details
 * Vytvori kopii mapy s novymi rozmery.
 * Vyvola signal @c mapResized() s nove vytvorenou mapou.
 */
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
		map_ = 0;
	}
}

/******************* MapBackgroundWizard ****************/

/** @details
 * Vytvori pruvodce zmenou pozadi mapy,
 * vytvori jedinou stranku tohoto pruvodce a policko pro
 * zmenu pozadi mapy.
 */
MapBackgroundWizard::MapBackgroundWizard():
		map_(0) {
	QWizardPage * page = new QWizardPage;

	setOptions(QWizard::NoBackButtonOnStartPage);
	setWindowTitle(
		tr("Map background") + " | " + MAIN_WINDOW->windowTitle() );
	page->setTitle(tr("Map background"));
	page->setSubTitle(
		tr("Change map background - the background of fields"
			" and peripheral walls.\n"
			" If the new background takes bigger area, you have to remove"
			" objects in the taken area." ) );

	addPage(page);

	QGridLayout * layout = new QGridLayout(page);

	bgName_ = new QLineEdit(this);
	QLabel * label = new QLabel(tr("Map back&ground:"), this);
	label->setBuddy(bgName_);

	QPushButton * browseButton = new QPushButton(tr("&Browse ..."), this);
	connect(browseButton, SIGNAL(clicked()),
		this, SLOT(browse()) );

	layout->addWidget(label, 0, 0);
	layout->addWidget(bgName_, 0, 1);
	layout->addWidget(browseButton, 0, 2);
}

/** @details
 * Nastavi policko pro zmenu pozadi na aktualni pozadi mapy @p map.
 * @param map mapa, pro kterou chceme pruvodce
 */
void MapBackgroundWizard::setMap(BombicMap * map) {
	map_ = map;
	bgName_->setText(map->background()->name());
}

/** @details
 * Vytvori kopii mapy s novym pozadim.
 * Vyvola signal @c mapBackgroundChanged() s nove vytvorenou mapou.
 */
void MapBackgroundWizard::accept() {
	QWizard::accept();

	if(map_->background()->name() == bgName_->text()) {
		return;
	}

	BombicMap * newMap = map_->createCopy(bgName_->text());
	if(newMap) {
		emit mapBackgroundChanged(newMap);
		map_ = 0;
	}
}

/** @details
 * Necha uzivatele vybrat soubor s definici pozadi a nazev tohoto pozadi
 * vyplni do policka @c bgName_.
 */
void MapBackgroundWizard::browse() {
	QString filename = QFileDialog::getOpenFileName(
		MAIN_WINDOW, tr("Choose background"), "",
		tr("Bombic map background files")+" (*"XML_FILE_EXTENSION")" );
	if(!filename.isEmpty()) {
		bgName_->setText(
			ResourceHandlerFunctions::attrNameValueFromName(
				filename ) );
	}
}
