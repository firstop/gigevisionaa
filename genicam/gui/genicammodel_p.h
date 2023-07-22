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
#ifndef GENICAMMODEL_P_H
#define GENICAMMODEL_P_H

template<class T, class U>
class QHash;
template<class T, class U>
class QMap;
template <class T>
class QSharedPointer;
template <class T>
class QWeakPointer;
namespace JgvInode {class Object;}
namespace JgvIport {class Interface;}
class QString;

namespace JgvInode {class Object;}

class GenicamModelPrivate
{
public:
    GenicamModelPrivate(QSharedPointer<JgvIport::Interface> iport):iport(iport){}
    QHash<QString, QSharedPointer<JgvInode::Object> > inodes;
    QSharedPointer<JgvIport::Interface> iport;
    QMap<qint64, QWeakPointer<JgvInode::Object> > wrapper;

};

#endif // GENICAMMODEL_P_H
