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

#ifndef IINTERFACE_H
#define IINTERFACE_H

#define JGV_IINTEGER( p ) Jgv::Interface::cast<JgvInteger::Interface>(p->interface())
#define JGV_IFLOAT( p ) Jgv::Interface::cast<JgvFloat::Interface>(p->interface())
#define JGV_ICOMMAND( p ) Jgv::Interface::cast<JgvCommand::Interface>(p->interface())
#define JGV_IENUMERATION( p ) Jgv::Interface::cast<JgvEnumeration::Interface>(p->interface())
#define JGV_IREGISTER( p ) Jgv::Interface::cast<JgvRegister::Interface>(p->interface())
#define JGV_IBOOLEAN( p ) Jgv::Interface::cast<JgvBoolean::Interface>(p->interface())
#define JGV_ISTRING( p ) Jgv::Interface::cast<JgvString::Interface>(p->interface())
#define JGV_ITYPE( p ) p->constInterface()->interfaceType()


class QString;

namespace Jgv {
enum Type {
    ICommand,
    IInteger,
    IFloat,
    IEnumeration,
    IBoolean,
    ICategory,
    IString,
    IRegister,
    Unknow
};


class Interface
{
public:
    virtual ~Interface() {}
    /*!
     * \brief
     * Cette propriété contient le type d'interface qu'implémente l'inode.
     * \return Le type d'interface implémenté.
     */
    virtual Type interfaceType() const = 0;
    /*!
     * \brief
     * Cette propriété contient le nom du type d'interface qu'implémente l'inode.
     * \return Le nom du type d'interface implémenté.
     */
    virtual QString interfaceTypeString() const = 0;


    template <class T, class U>
    static T *cast(U *ptr) {return static_cast<T *>(ptr);}

    template <class T, class U>
    static T *constCast(U *ptr) {return static_cast<const T *>(ptr);}

};

}


#endif // IINTERFACE_H
