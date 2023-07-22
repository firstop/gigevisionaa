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

#ifndef GENICAMOBJECTBUILDER_P_H
#define GENICAMOBJECTBUILDER_P_H

#include <QHash>
#include <QSharedPointer>

#include "genicamxmlfile.h"

class IPort;
namespace JgvIport {
class Interface;
}

namespace JgvInode{class Object;}

class InterfaceBuilderPrivate
{
public:
    InterfaceBuilderPrivate(QSharedPointer<JgvIport::Interface> iport);
    ~InterfaceBuilderPrivate();

    QSharedPointer<JgvIport::Interface> iport;
    QHash<QString, QSharedPointer<JgvInode::Object> > inodes;

    GenICamXMLFile xmlFile;

};

#endif // GENICAMOBJECTBUILDER_P_H
