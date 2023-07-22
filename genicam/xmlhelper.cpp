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

#include "xmlhelper.h"
#include "xmlhelper_p.h"

#include "category.h"
#include "stringreg.h"
#include "integer.h"
#include "intreg.h"
#include "intconverter.h"
#include "intswissknife.h"
#include "enumeration.h"
#include "enumentry.h"
#include "maskedintreg.h"
#include "jgvfloat.h"
#include "floatreg.h"
#include "converter.h"
#include "swissknife.h"
#include "command.h"
#include "boolean.h"

#include <QStringList>

XMLHelperPrivate::XMLHelperPrivate(const QDomNode &node)
    : node(node)
{
    valid = extractAttributes();
}

bool XMLHelperPrivate::extractAttributes()
{
    if (node.isNull()) {
        name =  QObject::trUtf8("Noeud nul");
        return false;
    }
    QDomElement e = node.toElement();
    if (e.isNull()) {
        name = QObject::trUtf8("Noeud non élement");
        return false;
    }
    name = e.attribute(Attributes::Name, "Noeud sans attribut nom");
    nameSpace = e.attribute(Attributes::NameSpace, "Neud sans namespace");
    return true;
}

XMLHelper::XMLHelper(const QDomNode &node)
    : d(new XMLHelperPrivate(node))
{}

XMLHelper::~XMLHelper()
{}

bool XMLHelper::isValid() const
{
    return d->valid;
}

Node::Type XMLHelper::getType() const
{
    Node::Type value = Node::CATEGORY;
    QString type = d->node.nodeName();

    if (type == JgvCategory::sType) value = Node::CATEGORY;
    else if (type == JgvStringReg::sType) value = Node::STRINGREG;
    else if (type == JgvInteger::sType) value = Node::INTEGER;
    else if (type == JgvIntReg::sType) value = Node::INTREG;
    else if (type == JgvIntConverter::sType) value = Node::INTCONVERTER;
    else if (type == JgvIntSwissknife::sType) value = Node::INTSWISSKNIFE;
    else if (type == JgvEnumeration::sType) value = Node::ENUMERATION;
    else if (type == JgvEnumentry::sType) value = Node::ENUMENTRY;
    else if (type == JgvMaskedIntReg::sType) value = Node::MASKEDINTREG;
    else if (type == JgvFloat::sType) value = Node::FLOAT;
    else if (type == JgvFloatReg::sType) value = Node::FLOATREG;
    else if (type == JgvConverter::sType) value = Node::CONVERTER;
    else if (type == JgvSwissknife::sType) value = Node::SWISSKNIFE;
    else if (type == JgvCommand::sType) value = Node::COMMAND;
    else if (type == JgvBoolean::sType) value = Node::BOOLEAN;
    else if (type == JgvInodeRegister::sType) value = Node::REGISTER;
    return value;
}

QString XMLHelper::getProperty(const QString &propertyName) const
{
    QString p;
    QDomElement e = d->node.namedItem(propertyName).toElement();
    if (!e.isNull()) {
        QString val = e.attribute(Attributes::Name);
        if (!val.isEmpty()) {
            p = val;
        } else {
            if (e.firstChild().isText()) {
                p = e.text();
            } else {
                //qWarning("%s",qPrintable(QObject::trUtf8("La propriété %0 n'existe pas sur %1").arg(propertyName).arg(getDisplayName())));
            }
        }
    }
    return p;
}

QString XMLHelper::nameAttribut() const
{
    return d->name;
}

QString XMLHelper::namespaceAttribut() const
{
    return d->nameSpace;
}

QStringList XMLHelper::getProperties(const QString &propertyName) const
{
    QStringList properties;
    QDomNode n = d->node.firstChildElement(propertyName);
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
            properties << e.text();

        }
        n = n.nextSiblingElement(propertyName);
    }

    return properties;
}

QString XMLHelper::getChildNameAttribute(const QString &tagName, const QString &textValue) const
{
    QDomElement element = d->node.toElement();
    if (!element.isNull()) {
        QDomElement child = d->node.firstChildElement(tagName);
        while (!child.isNull()) {
            if (child.hasAttribute(Attributes::Name) && (child.text() == textValue)) {
                return child.attribute(Attributes::Name);
            }
            child = child.nextSiblingElement(tagName);
        }
    }
    qWarning("XMLHelper can't find child %s with text %s", qPrintable("tagName"), qPrintable(textValue));
    return QString();
}

QString XMLHelper::getChildAttributeValue(const QString &childName, const QString &attributeName) const
{
    QString value;
    QDomElement element = d->node.firstChildElement(childName);
    if (!element.isNull()) {
        QDomAttr attr =  element.attributeNode(attributeName);
        if (!attr.isNull()) {
            value = attr.value();
        }
    }
    return value;
}

QDomNodeList XMLHelper::getChildrenNode(const QString &sType) const
{
    return d->node.toElement().elementsByTagName(sType);
}



