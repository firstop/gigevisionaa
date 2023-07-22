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
#ifndef FLOATREG_P_H
#define FLOATREG_P_H

#include "inoderegister_p.h"

#include <QtEndian>

// float  IEEE-754 32 bits
// double IEEE-754 64 bits
template<>
inline float qFromBigEndian<float>(const uchar *src)
{
    const quint32 i = qFromBigEndian<quint32>(src);
    return *(reinterpret_cast<const float *>(&i));
}

template<>
inline double qFromBigEndian<double>(const uchar *src)
{
    const quint64 i = qFromBigEndian<quint64>(src);
    return *(reinterpret_cast<const double *>(&i));
}

template<>
inline void qToBigEndian<float>(float src, uchar *dest)
{
    const quint32 i = *(reinterpret_cast<quint32 *>(&src));
    qToBigEndian<quint32>(i, dest);
}

template<>
inline void qToBigEndian<double>(double src, uchar *dest)
{
    const quint64 i = *(reinterpret_cast<quint64 *>(&src));
    qToBigEndian<quint64>(i, dest);
}

template<>
inline float qFromLittleEndian<float>(const uchar *src)
{
    const quint32 i = qFromLittleEndian<quint32>(src);
    return *(reinterpret_cast<const float *>(&i));
}

template<>
inline double qFromLittleEndian<double>(const uchar *src)
{
    const quint64 i = qFromLittleEndian<quint64>(src);
    return *(reinterpret_cast<const double *>(&i));
}

template<>
inline void qToLittleEndian<float>(float src, uchar *dest)
{
    const quint32 i = *(reinterpret_cast<quint32 *>(&src));
    qToLittleEndian<quint32>(i, dest);
}

template<>
inline void qToLittleEndian<double>(double src, uchar *dest)
{
    const quint64 i = *(reinterpret_cast<quint64 *>(&src));
    qToLittleEndian<quint64>(i, dest);
}

namespace JgvFloatReg {

namespace Properties {
    const QString Sign = "Sign";
}

struct Cache {
    double value;
    bool isValid;
    Cache() : isValid(false) {}
};

class ObjectPrivate : public JgvInodeRegister::ObjectPrivate
{
public:
    ObjectPrivate(QSharedPointer<JgvIport::Interface> iport)
        : JgvInodeRegister::ObjectPrivate(iport)
    {}

    QString sign;
    Cache cache;
};

} // JgvFloatReg

#endif // FLOATREG_P_H
