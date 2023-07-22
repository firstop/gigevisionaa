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

#include "inode.h"
#include "inode_p.h"

#include "xmlhelper.h"
#include "genicamobjectbuilder.h"
#include "iinteger.h"
#include "boolean.h"

#include <QStringList>

using namespace JgvInode;

void ObjectPrivate::warnForBadInterface(const QString &source, QSharedPointer<JgvInode::Object> inode, const QString &expectedInterfaces) const
{
    qWarning("GenICam inode %s, property %s [%s] have wrong interface %s, waiting for %s",
             qPrintable(featureName), qPrintable(source), qPrintable(inode->featureName()),
             qPrintable(inode->constInterface()->interfaceTypeString()), qPrintable(expectedInterfaces));
}


Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : d_ptr(new ObjectPrivate(iport))
{}

Object::Object(ObjectPrivate &dd)
    : d_ptr(&dd)
{}

Object::~Object()
{
    //qDebug("Deleting inode %s", qPrintable(featureName()));
}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);

    d->sType = sType;

    d->row = -1;
    d->toolTip = helper.getProperty(Properties::ToolTip);
    d->description = helper.getProperty(Properties::Description);
    d->displayName = helper.getProperty(Properties::DisplayName);
    d->visibility = helper.getProperty(Properties::Visibility);
    if (d->visibility.isEmpty()) {
        d->visibility = Visibility::Beginner;
    }
    d->eventID = helper.getProperty(Properties::EventID);
    d->pIsImplemented = builder.buildInode(helper.getProperty(Properties::pIsImplemented));
    d->pIsAvailable = builder.buildInode(helper.getProperty(Properties::pIsAvailable));
    d->pIsLocked = builder.buildInode(helper.getProperty(Properties::pIsLocked));
    d->featureName = helper.nameAttribut();
    if (d->displayName.isEmpty()){
        d->displayName = d->featureName;
    }
    // si la propriété ImposedAccesMode n'existe pas,
    // l'inode est en accès complet.
    d->imposedCanRead = true;
    d->imposedCanWrite = true;
    QString prop = helper.getProperty(Properties::ImposedAccessMode);
    if (prop == JgvInode::AccessMode::ReadOnly) {
        d->imposedCanWrite = false;
    }
    else if (prop == JgvInode::AccessMode::WriteOnly) {
        d->imposedCanRead = false;
    }

    d->namespaceIsCustom = helper.namespaceAttribut() == "Custom";
}

const QString &Object::typeString() const
{
    Q_D(const Object);
    return d->sType;
}

const QString &Object::displayName() const
{
    Q_D(const Object);
    return d->displayName;
}

const QString &Object::featureName() const
{
    Q_D(const Object);
    return d->featureName;
}

const QString &Object::description() const
{
    Q_D(const Object);
    return d->description;
}

const QString &Object::visibility() const
{
    Q_D(const Object);
    return d->visibility;
}

void Object::setInodeToInvalidate(QWeakPointer<Object> inode)
{
    Q_D(Object);

    if (!d->inodesToInvalidate.contains(inode)) {
        d->inodesToInvalidate.append(inode);
    }
}

QStringList Object::invalidateNames() const
{
    Q_D(const Object);

    QStringList list;
    foreach (QWeakPointer<JgvInode::Object> inode, d->inodesToInvalidate) {
        QSharedPointer<JgvInode::Object> p = inode.toStrongRef();
        if (p) {
            list.append(p->featureName());
        }
    }
    return list;
}

QStringList Object::getInvalidatorFeatureNames() const
{
    // un simple inode n'a pas d'invalidateur
    return QStringList();
}

void Object::setHierarchy(int row, QWeakPointer<JgvInode::Object> parent)
{
    Q_D(Object);
    d->row = row;
    d->parent = parent;
}

int Object::row() const
{
    Q_D(const Object);

    return d->row;
}

QWeakPointer<JgvInode::Object> Object::parent() const
{
    Q_D(const Object);

    return d->parent;
}

QWeakPointer<JgvInode::Object> Object::getChild(int raw) const
{
    // par défaut un inode n'a pas d'enfant.
    return QWeakPointer<JgvInode::Object>();
}

bool Object::haveChild() const
{
    // par défaut un inode n'a pas d'enfant.
    return false;
}

int Object::childCount() const
{
    // par défaut un inode n'a pas d'enfant.
    return 0;
}

int Object::getPollingTime() const
{
    // par défaut un inode n'a pas de temps de polling.
    return -1;
}

bool Object::isImplemented() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pIsImplemented.toStrongRef();
    // normalement selon GenIcam, on devrait avoir une interface IInteger
    // cependant, au moins chez Pleora on trouve des interfaces IBoolean
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            return (JGV_IINTEGER(p)->getValue() != 0);
        }
        if (JGV_ITYPE(p) == Jgv::IBoolean) {
            return (JGV_IBOOLEAN(p)->getValue());
        }
        d->warnForBadInterface("IsImplemented", p, "IInteger or IBoolean");
    }
    // si la propriété est absente, l'implémentation est valide.
    return true;
}

bool Object::isAvailable() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pIsAvailable.toStrongRef();
    // normalement selon GenIcam, on devrait avoir une interface IInteger
    // cependant, au moins chez Pleora on trouve des interfaces IBoolean
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            return (JGV_IINTEGER(p)->getValue() != 0);
        }
        if (JGV_ITYPE(p) == Jgv::IBoolean) {
            return (JGV_IBOOLEAN(p)->getValue());
        }
        d->warnForBadInterface("IsAvailable", p, "IInteger or IBoolean");
    }

    // si la propriété est absente, l'inode est disponible.
    return true;
}

bool Object::isLocked() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pIsLocked.toStrongRef();
    // normalement selon GenIcam, on devrait avoir une interface IInteger
    // cependant, au moins chez Pleora on trouve des interfaces IBoolean
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            return (JGV_IINTEGER(p)->getValue() != 0);
        }
        if (JGV_ITYPE(p) == Jgv::IBoolean) {
            return (JGV_IBOOLEAN(p)->getValue());
        }
        d->warnForBadInterface("IsLocked", p, "IInteger or IBoolean");
    }

    // si la propriété est absente, l'inode n'est pas bloqué.
    return false;
}











