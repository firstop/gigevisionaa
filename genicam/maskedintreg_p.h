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

#ifndef MASKEDINTREG_P_H
#define MASKEDINTREG_P_H

#include "inoderegister_p.h"

namespace JgvMaskedIntReg {

namespace Properties {
const QString LSB = "LSB";
const QString MSB = "MSB";
const QString Bit = "Bit";
} // Properties

struct Cache {
    qint64 value;
    bool isValid;
    Cache() : isValid(false) {}
};

class ObjectPrivate : public JgvInodeRegister::ObjectPrivate
{
public:
    ObjectPrivate(QSharedPointer<JgvIport::Interface> iport)
        : JgvInodeRegister::ObjectPrivate(iport)
    {}

    int lsb;
    int msb;
    int bit;

    Cache cache;
};
} // JgvMaskedIntReg

#endif // MASKEDINTREG_P_H
