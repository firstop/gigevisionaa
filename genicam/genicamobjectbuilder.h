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

#ifndef GENICAMOBJECTBUILDER_H
#define GENICAMOBJECTBUILDER_H

#include <QScopedPointer>
#include "iinterface.h"

template <class T>
class QWeakPointer;
template <class T>
class QSharedPointer;
template <class T, class U>
class QHash;
class QString;

class IPort;
class XMLHelper;

namespace JgvInode {
class Object;
}

namespace JgvIport {
class Interface;
}



class XMLHelper;
class InterfaceBuilderPrivate;
class InterfaceBuilder
{
public:
    InterfaceBuilder(const QString &fileName, const QByteArray &file, QSharedPointer<JgvIport::Interface> iport);
    ~InterfaceBuilder();

    QWeakPointer<JgvInode::Object> buildInode(const QString &name);
    QHash<QString, QSharedPointer<JgvInode::Object> > allReferencedInodes();

private:
    QSharedPointer<JgvInode::Object> buildInode(const XMLHelper &helper);
    const QScopedPointer<InterfaceBuilderPrivate> d;

};



#endif // GENICAMOBJECTBUILDER_H
