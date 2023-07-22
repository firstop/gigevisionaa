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
#include "command.h"
#include "command_p.h"
#include "icommand.h"
#include "iinteger.h"
#include "genicamobjectbuilder.h"
#include "xmlhelper.h"

#include <QVariant>

using namespace JgvCommand;


Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInode::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);

    JgvInode::Object::prepare(builder, helper);
    d->sType = sType;

    bool ok;
    d->value = helper.getProperty(Properties::Value).toUInt(&ok, 16);
    d->commandValue = helper.getProperty(Properties::CommandValue).toInt(&ok, 10);

    d->pValue = builder.buildInode(helper.getProperty(Properties::pValue));
    d->pCommandValue = builder.buildInode(helper.getProperty(Properties::pCommandValue));

    // gestion de l'acquisition
    // SNFC : Standard Features Naming Convention
    // Le SNFC définit le node TLParamsLocked comme étant un verrou
    // qui interdit les changements critiques sur le device.
    // Devient un locker, une commande qui lance le stream,
    // un unlocker une commande qui stop le flux.
    d->isTLLocker = false;
    d->isTLUnlocker = false;
    if (d->featureName == "AcquisitionStart") {
        d->isTLLocker = true;
        d->pTLLocker = builder.buildInode("TLParamsLocked");
        d->inodesToInvalidate.append(d->pTLLocker);
    }
    else if (d->featureName == "AcquisitionStop"
             || d->featureName == "AcquisitionAbord")
    {
        d->isTLUnlocker = true;
        d->pTLLocker = builder.buildInode("TLParamsLocked");
        d->inodesToInvalidate.append(d->pTLLocker);
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

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    return (p) ? p->isWritable() : true;
}

QVariant Object::getVariant()
{
    return QVariant("<command>");
}

void Object::execute() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            qint64 commandValue = d->commandValue;
            QSharedPointer<JgvInode::Object> pCommandValue = d->pCommandValue.toStrongRef();
            if (pCommandValue) {
                if (pCommandValue->constInterface()->interfaceType() == Jgv::IInteger) {
                    commandValue = JGV_IINTEGER(pCommandValue)->getValue();
                }
                else {
                    d->warnForBadInterface("Command pCommandValue", p, "IInteger");
                    return;
                }
            }
            JGV_IINTEGER(p)->setValue(commandValue);

            int lock = -1;
            if (d->isTLLocker) {
                lock = 1;
            }
            else if (d->isTLUnlocker) {
                lock = 0;
            }
            // si la commande modifie le verrou, on change la valeur de ce verrou (si le noeud existe)
            if (lock >= 0) {
                p = d->pTLLocker.toStrongRef();
                if (p) {
                    // le SNFC définit l'interface du locker comme IInteger
                    if (JGV_ITYPE(p) == Jgv::IInteger) {
                        JGV_IINTEGER(p)->setValue(lock);
                    }
                    else {
                        d->warnForBadInterface("Command pTLLocker", p, "IInteger");
                    }
                }
            }
        }
        else {
            d->warnForBadInterface("Command setValue()", p, "IInteger");
        }

    }
}

bool Object::isDone() const
{
    qDebug("%s",qPrintable("TODO command isDone"));
}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}



