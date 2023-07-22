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

#include "iinteger.h"

#include <limits>
#include <QString>

qint64 JgvInteger::Interface::getMin() const
{
    return std::numeric_limits<qint64>::min();
}

qint64 JgvInteger::Interface::getMax() const
{
    return std::numeric_limits<qint64>::min();
}
qint64 JgvInteger::Interface::getInc() const
{
    return Q_INT64_C(0);
}

Jgv::Type JgvInteger::Interface::interfaceType() const
{
    return Jgv::IInteger;
}

QString JgvInteger::Interface::interfaceTypeString() const
{
    return "IInteger";
}
