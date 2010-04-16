/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "flowlayout.h"

/**
 * @param parent nadrazeny widget
 * @param margin prostor vymezeny okolo layoutu
 * @param spacing prostor vymezeny okolo kazdeho prvku
 */
FlowLayout::FlowLayout(QWidget *parent, int margin, int spacing)
    : QLayout(parent)
{
    setMargin(margin);
    setSpacing(spacing);
}

/**
 * @param spacing prostor vymezeny okolo kazdeho prvku
 */
FlowLayout::FlowLayout(int spacing)
{
    setSpacing(spacing);
}

/** @details
 * Odalokuje prvky layoutu.
 */
FlowLayout::~FlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

/** @details
 * Prida prvek do layoutu, stava se jeho vlastnikem.
 * Prvek bude odalokovan spolecne s layoutem.
 * Zapricini prekresleni layoutu.
 * @param item prvek, ktery ma byt pridan
 */
void FlowLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
    update();
}

/**
 * @return Pocet prvku v layoutu.
 */
int FlowLayout::count() const
{
    return itemList.size();
}

/**
 * @param index platny index, musi byt mezi nulou a count()
 * @return Prvek na pozici @p index.
 */
QLayoutItem *FlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}

/** @details
 * Prvek na pozici @p index odstrani z layoutu a vrati jej.
 * Vlastnictvi prvku prechazi na volajiciho, prvek jiz nebude
 * s layoutem odalokovan.
 * @return Prvek na pozici @p index.
 */
QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    else
        return 0;
}

/**
 * @retval 0 Vzdy.
 */
Qt::Orientations FlowLayout::expandingDirections() const
{
    return Qt::Horizontal;
}

/**
 * @retval true Vzdy.
 */
bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

/** @details
 * Provede rozlozeni pro sirsku @p width.
 * Spocita vysku takoveho rozlozeni.
 * @param width pozadovana sirska
 * @return Vyska odpovidajici rozlozeni pri zadane sirsce.
 */
int FlowLayout::heightForWidth(int width) const
{
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}

/** @details
 * Nastavi geometrii na @p rect,
 * Pro zadany obdelnik @p rect provede rozlozeni prvku.
 * @param rect pozadovany obdelnik
 */
void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

/** @details
 * Pro aktualni sirsku spocita korespondujici vysku.
 * @return Idelani velikost odpovidajici aktualni sirce.
 */
QSize FlowLayout::sizeHint() const
{
	int w = geometry().width();
	// hint podle aktualni sirky
	QSize size(w, heightForWidth(w));
	size += QSize(2*margin(), 2*margin());
	return size;
}

/** @details
 * Spocita minimalni velikost nejvetsiho prvku.
 * @return Nejmensi mozna velikost.
 */
QSize FlowLayout::minimumSize() const
{
	QSize size(0, 0);
	QLayoutItem *item;
	foreach (item, itemList)
		size = size.expandedTo(item->minimumSize());

	size += QSize(2*margin(), 2*margin());
	return size;
}

/** @details
 * Spocita rozlozeni prvku layoutu do obdelniku @p rect.
 * Pokud je spusten pouze v testovacim rezimu ( @p testOnly je true)
 * Prvky ve skutecnosti nebudou premisteny.
 * @param rect obdelnik, do ktereho se ma layout vejit
 * @param testOnly rezim,
 *                 @c true - polohy jsou pouze spocitany,
 *                 @c false - prvky jsou i rozmisteny
 * @return Vyska prave provedeneho rozlozeni.
 */
int FlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int x = rect.x();
    int y = rect.y();
    int lineHeight = 0;

    QLayoutItem *item;
    foreach (item, itemList) {
        int nextX = x + item->sizeHint().width() + spacing();
        if (nextX - spacing() > rect.right() && lineHeight > 0) {
            x = rect.x();
            y = y + lineHeight + spacing();
            nextX = x + item->sizeHint().width() + spacing();
            lineHeight = 0;
        }

        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y();
}
