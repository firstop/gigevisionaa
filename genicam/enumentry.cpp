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
#include "enumentry.h"
#include "enumentry_p.h"
#include "genicamobjectbuilder.h"
#include "xmlhelper.h"

#include <QVariant>

using namespace JgvEnumentry;

Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInode::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);
    JgvInode::Object::prepare(builder, helper);
    d->sType = sType;

    bool ok;
    d->value = 0;
    QString prop = helper.getProperty(Properties::Value);
    qint64 val = prop.toLongLong(&ok, 10);
    if (ok) {
        d->value = val;
    }
    else {
        val = prop.toLongLong(&ok, 16);
        if (ok) {
            d->value = val;
        }
    }
    d->name = helper.nameAttribut();
}

void Object::invalidate()
{}

bool Object::isWritable() const
{
    return false;
}

QString Object::getStringValue() const
{
    Q_D(const Object);

    return d->name;
}

qint64 Object::getIntValue() const
{
    Q_D(const Object);

    return d->value;
}

QVariant Object::getVariant()
{
    return QVariant();
}

Jgv::Interface *Object::interface()
{
    return 0;
}

const Jgv::Interface *Object::constInterface() const
{
    return 0;
}

