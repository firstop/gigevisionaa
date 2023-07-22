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

#ifndef ISTRING_H
#define ISTRING_H

#include "iinteger.h"
#include <QtGlobal>

namespace JgvString {

    class Interface : public Jgv::Interface
    {
    public:
        virtual ~Interface(){}

        virtual QByteArray getValue() = 0;
        virtual void setValue(const QByteArray &value) = 0;
        virtual qint64 getMaxLenght() = 0;

        Jgv::Type interfaceType() const;
        QString interfaceTypeString() const;
    };

} // IString

#endif // IINTEGER_H
