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

#include "intswissknife.h"
#include "intswissknife_p.h"
#include "formula.h"
#include "genicamobjectbuilder.h"
#include "xmlhelper.h"
#include "genicam.h"

#include <QStringList>
#include <QVariant>

using namespace JgvIntSwissknife;

Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInode::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);

    JgvInode::Object::prepare(builder, helper);
    d->sType = sType;

    QScopedPointer<Formula> pFormula(new Formula(helper.getProperty(Properties::Formula)));
    d->formula.swap(pFormula);

    foreach (QString var, helper.getProperties(Properties::pVariable)) {
        // obtient l'attribut name
        QString nameAttribute = helper.getChildNameAttribute(Properties::pVariable, var);
        if (!nameAttribute.isEmpty()) {
            d->pVariables.insert(nameAttribute, builder.buildInode(var));
        }
    }
}

void Object::invalidate()
{}

bool Object::isWritable() const
{
    return false;
}

QVariant Object::getVariant()
{
    Q_D(const Object);

    if (!d->namespaceIsCustom) {
        return GenICam::Representation::toString(featureName(), getValue());
    }

    return QVariant(getValue());
}

qint64 Object::getValue()
{
    Q_D(Object);

    QMap<QByteArray, qint64> variables;
    QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator it = d->pVariables.constBegin();
    while (it != d->pVariables.constEnd()) {
        QSharedPointer<JgvInode::Object> p = (*it).toStrongRef();
        if (p) {
            if (JGV_ITYPE(p) == Jgv::IInteger) {
                variables.insert(it.key().toAscii(), JGV_IINTEGER(p)->getValue());
            }
            else {
                d->warnForBadInterface("IntSwissKnife getValue()", p, "IInteger");
            }
        }

        ++it;
    }

    qint64 result = d->formula->evaluateAsInteger(variables, QMap<QByteArray, double>());
    //    d->formula->debugOutput("IntSwissKnife " + featureName());
    //    qDebug("RESULT : %lld", result);
    return result;
}

void Object::setValue(qint64 value)
{
    Q_UNUSED(value)
    qWarning("IntSwissknife %s setValue : Not implemented (no sense) !", qPrintable(featureName()));
}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}

