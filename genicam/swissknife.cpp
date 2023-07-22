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

#include "swissknife.h"
#include "swissknife_p.h"
#include "iinteger.h"
#include "genicamobjectbuilder.h"
#include "formula.h"
#include "xmlhelper.h"
#include "genicam.h"

#include <QStringList>
#include <QVariant>

using namespace JgvSwissknife;


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

double Object::getValue()
{
    Q_D(Object);

    QMap<QByteArray, qint64> intVariables;
    QMap<QByteArray, double> floatVariables;
    QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator it = d->pVariables.constBegin();
    while (it != d->pVariables.constEnd()) {
        QSharedPointer<JgvInode::Object> p = (*it).toStrongRef();
        if (p) {
            if (JGV_ITYPE(p) == Jgv::IInteger) {
                intVariables.insert(it.key().toAscii(), JGV_IINTEGER(p)->getValue());
            }
            else if (JGV_ITYPE(p) == Jgv::IFloat) {
                floatVariables.insert(it.key().toAscii(), JGV_IFLOAT(p)->getValue());
            }
            else {
                d->warnForBadInterface("SwissKnife setValue()", p, "IInteger or IFloat");
            }
        }
        ++it;
    }

    const double res = d->formula->evaluateAsDouble(intVariables, floatVariables);
    //    d->formula->debugOutput("SwissKnife " + featureName());
    //    qDebug("RESULT : %f", res);
    return res;
}

void Object::setValue(double value)
{}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}



