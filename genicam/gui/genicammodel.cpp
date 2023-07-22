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
#include "genicammodel.h"
#include "genicammodel_p.h"
#include "genicamobjectbuilder.h"
#include "inode.h"
#include "inode_p.h"
#include "icommand.h"
#include "iinteger.h"
#include "ifloat.h"
#include "ienumeration.h"
#include "istring.h"
#include "iboolean.h"
#include "xmlhelper.h"

#include <QFont>
#include <QColor>
#include <QTimerEvent>
#include <QSize>
#include <QDomNode>

GenicamModel::GenicamModel(QSharedPointer<JgvIport::Interface> iport, QObject *parent)
    : QAbstractItemModel(parent), d(new GenicamModelPrivate(iport))
{}

GenicamModel::~GenicamModel()
{
    qDebug("Deleting GenICam Model");
}

void GenicamModel::setGenICamXml(const QString &fileName, const QByteArray &file)
{
    InterfaceBuilder builder(fileName, file, d->iport);
    // construit l'arborescence complète des inodes
    builder.buildInode("Root");

    // gère la durée de vie des inodes
    d->inodes = builder.allReferencedInodes();
}

QModelIndex GenicamModel::searchFeature(int column, const QString &featureName) const
{
    QSharedPointer<JgvInode::Object> inode = d->inodes.value(featureName);
    if (inode) {
        QModelIndex idx = createIndex(inode->row(), column, inode.data());
        d->wrapper.insert(idx.internalId(), inode.toWeakRef());
        return idx;
    }
    return QModelIndex();
}


QModelIndex GenicamModel::index(int row, int column, const QModelIndex &parent) const
{

    // teste l'index (row >= 0, column >= 0, row < rowCount(parent), column < columnCount(parent))
    if (Q_UNLIKELY(!hasIndex(row, column, parent))) {
        return QModelIndex();
    }
    // si pas de parent, on est sur la racine
    QSharedPointer<JgvInode::Object> parentInode
            = (parent.isValid())
            ? d->wrapper.value(parent.internalId()).toStrongRef()
            : d->inodes.value("Root");

    QWeakPointer<JgvInode::Object> childInode = parentInode->getChild(row);
    QModelIndex idx = createIndex(row, column, childInode.toStrongRef().data());
    d->wrapper.insert(idx.internalId(), childInode);
    return idx;
}

QModelIndex GenicamModel::parent(const QModelIndex &child) const
{
    if (Q_UNLIKELY(!child.isValid())) {
        return QModelIndex();
    }

    QSharedPointer<JgvInode::Object> childInode = d->wrapper.value(child.internalId()).toStrongRef();
    if (childInode) {
        QWeakPointer<JgvInode::Object> parentInode = childInode->parent();
        QSharedPointer<JgvInode::Object> p = parentInode.toStrongRef();
        if (p) {
            QModelIndex idx = createIndex(p->row(), 0, p.data());
            d->wrapper.insert(idx.internalId(), parentInode);
            return idx;
        }
    }

    return QModelIndex();
}

int GenicamModel::rowCount(const QModelIndex &parent) const
{
    QSharedPointer<JgvInode::Object> p
            = (parent.isValid())
            ? d->wrapper.value(parent.internalId()).toStrongRef()
            : d->inodes.value("Root");

    if (p) {
       return p->childCount();
    }

    qCritical("GenicamModel rowCount failed to obtain inode pointer");
    return 0;
}

int GenicamModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    // DisplayName, Value, Visibility
    return 3;
}



QVariant GenicamModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid()) {
        return QVariant();
    }

    QWeakPointer<JgvInode::Object>  inode = d->wrapper.value(index.internalId());
    QSharedPointer<JgvInode::Object> p = inode.toStrongRef();
    if (!p) {
        return QVariant();
    }

    switch (role) {
    case Qt::UserRole:
        return QVariant::fromValue(inode);
    case Qt::DisplayRole:
        // colonne displayName
        if (index.column() == 0) {
            return QVariant(p->displayName());
        }
        else if (index.column() == 1) {
            if (!p->isImplemented()) return QVariant(trUtf8("{Non implémenté}"));
            if (!p->isAvailable()) return QVariant(trUtf8("{Indisponible}"));
            if (p->typeString() == "Category") return " ";
            return p->getVariant();
        }
        else if (index.column() == 2) {
            return QVariant(p->visibility());
        }
        break;
    case Qt::FontRole:
        // première colonne avec enfant, en gras
        if (index.column() == 0 && p->haveChild()) {
            QFont font;
            font.setBold(true);
            return QVariant(font);
        }
        break;
    case Qt::ForegroundRole:
        // sans enfant (seule ICategory a des enfants)
        if (!p->haveChild()) {
            // première colonne bleu foncé
            if (index.column() == 0) {
                return QVariant(QColor(Qt::darkBlue));
            }
            // deuxième colonne en gris si pas modifiable, en rouge si verrouillé
            if (index.column() == 1) {
                if (!p->isWritable()) {
                    return QVariant(QColor(Qt::darkGray));
                }
                if (p->isLocked())
                {
                    return QVariant(QColor(Qt::darkRed));
                }
            }
        }
        else if (index.column() == 0) {
            // première colonne category en noir
            return QVariant(QColor(Qt::black));
        }

        break;
    }
    return QVariant();
}

