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

#ifndef INTEGER_P_H
#define INTEGER_P_H

#include "inode_p.h"

namespace JgvInteger {

namespace Properties {
const QString Value = "Value";
const QString Min = "Min";
const QString Max = "Max";
const QString Inc = "Inc";
const QString pValue = "pValue";
const QString pMin = "pMin";
const QString pMax = "pMax";
const QString pInc = "pInc";
const QString Representation = "Representation";
}

class ObjectPrivate : public JgvInode::ObjectPrivate
{
public:
    ObjectPrivate(QSharedPointer<JgvIport::Interface> iport)
        : JgvInode::ObjectPrivate(iport)
    {}

    qint64 value;
    qint64 min;
    qint64 max;
    qint64 inc;

    QWeakPointer<JgvInode::Object> pValue;
    QWeakPointer<JgvInode::Object> pMin;
    QWeakPointer<JgvInode::Object> pMax;
    QWeakPointer<JgvInode::Object> pInc;

    QString representation;

};
} // JgvInteger

#endif // INTEGER_P_H
