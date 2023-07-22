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

#include "intreg.h"
#include "intreg_p.h"
#include "xmlhelper.h"
#include "genicam.h"

#include <QVariant>
#include <QtEndian>

using namespace JgvIntReg;

Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInodeRegister::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);

    JgvInodeRegister::Object::prepare(builder, helper);
    d->sType = sType;

    d->isUnsigned = helper.getProperty(Properties::Sign) == Sign::Unsigned;
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

qint64 Object::getValue()
{
    Q_D(Object);

    if (!d->cache.isValid) {
        const uchar *p = data();
        if (p != 0) {
            const qint64 size = dataSize();
            if (size == 4) {
                d->cache.value = (d->isLittleEndian)
                        ? (d->isUnsigned ? qFromLittleEndian<quint32>(p) : qFromLittleEndian<qint32>(p))
                        : (d->isUnsigned ? qFromBigEndian<quint32>(p) : qFromBigEndian<qint32>(p));
                d->cache.isValid = d->readUpdateCache;
            }
            else if (size == 8) {
                d->cache.value = (d->isLittleEndian)
                        ? (d->isUnsigned ? qFromLittleEndian<qint64>(p) : qFromLittleEndian<qint64>(p))
                        : (d->isUnsigned ? qFromBigEndian<qint64>(p) : qFromBigEndian<qint64>(p));
                d->cache.isValid = d->readUpdateCache;
            }
            else {
                qWarning("%s", qPrintable(QString("IntReg::getValue le buffer a une taille non reconnue %0").arg(size)));
            }
        }
        else {
            qWarning("IntReg %s, InodeRegister pointer failed !", qPrintable(featureName()));
        }
    }
    return d->cache.value;
}

void Object::setValue(qint64 value)
{
    Q_D(Object);

    d->cache.value = value;

    uchar *p = data();
    if (p == 0) {
        qWarning("IntReg %s, InodeRegister pointer failed !", qPrintable(featureName()));
        return;
    }

    const qint64 size = dataSize();
    if (size == 4) {
        if (d->isLittleEndian) {
            qToLittleEndian<qint32>(d->cache.value, p);
        }
        else {
            qToBigEndian<qint32>(d->cache.value, p);
        }
    }
    else if (size == 8) {
        if (d->isLittleEndian) {
            qToLittleEndian<qint64>(d->cache.value, p);
        }
        else {
            qToBigEndian<qint64>(d->cache.value, p);
        }
    }
    else {
        qWarning("%s", qPrintable(QString("IntReg::setValue le buffer a une taille non reconnue %0").arg(size)));
    }

    // provoque l'écriture sur l'iport
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



