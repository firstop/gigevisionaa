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

#include "inoderegister.h"
#include "inoderegister_p.h"
#include "genicamobjectbuilder.h"
#include "xmlhelper.h"
#include "iport.h"
#include "iinteger.h"

#include <QStringList>

using namespace JgvInodeRegister;

void ObjectPrivate::initPtr()
{
    ptrSize = length;
    QSharedPointer<JgvInode::Object> p = pLength.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            ptrSize = JGV_IINTEGER(p)->getValue();
        }
        else {
            warnForBadInterface("InodeRegister InitPtr", p, "IInteger");
        }
    }
    if (ptrSize > 0) {
        QScopedPointer<quint8, QScopedPointerArrayDeleter<quint8> > newPtr(new quint8[ptrSize]);
        ptr.swap(newPtr);
    }
}


Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInode::Object(*new ObjectPrivate(iport))
{}

Object::~Object()
{}

Object::Object(ObjectPrivate &dd)
    : JgvInode::Object(dd)
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);

    // prepare l'inode
    JgvInode::Object::prepare(builder, helper);

    d->sType = sType;
    d->ptrSize = 0;

    bool ok;
    foreach (QString prop, helper.getProperties(Properties::Address)) {
        qint64 val = prop.toLongLong(&ok, 16);
        if (ok) {
            d->addresses.append(val);
        }
        else {
            qWarning("%s", qPrintable(QString("%0 Register init: erreur converting address %1")
                                      .arg(d->featureName)
                                      .arg(prop)));
        }
    }

    foreach (QString prop, helper.getProperties(Properties::pAddress)) {
        d->pAddresses.append(builder.buildInode(prop));
    }

    d->length = helper.getProperty(Properties::Length).toLongLong(&ok, 10);
    d->pLength = builder.buildInode(helper.getProperty(Properties::pLength));


    // si ce n'est pas précisé, le registre est en bigEndian
    d->isLittleEndian = (helper.getProperty(Properties::Endianess) == Endianess::LittleEndian);


    // si la propriété AccesMode n'existe pas,
    // le registre est en accès complet.
    d->canRead = true;
    d->canWrite = true;
    QString accesMode = helper.getProperty(Properties::AccessMode);
    if (accesMode == JgvInode::AccessMode::ReadOnly) {
        d->canWrite = false;
    }
    else if (accesMode == JgvInode::AccessMode::WriteOnly) {
        d->canRead = false;
    }

    // par défaut, tout est dans le cache
    d->readUpdateCache = true;
    d->writeUpdateCache = true;
    QString cachable = helper.getProperty(Properties::Cachable);
    // WriteThrough : une écriture provoque la mise à jour du cache (la lecture aussi)
    if (cachable == Cache::WriteThrough) {
        d->readUpdateCache = true;
        d->writeUpdateCache = true;
    }
    // WriteAround : seule une lecture provoque la mise à jour du cache
    else if (cachable == Cache::WriteAround) {
        d->readUpdateCache = true;
        d->writeUpdateCache = false;
    }
    // NoCache : fonctionnement sans cache.
    else if (cachable == Cache::NoCache) {
        d->readUpdateCache = false;
        d->writeUpdateCache = false;
    }

    d->pPort = helper.getProperty(Properties::pPort);

    // si pas de polling time, on retourne -1
    d->pollingTime = helper.getProperty(Properties::PollingTime).toLongLong(&ok, 10);
    if (!ok) {
        d->pollingTime = -1;
    }

    foreach (QString prop, helper.getProperties(Properties::pInvalidator)) {
        QWeakPointer<JgvInode::Object> inode = builder.buildInode(prop);
        QSharedPointer<JgvInode::Object> p = inode.toStrongRef();
        if (p) {
            d->pInvalidators.append(inode);
            // obtient le weakpointer sur this par le builder
            p->setInodeToInvalidate(builder.buildInode(d->featureName));
        }
    }

    d->indexOffset = 0;
    d->pIndex = builder.buildInode(helper.getProperty(Properties::pIndex));
    QSharedPointer<JgvInode::Object> p = d->pIndex.toStrongRef();
    if (p) {
        d->indexOffset = helper.getChildAttributeValue(Properties::pIndex, "Offset").toInt(&ok);
        if (!ok) {
            d->indexOffset = 0;
            qWarning("InodeRegister %s prepare(): failed to get index offset",
                     qPrintable(d->featureName));
        }
    }

}

QStringList Object::getInvalidatorFeatureNames() const
{
    Q_D(const Object);
    QStringList invalidators;
    foreach (QWeakPointer<JgvInode::Object> inode, d->pInvalidators) {
        QSharedPointer<JgvInode::Object> p = inode.toStrongRef();
        if (p) {
            invalidators.append(p->featureName());
        }
    }
    return invalidators;
}

int Object::getPollingTime() const
{
    Q_D(const Object);
    return d->pollingTime;
}

bool Object::isWritable() const
{
    Q_D(const Object);
    return d->canWrite;
}

uchar *Object::data()
{
    Q_D(Object);

    // premier appel
    if (!d->ptr) {
        d->initPtr();
    }
    if (d->ptr) {
        d->iport->read(d->ptr.data(), address(), d->ptrSize);
        return d->ptr.data();
    }
    return 0;
}

qint64 Object::dataSize()
{
    Q_D(Object);

    // premier appel
    if (!d->ptr) {
        d->initPtr();
    }
    return d->ptrSize;
}

void Object::updateData()
{
    Q_D(Object);
    if (d->canWrite && d->ptr) {
        d->iport->write(d->ptr.data(), address(), dataSize());
        // invalide les inodes listés
        foreach (QWeakPointer<JgvInode::Object> inode, d->inodesToInvalidate) {
            QSharedPointer<JgvInode::Object> p = inode.toStrongRef();
            if (p) {
                p->invalidate();
            }
        }
    }
}

qint64 Object::address() const
{
    Q_D(const Object);

    qint64 address = 0;
    foreach (qint64 i, d->addresses) {
        address += i;
    }

    foreach (QWeakPointer<JgvInode::Object> ii, d->pAddresses) {
        QSharedPointer<JgvInode::Object> p = ii.toStrongRef();
        if (p) {
            if (JGV_ITYPE(p) == Jgv::IInteger) {
                address += JGV_IINTEGER(p)->getValue();;
            }
            else {
                d->warnForBadInterface("InodeRegister address()", p, "IInteger");
            }
        }
    }

    QSharedPointer<JgvInode::Object> p = d->pIndex.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            address += (d->indexOffset * JGV_IINTEGER(p)->getValue());
        }
        else {
            d->warnForBadInterface("InodeRegister address()", p, "IInteger");
        }

    }

    return address;
}








