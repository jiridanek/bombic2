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

#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

/** Plynuly layout.
 * Prvky tohoto layoutu se rozmistuji plynule v rade (horizontalne),
 * se zalomenim.
 */  
class FlowLayout : public QLayout
{
public:
	/// Konstruuje layout s nadrazenym widgetem.
    FlowLayout(QWidget *parent, int margin = 0, int spacing = -1);
	/// Konstuuje layout bez nadrazeneho widgetu.
    FlowLayout(int spacing = -1);
	/// Destruuje layout.
    ~FlowLayout();

	/// Prida prvek na konec layoutu.
    void addItem(QLayoutItem *item);
	/// Ve kterych smerech se ma layout roztahovat.
    Qt::Orientations expandingDirections() const;
	/// Jestli ma pro urcitou sirku definovanou vysku.
    bool hasHeightForWidth() const;
	/// Vyska pro urcitou sirku.
    int heightForWidth(int) const;
	/// Pocet prvku v layoutu.
    int count() const;
	/// Prvek layoutu na pozici @p index.
    QLayoutItem *itemAt(int index) const;
	/// Odstranit a vratit prvek na pozici @p index.
    QLayoutItem *takeAt(int index);
	/// Minimalni velikost layoutu.
    QSize minimumSize() const;
	/// Nastavi geometrii layoutu.
    void setGeometry(const QRect &rect);
	/// Hint velikosti.
    QSize sizeHint() const;
	
private:
	/// Provest rozlozeni prvku.
    int doLayout(const QRect &rect, bool testOnly) const;
	/// Seznamm prvku layoutu.
    QList<QLayoutItem *> itemList;

};

#endif
