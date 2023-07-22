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

#ifndef SWISSKNIFE_H
#define SWISSKNIFE_H

#include "inode.h"
#include "ifloat.h"

namespace JgvSwissknife {

const QString sType = "SwissKnife";

class ObjectPrivate;
class Object : public JgvInode::Object, public JgvFloat::Interface
{

public:
    Object(QSharedPointer<JgvIport::Interface> iport);

    void prepare(InterfaceBuilder &builder, const XMLHelper &helper);

    void invalidate();

    bool isWritable() const;

    QVariant getVariant();

    double getValue();
    void setValue(double value);

    Jgv::Interface *interface();
    const Jgv::Interface *constInterface() const;

private:

    Q_DECLARE_PRIVATE(Object)

}; // Interface

} // SwissKnife

#endif // SWISSKNIFE_H
