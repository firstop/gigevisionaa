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

#include "genicamobjectbuilder.h"
#include "genicamobjectbuilder_p.h"
#include "genicamxmlfile.h"
#include "inode.h"
#include "xmlhelper.h"
#include "category.h"
#include "stringreg.h"
#include "boolean.h"
#include "integer.h"
#include "intreg.h"
#include "enumeration.h"
#include "maskedintreg.h"
#include "jgvfloat.h"
#include "floatreg.h"
#include "converter.h"
#include "intconverter.h"
#include "intswissknife.h"
#include "swissknife.h"
#include "command.h"
#include "register.h"
#include "iport.h"

#include <QDomNode>
#include <QDebug>

InterfaceBuilderPrivate::InterfaceBuilderPrivate(QSharedPointer<JgvIport::Interface> iport)
    : iport(iport)
{}

InterfaceBuilderPrivate::~InterfaceBuilderPrivate()
{}

InterfaceBuilder::InterfaceBuilder(const QString &fileName, const QByteArray &file, QSharedPointer<JgvIport::Interface> iport)
    : d(new InterfaceBuilderPrivate(iport))
{
    d->xmlFile.setFileName(fileName);
    if (fileName.endsWith(".zip")) {
        d->xmlFile.setZippedContent(file);
    } else {
       d->xmlFile.setContent(file);
    }
}

InterfaceBuilder::~InterfaceBuilder()
{}

QWeakPointer<JgvInode::Object> InterfaceBuilder::buildInode(const QString &name)
{
    QSharedPointer<JgvInode::Object> inode = d->inodes.value(name);
    if (inode.isNull()) {
        XMLHelper helper(d->xmlFile.getRootChildNodeByNameAttribute(name));
        if (helper.isValid()) {
            inode = buildInode(helper);
        }
    }
    return inode.toWeakRef();
}

QHash<QString, QSharedPointer<JgvInode::Object> > InterfaceBuilder::allReferencedInodes()
{
    return d->inodes;
}


/*!
 * \brief InterfaceBuilder::buildInode
 * \param helper
 * \return
 */

QSharedPointer<JgvInode::Object> InterfaceBuilder::buildInode(const XMLHelper &helper)
{
    QSharedPointer<JgvInode::Object> inode;

    // Attention, lors de l'initialisation de l'inode,
    // celui-ci peut appeler un nouvel inode manipulant le pointeur
    // de l'appelant, celui-ci n'étant pas initialiser.
    // Il faut donc absolument inserer dans la map l'inode avant de l'initialiser.
    // On évite ainsi les double instances.
    switch (helper.getType()) {
    case Node::CATEGORY: {
        QSharedPointer<JgvCategory::Object> category(new JgvCategory::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), category);
        category->prepare(*this, helper);
        inode = category;
        break;
    }
    case Node::STRINGREG: {
        QSharedPointer<JgvStringReg::Object> stringreg(new JgvStringReg::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), stringreg);
        stringreg->prepare(*this, helper);
        inode = stringreg;
        break;
    }
    case Node::BOOLEAN: {
        QSharedPointer<JgvBoolean::Object> boolean(new JgvBoolean::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), boolean);
        boolean->prepare(*this, helper);
        inode = boolean;
        break;
    }
    case Node::INTEGER: {
        QSharedPointer<JgvInteger::Object> integer(new JgvInteger::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), integer);
        integer->prepare(*this, helper);
        inode = integer;
        break;
    }
    case Node::INTREG: {
        QSharedPointer<JgvIntReg::Object> intreg(new JgvIntReg::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), intreg);
        intreg->prepare(*this, helper);
        inode = intreg;
        break;
    }
    case Node::MASKEDINTREG: {
        QSharedPointer<JgvMaskedIntReg::Object> maskedintreg(new JgvMaskedIntReg::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), maskedintreg);
        maskedintreg->prepare(*this, helper);
        inode = maskedintreg;
        break;
    }
    case Node::ENUMERATION: {
        QSharedPointer<JgvEnumeration::Object> enumeration(new JgvEnumeration::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), enumeration);
        enumeration->prepare(*this, helper);
        inode = enumeration;
        break;
    }
    case Node::FLOAT: {
        QSharedPointer<JgvFloat::Object> fl(new JgvFloat::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), fl);
        fl->prepare(*this, helper);
        inode = fl;
        break;
    }
    case Node::FLOATREG: {
        QSharedPointer<JgvFloatReg::Object> floatreg(new JgvFloatReg::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), floatreg);
        floatreg->prepare(*this, helper);
        inode = floatreg;
        break;
    }
    case Node::CONVERTER: {
        QSharedPointer<JgvConverter::Object> converter(new JgvConverter::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), converter);
        converter->prepare(*this, helper);
        inode = converter;
        break;
    }
    case Node::INTCONVERTER: {
        QSharedPointer<JgvIntConverter::Object> intconverter(new JgvIntConverter::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), intconverter);
        intconverter->prepare(*this, helper);
        inode = intconverter;
        break;
    }
    case Node::INTSWISSKNIFE: {
        QSharedPointer<JgvIntSwissknife::Object> intswissknife(new JgvIntSwissknife::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), intswissknife);
        intswissknife->prepare(*this, helper);
        inode = intswissknife;
        break;
    }
    case Node::SWISSKNIFE: {
        QSharedPointer<JgvSwissknife::Object> swissknife(new JgvSwissknife::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), swissknife);
        swissknife->prepare(*this, helper);
        inode = swissknife;
        break;
    }
    case Node::COMMAND: {
        QSharedPointer<JgvCommand::Object> command(new JgvCommand::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), command);
        command->prepare(*this, helper);
        inode = command;
        break;
    }
    case Node::REGISTER: {
        QSharedPointer<JgvRegister::Object> reg(new JgvRegister::Object(d->iport));
        d->inodes.insert(helper.nameAttribut(), reg);
        reg->prepare(*this, helper);
        inode = reg;
        break;
    }
    default:
        qDebug() << helper.getType();
        qWarning("Warning InterfaceBuilder::buildInode(const XMLHelper &helper)");
        break;
    }
    return inode;
}




