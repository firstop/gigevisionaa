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
#ifndef BOOLEAN_P_H
#define BOOLEAN_P_H

#include "inode_p.h"

namespace JgvBoolean {

namespace Properties {
    const QString Value = "Value";
    const QString pValue = "pValue";
    const QString OnValue = "OnValue";
    const QString OffValue = "OffValue";

} // Properties

class ObjectPrivate : public JgvInode::ObjectPrivate
{
public:
    ObjectPrivate(QSharedPointer<JgvIport::Interface> iport)
        : JgvInode::ObjectPrivate(iport)
    {}

    qint64 value;
    qint64 onValue;
    qint64 offValue;
    QWeakPointer<JgvInode::Object> pValue;
};
}

#endif // BOOLEAN_P_H