QVariant GenicamModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return QVariant("Feature");
        }
        if (section == 1) {
            return QVariant("Value");
        }
        if (section == 2) {
            return QVariant("Visibility");
        }
    }
    return QVariant();
}

Qt::ItemFlags GenicamModel::flags(const QModelIndex &index) const
{
    // si l'index est invalide, ou concerne une colonne non visible
    if (!index.isValid() || index.column() > 1) {
        return Qt::NoItemFlags;
    }

    QWeakPointer<JgvInode::Object> inode = d->wrapper.value(index.internalId());
    QSharedPointer<JgvInode::Object> p = inode.toStrongRef();
    if (!p) {
        return Qt::NoItemFlags;
    }

    // ne sont activé que les items de la 2eme colonne sans enfants
    if ((index.column() != 1) || p->haveChild()) {
        return Qt::NoItemFlags;
    }

    // si l'inode est writable et pas verrouillé, il devient éditable
    return (p->isWritable() && (!p->isLocked())) ? Qt::ItemIsEnabled|Qt::ItemIsEditable : Qt::ItemIsEnabled;
}

bool GenicamModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) {
        return false;
    }
    bool ret = false;
    QWeakPointer<JgvInode::Object> inode = d->wrapper.value(index.internalId());
    QSharedPointer<JgvInode::Object> p = inode.toStrongRef();
    if (!p) {
        return Qt::NoItemFlags;
    }

    switch (JGV_ITYPE(p)) {

    case Jgv::ICommand:
        JGV_ICOMMAND(p)->execute();
        ret = true;
        break;

    case Jgv::IInteger:
        JGV_IINTEGER(p)->setValue(value.toLongLong());
        ret = true;
        break;

    case Jgv::IFloat:
        JGV_IFLOAT(p)->setValue(value.toDouble());
        ret = true;
        break;

    case Jgv::IEnumeration:
        JGV_IENUMERATION(p)->setIntValue(value.toLongLong());
        ret = true;
        break;

    case Jgv::IString:
        JGV_ISTRING(p)->setValue(value.toByteArray());
        ret = true;
        break;

    case Jgv::IBoolean:
        JGV_IBOOLEAN(p)->setValue(value.toBool());
        ret = true;
        break;


    default:
        qWarning("%s", qPrintable(QString("GenicamModel setData error %0").arg(p->typeString())));
    }


    if (ret) {
        emit dataChanged(index, index);
        foreach (QString inodeName, p->invalidateNames()) {
            QSharedPointer<JgvInode::Object> invalideInode = d->inodes.value(inodeName);
            QModelIndex invalideInodeIndex = createIndex(invalideInode->row(), 1, invalideInode.data());
            dataChanged(invalideInodeIndex, invalideInodeIndex);
        }
    }
    return ret;
}

QString GenicamModel::name() const
{
    // on passe par les inodes
    QSharedPointer<JgvInode::Object> inode = d->inodes.value("DeviceModelName");
    if (inode) {
        if (inode->interface()->interfaceType() == Jgv::IString) {
            return JGV_ISTRING(inode)->getValue();
        }
    }
    return trUtf8("GiGE Vision caméra");
}

void GenicamModel::timerEvent(QTimerEvent *event)
{
    //    QList<QSharedPointer<JgvInode::Object> > inodes = m_pollingsTimer.values(event->timerId());
    //    foreach(QSharedPointer<JgvInode::Object> inode, inodes) {
    //        QModelIndex index = createIndex(inode->row(), 1, inode.data());
    //        dataChanged(index, index);
    //    }
}

