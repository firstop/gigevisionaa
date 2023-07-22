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

#ifndef XMLHELPER_H
#define XMLHELPER_H


#include <QScopedPointer>
#include <QString>

class QDomNodeList;
class QStringList;
class QDomNode;

namespace Node {
enum Type {
    INODE,
    CATEGORY,
    STRINGREG,
    INTEGER,
    INTREG,
    INTCONVERTER,
    INTSWISSKNIFE,
    ENUMERATION,
    ENUMENTRY,
    MASKEDINTREG,
    FLOAT,
    FLOATREG,
    CONVERTER,
    SWISSKNIFE,
    COMMAND,
    BOOLEAN,
    REGISTER
};
}

namespace Attributes {
const QString Name = "Name";
const QString NameSpace = "NameSpace";
}

class XMLHelperPrivate;
class XMLHelper
{


public:
    XMLHelper(const QDomNode &node);
    ~XMLHelper();

    bool isValid() const;

    Node::Type getType() const;

    QString getProperty(const QString &propertyName) const;
    QString nameAttribut() const;
    QString namespaceAttribut() const;

    QStringList getProperties(const QString &propertyName) const;
    QString getChildNameAttribute(const QString &tagName, const QString &textValue) const;
    QString getChildAttributeValue(const QString &childName, const QString &attributeName) const;
    QDomNodeList getChildrenNode(const QString &sType) const;

private:
    QScopedPointer<XMLHelperPrivate> d;


}; // XMLHelper



#endif // XMLHELPER_H
