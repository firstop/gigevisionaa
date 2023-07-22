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

#include "ifloat.h"

#include <limits>
#include <QString>

double JgvFloat::Interface::getMin() const
{
    return std::numeric_limits<double>::min();
}

double JgvFloat::Interface::getMax() const
{
    return std::numeric_limits<double>::max();
}

double JgvFloat::Interface::getInc() const
{
    return 0.;
}

Jgv::Type JgvFloat::Interface::interfaceType() const
{
    return Jgv::IFloat;
}

QString JgvFloat::Interface::interfaceTypeString() const
{
    return "IFloat";
}
