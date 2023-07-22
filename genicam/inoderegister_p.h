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

#ifndef REGISTERINODE_P_H
#define REGISTERINODE_P_H

#include "inode_p.h"
#include <QScopedPointer>

namespace JgvInodeRegister {

namespace Endianess {
const QString BigEndian = "BigEndian";
const QString LittleEndian = "LittleEndian";
}

namespace Cache {
const QString NoCache = "NoCache";              // pas de cache
const QString WriteThrough = "WriteThrough";    // une écriture sur le device implique une écriture sur le cache
const QString WriteAround = "WriteAround";      // seule la lecture est mise en cache
}

namespace Properties {
const QString Address = "Address";
const QString Length = "Length";
const QString pLength = "pLength";
const QString AccessMode = "AccessMode";
const QString pPort = "pPort";
const QString Cachable = "Cachable";
const QString PollingTime = "PollingTime";
const QString pAddress = "pAddress";
const QString IntSwissKnife = "IntSwissKnife";
const QString pInvalidator = "pInvalidator";
const QString pIndex = "pIndex";
const QString Endianess = "Endianess";
}


class ObjectPrivate : public JgvInode::ObjectPrivate
{
public:
    ObjectPrivate(QSharedPointer<JgvIport::Interface> iport)
        : JgvInode::ObjectPrivate(iport)
    {}
    virtual ~ObjectPrivate(){}

    bool canRead;
    bool canWrite;
    bool readUpdateCache;
    bool writeUpdateCache;

    bool isLittleEndian;

    QString pPort;
    int pollingTime;

    qint64 length;
    QWeakPointer<JgvInode::Object> pLength;

    QList<qint64> addresses;
    QList<QWeakPointer<JgvInode::Object> > pAddresses;
    QList<QWeakPointer<JgvInode::Object> > pInvalidators;

    QWeakPointer<JgvInode::Object> pIndex;
    int indexOffset;


private:
    QScopedPointer<quint8, QScopedPointerArrayDeleter<quint8> > ptr;
    qint64 ptrSize;
    friend class Object;
    void initPtr();
};
}

#endif // REGISTERINODE_P_H
