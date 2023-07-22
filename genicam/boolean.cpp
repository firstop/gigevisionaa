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
#include "boolean.h"
#include "boolean_p.h"
#include "iinteger.h"
#include "genicamobjectbuilder.h"
#include "xmlhelper.h"

#include <QVariant>

using namespace JgvBoolean;

Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInode::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);

    JgvInode::Object::prepare(builder, helper);

    d->sType = sType;
    d->value = helper.getProperty(Properties::Value).toInt();

    bool ok;
    int val = helper.getProperty(Properties::OnValue).toInt(&ok);
    d->onValue = ok?val:1;
    val = helper.getProperty(Properties::OffValue).toInt(&ok);
    d->offValue = ok?val:0;

    d->pValue = builder.buildInode(helper.getProperty(Properties::pValue));
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
    return getValue() ? QVariant("True") : QVariant("False");
}

bool Object::getValue()
{
    Q_D(Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            d->value = JGV_IINTEGER(p)->getValue();
        }
        else {
            d->warnForBadInterface("Boolean getValue()", p, "IInteger");
        }
    }
    return (d->value != d->offValue);
}

void Object::setValue(bool value)
{
    Q_D(Object);

    d->value = (value) ? d->onValue : d->offValue;

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            // écrit vers le registre
            JGV_IINTEGER(p)->setValue(d->value);
        }
        else {
            d->warnForBadInterface("Boolean setValue()", p, "IInteger");
        }
    }
}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}






