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

#ifndef INODE_P_H
#define INODE_P_H


#include <QString>
#include <QSharedPointer>
#include <QWeakPointer>

namespace JgvInode {
class Object;
}

namespace JgvIport {
class Interface;
}
namespace JgvInode {

namespace Properties {
const QString ToolTip = "ToolTip";
const QString Description = "Description";
const QString DisplayName = "DisplayName";
const QString Visibility = "Visibility";
const QString ImposedAccessMode = "ImposedAccessMode";
const QString EventID = "EventID";
const QString pIsImplemented = "pIsImplemented";
const QString pIsAvailable = "pIsAvailable";
const QString pIsLocked = "pIsLocked";
}

namespace Visibility {
const QString Beginner = "Beginner";
const QString Expert = "Expert";
const QString Guru = "Guru";
const QString Invisible = "Invisible";
}

namespace AccessMode {
const QString ReadOnly = "RO";
const QString ReadWrite = "RW";
const QString WriteOnly = "WO";
}

class ObjectPrivate
{
public:
    ObjectPrivate(QSharedPointer<JgvIport::Interface> iport)
        : iport(iport)
    {}
    virtual ~ObjectPrivate(){}

    QString sType;
    QString featureName;
    QString toolTip;
    QString description;
    QString displayName;
    QString visibility;
    bool imposedCanRead;
    bool imposedCanWrite;
    bool namespaceIsCustom;

    QString eventID;

    QWeakPointer<JgvInode::Object> pIsImplemented;
    QWeakPointer<JgvInode::Object> pIsAvailable;
    QWeakPointer<JgvInode::Object> pIsLocked;

    int row;
    QWeakPointer<JgvInode::Object> parent;
    QList<QWeakPointer<JgvInode::Object> > inodesToInvalidate;

    QSharedPointer<JgvIport::Interface> iport;
    void warnForBadInterface(const QString &source, QSharedPointer<JgvInode::Object> inode, const QString &expectedInterfaces) const;

}; // ObjectPrivate

} // Inode

#endif // INODE_P_H
