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

#ifndef REGISTER_H
#define REGISTER_H

#include "inoderegister.h"
#include "iregister.h"

namespace JgvRegister {

const QString sType = "Register";

class ObjectPrivate;
class Object :  public JgvInodeRegister::Object, public JgvRegister::Interface
{

public:
    Object(QSharedPointer<JgvIport::Interface> iport);

    void prepare(InterfaceBuilder &builder, const XMLHelper &helper);
    void invalidate();

    QVariant getVariant();

    void get(quint8 *buffer, qint64 length);
    void set(quint8 *buffer, qint64 length);
    qint64 getAddress();
    qint64 getLength();

    Jgv::Interface *interface();
    const Jgv::Interface *constInterface() const;


private:
    Q_DECLARE_PRIVATE(Object)

}; // Interface

} // REGISTER_H

#endif // INTEGER_H
