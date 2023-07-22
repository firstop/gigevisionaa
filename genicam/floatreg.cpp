/***************************************************************************
 *   Copyright (C) 2014-2015 by Cyril BALETAUD                             *
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

#include "floatreg.h"
#include "floatreg_p.h"
#include "genicamobjectbuilder.h"
#include "xmlhelper.h"
#include "genicam.h"

#include <QVariant>

using namespace JgvFloatReg;

Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInodeRegister::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);

    JgvInodeRegister::Object::prepare(builder, helper);
    d->sType = sType;

    d->sign = helper.getProperty(Properties::Sign);
}

void Object::invalidate()
{
    Q_D(Object);

    d->cache.isValid = false;
}

QVariant Object::getVariant()
{
    Q_D(const Object);
    if (!d->namespaceIsCustom) {
        return GenICam::Representation::toString(featureName(), getValue());
    }

    return QVariant(getValue());
}

double Object::getValue()
{
    Q_D (Object);

    if (!d->cache.isValid) {
        const uchar *p = data();
        if (p != 0) {
            const qint64 pSize = dataSize();
            if (pSize == 4) {
                d->cache.value = (d->isLittleEndian)?qFromLittleEndian<float>(p):qFromBigEndian<float>(p);
            }
            else if (pSize == 8) {
                d->cache.value = (d->isLittleEndian)?qFromLittleEndian<double>(p):qFromBigEndian<double>(p);
            }
            else {
                qWarning("FloatReg %s getValue error: buffer size: %lld", qPrintable(featureName()), pSize);
            }
        }
    }

    d->cache.isValid = d->readUpdateCache;
    return d->cache.value;
}

void Object::setValue(double value)
{
    Q_D(Object);

    d->cache.value = value;

    uchar *p = data();
    if (p == 0) {
        qWarning("FloatReg %s, InodeRegister pointer failed !", qPrintable(featureName()));
        return;
    }

    const qint64 pSize = dataSize();
    if (pSize == 4) {
        if (d->isLittleEndian) {
            qToLittleEndian<float>(static_cast<float>(d->cache.value), p);
        }
        else {
            qToBigEndian<float>(static_cast<float>(d->cache.value), p);
        }
    }
    else if (pSize == 8) {
        if (d->isLittleEndian) {
            qToLittleEndian<double>(d->cache.value, p);
        }
        else {
            qToBigEndian<double>(d->cache.value, p);
        }
    }
    else {
        qWarning("FloatReg::setValue error: buffer size: %lld", pSize);
        return;
    }

    updateData();
    d->cache.isValid = d->writeUpdateCache;

}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}

