/***************************************************************************
 *   Copyright (C) 2014-2015 by Cyril BALETAUD                                  *
 *   cyril.baletaud@gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef GENICAMCOMBOBOXEDITOR_H
#define GENICAMCOMBOBOXEDITOR_H

#include "genicameditor.h"
#include <QScopedPointer>

namespace GenICam {

class ComboboxEditorPrivate;
class ComboboxEditor : public EnumerationEditor
{
public:
    ComboboxEditor(const QString &name, QWidget * parent = 0);
    ~ComboboxEditor();

    void addItem(const QString& text, const qint64 value);
    void setCurrentIndex(int index);
    qint64 value() const;

private:
    const QScopedPointer<ComboboxEditorPrivate> d;
    Q_DISABLE_COPY(ComboboxEditor)

};
}

#endif // GENICAMCOMBOBOXEDITOR_H
