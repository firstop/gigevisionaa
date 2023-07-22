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
#ifndef CONVERTER_P_H
#define CONVERTER_P_H

#include "inode_p.h"
#include <QMap>

class Formula;

namespace JgvConverter {

namespace Properties {
    const QString FormulaTo = "FormulaTo";
    const QString FormulaFrom = "FormulaFrom";
    const QString pVariable = "pVariable";
    const QString pValue = "pValue";
    const QString Slope = "Slope";
}

namespace Slope {
    const QString INCREASING = "Increasing";
    const QString DECREASING = "Decreasing";
    const QString VARYING = "Varying";
    const QString AUTOMATIC = "Automatic";
    enum Value {
        Increasing,
        Decreasing,
        Varying,
        Automatic
    };
}

class ObjectPrivate : public JgvInode::ObjectPrivate
{
public:
    ObjectPrivate(QSharedPointer<JgvIport::Interface> iport)
        : JgvInode::ObjectPrivate(iport)
    {}

    QMap<QString, QWeakPointer<JgvInode::Object> > pVariables;
    QWeakPointer<JgvInode::Object> pValue;

    QScopedPointer<Formula> formulaFrom;
    QScopedPointer<Formula> formulaTo;

    Slope::Value slope;
};

} // JGV_IntReg

#endif // CONVERTER_P_H
