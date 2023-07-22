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

#ifndef MASKEDINTREG_H
#define MASKEDINTREG_H

#include "inoderegister.h"
#include "iinteger.h"

namespace JgvMaskedIntReg {

    const QString sType = "MaskedIntReg";

    class ObjectPrivate;
    class Object : public JgvInodeRegister::Object, public JgvInteger::Interface
    {

    public:
        Object(QSharedPointer<JgvIport::Interface> iport);
        void prepare(InterfaceBuilder &builder, const XMLHelper &helper);

        void invalidate();

        QVariant getVariant();

        qint64 getValue();
        void setValue(qint64 value);

        Jgv::Interface *interface();
        const Jgv::Interface *constInterface() const;

    private:
        Q_DECLARE_PRIVATE(Object)

    }; // Interface

} // MaskedIntReg

#endif // MASKEDINTREG_H
