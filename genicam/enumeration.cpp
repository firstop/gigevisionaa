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
#include "enumeration.h"
#include "enumeration_p.h"
#include "enumentry.h"
#include "iinteger.h"
#include "genicamobjectbuilder.h"
#include "xmlhelper.h"

#include <QVariant>
#include <QStringList>
#include <QDomNodeList>

using namespace JgvEnumeration;


Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInode::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);
    JgvInode::Object::prepare(builder, helper);
    d->sType = sType;

    bool ok;
    d->value = helper.getProperty(Properties::Value).toLongLong(&ok, 10);
    d->pValue = builder.buildInode(helper.getProperty(Properties::pValue));

    QDomNodeList entryList = helper.getChildrenNode(JgvEnumentry::sType);
    for (int i=0; i<entryList.count(); ++i) {
        XMLHelper helper(entryList.at(i));
        if (helper.isValid()) {
            QSharedPointer<JgvEnumentry::Object> entry(new JgvEnumentry::Object(d->iport));
            d->entries.append(entry);
            entry->prepare(builder, helper);
        }
    }
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
    return QVariant(getStringValue());
}

QString Object::getStringValue()
{
    Q_D(const Object);

    // obtient l'index courant
    const qint64 index = getIntValue();
    // obtient le texte lié à l'index
    foreach (QSharedPointer<JgvEnumentry::Object> entry, d->entries) {
        if (entry->getIntValue() == index) {
            return entry->getStringValue();
            break;
        }
    }
    return QString();
}

void Object::setStringValue(const QString &value)
{
    Q_D(Object);

    qDebug("TODO Interface::setStringValue");
}


qint64 Object::getIntValue()
{
    Q_D(Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            d->value = JGV_IINTEGER(p)->getValue();
        }
        else {
            d->warnForBadInterface("Enumeration getValue()", p, "IInteger");
        }
    }
    return d->value;
}

void Object::setIntValue(qint64 value)
{
    Q_D(Object);

    d->value = value;

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            JGV_IINTEGER(p)->setValue(d->value);
        }
        else {
            d->warnForBadInterface("enumeration setValue()", p, "IInteger");
        }
    }

    foreach (QWeakPointer<JgvInode::Object> inode, d->inodesToInvalidate) {
        QSharedPointer<JgvInode::Object> p = inode.toStrongRef();
        if (p) {
            p->invalidate();
        }
    }
}
/*!
 * \brief Interface::getEntries
 * Obtient les liste des entrées disponibles
 * \return La liste des entrées valides
 */
QList<JgvEnumentry::Object *> Object::getEntries() const
{
    Q_D(const Object);

    QList<JgvEnumentry::Object *> entries;
    foreach (QSharedPointer<JgvEnumentry::Object> entry, d->entries) {
        if (entry->isImplemented() && entry->isAvailable()) {
            entries.append(entry.data());
        }
    }
    return entries;
}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}






