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
#include "jgvfloat.h"
#include "jgvfloat_p.h"
#include "genicamobjectbuilder.h"
#include "xmlhelper.h"
#include "genicam.h"

#include <limits>
#include <QVariant>

using namespace JgvFloat;

Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInode::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);

    JgvInode::Object::prepare(builder, helper);
    d->sType = sType;

    bool ok;
    d->value = helper.getProperty(Properties::Value).toDouble(&ok);
    d->min = helper.getProperty(Properties::Min).toDouble(&ok);
    if (!ok) d->min = std::numeric_limits<double>::min();
    d->max = helper.getProperty(Properties::Max).toDouble(&ok);
    if (!ok) d->max = std::numeric_limits<double>::max();
    d->inc = helper.getProperty(Properties::Inc).toDouble(&ok);

    d->pValue = builder.buildInode(helper.getProperty(Properties::pValue));
    d->pMin = builder.buildInode(helper.getProperty(Properties::pMin));
    d->pMax = builder.buildInode(helper.getProperty(Properties::pMax));
    d->pInc = builder.buildInode(helper.getProperty(Properties::pInc));

    d->representation = helper.getProperty(Properties::Representation);
    d->unit = helper.getProperty(Properties::Unit);
    d->displayNotation = helper.getProperty(Properties::DisplayNotation);
    d->precision = helper.getProperty(Properties::DisplayPrecision).toInt();
}

void Object::invalidate()
{
    Q_D(Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        p->invalidate();
    }
}

bool Object::isWritable() const
{
    Q_D(const Object);

    if (!d->imposedCanWrite) {
        return false;
    }

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    return (p) ? p->isWritable() : true;
}

QVariant Object::getVariant()
{
    Q_D(const Object);
    if (!d->namespaceIsCustom) {
        return GenICam::Representation::toString(featureName(), getValue());
    }

    return QVariant(getValue());
}

int Object::getPollingTime() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    return (p) ? p->getPollingTime() : -1;
}

double Object::getValue()
{
    Q_D(Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IFloat) {
            d->value = JGV_IFLOAT(p)->getValue();
        }
        else {
            d->warnForBadInterface("Float getValue()", p, "IFloat");
        }
    }
    return d->value;
}

void Object::setValue(double value)
{
    Q_D(Object);

    d->value = value;
    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IFloat) {
            JGV_IFLOAT(p)->setValue(d->value);
        }
        else {
            d->warnForBadInterface("Float setValue()", p, "IFloat");
        }
    }
}

double Object::getMin() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pMin.toStrongRef();
    return (p) ? JGV_IFLOAT(p)->getValue() : d->min;
}

double Object::getMax() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pMax.toStrongRef();
    return (p) ? JGV_IFLOAT(p)->getValue() : d->max;
}

double Object::getInc() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pInc.toStrongRef();
    return (p) ? JGV_IFLOAT(p)->getValue() : d->inc;
}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}


