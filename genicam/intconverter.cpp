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

#include "intconverter.h"
#include "intconverter_p.h"
#include "formula.h"
#include "genicamobjectbuilder.h"
#include "ifloat.h"
#include "xmlhelper.h"
#include "genicam.h"

#include <QStringList>
#include <QVariant>

#include <QDebug>

using namespace JgvIntConverter;


Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInode::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);
    JgvInode::Object::prepare(builder, helper);
    d->sType = sType;

    QScopedPointer<Formula> pFrom(new Formula(helper.getProperty(Properties::FormulaFrom)));
    d->formulaFrom.swap(pFrom);
    QScopedPointer<Formula> pTo(new Formula(helper.getProperty(Properties::FormulaTo)));
    d->formulaTo.swap(pTo);

    d->pValue = builder.buildInode(helper.getProperty(Properties::pValue));

    foreach (QString var, helper.getProperties(Properties::pVariable)) {
        // obtient l'attribut name
        QString nameAttribute = helper.getChildNameAttribute(Properties::pVariable, var);
        if (!nameAttribute.isEmpty()) {
            d->pVariables.insert(nameAttribute, builder.buildInode(var));
        }
    }
}

void Object::invalidate()
{
    Q_D(Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        p->invalidate();
    }
}

bool Object::isWritable() const
{
    Q_D(const Object);

    if (!d->imposedCanWrite) {
        return false;
    }

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    return (p) ? p->isWritable() : true;
}

QVariant Object::getVariant()
{
    Q_D(const Object);
    if (!d->namespaceIsCustom) {
        return GenICam::Representation::toString(featureName(), getValue());
    }

    return QVariant(getValue());
}

int Object::getPollingTime() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        return p->getPollingTime();
    }
    return -1;
}



qint64 Object::getValue()
{
    Q_D(Object);

    QMap<QByteArray, qint64> iVariables;
    QMap<QByteArray, double> dVariables;
    QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator it = d->pVariables.constBegin();
    while (it != d->pVariables.constEnd()) {
        QSharedPointer<JgvInode::Object> p = (*it).toStrongRef();
        if (p) {
            if (JGV_ITYPE(p) == Jgv::IInteger) {
                iVariables.insert(it.key().toAscii(), JGV_IINTEGER(p)->getValue());
            }
            else {
                d->warnForBadInterface("IntConverter getValue()", p, "IInteger");
            }
        }
        ++it;
    }

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            iVariables.insert("TO", JGV_IINTEGER(p)->getValue());
        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            dVariables.insert("TO", JGV_IFLOAT(p)->getValue());
        }
        else {
            d->warnForBadInterface("IntConverter getValue()", p, "IInteger or IFloat");
        }
    }

    const qint64 result =  d->formulaFrom->evaluateAsInteger(iVariables, dVariables);
    //    d->formulaFrom->debugOutput("IntConverter " + featureName());
    //    qDebug("RESULT : %lld", result);
    return result;
}

void Object::setValue(qint64 value)
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
                d->warnForBadInterface("IntConverter setValue()", p, "IInteger");
            }
        }
        ++it;
    }

    variables.insert("FROM", value);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            qint64 result = d->formulaTo->evaluateAsInteger(variables, QMap<QByteArray, double>());
            //            d->formulaTo->debugOutput("IntConverter " + featureName());
            //            qDebug("set RESULT : %lld",result);
            JGV_IINTEGER(p)->setValue(result);

        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            double result = d->formulaTo->evaluateAsDouble(variables, QMap<QByteArray, double>());
            //            d->formulaTo->debugOutput("IntConverter " + featureName());
            //            qDebug("set RESULT : %f",result);
            JGV_IFLOAT(p)->setValue(result);
        }
        else {
            d->warnForBadInterface("IntConverter setValue()", p, "IInteger or IFloat");
        }
    }
}

qint64 Object::getMin() const
{
    Q_D(const Object);

    QMap<QByteArray, qint64> iVariables;
    QMap<QByteArray, double> dVariables;
    QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator it = d->pVariables.constBegin();
    while (it != d->pVariables.constEnd()) {
        QSharedPointer<JgvInode::Object> p = (*it).toStrongRef();
        if (p) {
            if (JGV_ITYPE(p) == Jgv::IInteger) {
                iVariables.insert(it.key().toAscii(), JGV_IINTEGER(p)->getValue());
            }
            else {
                d->warnForBadInterface("IntConverter getValue()", p, "IInteger");
            }
        }
        ++it;
    }

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            iVariables.insert("TO", JGV_IINTEGER(p)->getMin());
        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            dVariables.insert("TO", JGV_IFLOAT(p)->getMin());
        }
        else {
            d->warnForBadInterface("IntConverter getMin()", p, "IInteger or IFloat");
        }
    }

    const qint64 result =  d->formulaFrom->evaluateAsInteger(iVariables, dVariables);
    //    d->formulaFrom->debugOutput("IntConverter " + featureName());
    //    qDebug("RESULT : %lld", result);

    return result;
}

qint64 Object::getMax() const
{
    Q_D(const Object);

    QMap<QByteArray, qint64> iVariables;
    QMap<QByteArray, double> dVariables;
    QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator it = d->pVariables.constBegin();
    while (it != d->pVariables.constEnd()) {
        QSharedPointer<JgvInode::Object> p = (*it).toStrongRef();
        if (p) {
            if (JGV_ITYPE(p) == Jgv::IInteger) {
                iVariables.insert(it.key().toAscii(), JGV_IINTEGER(p)->getValue());
            }
            else {
                d->warnForBadInterface("IntConverter getValue()", p, "IInteger");
            }
        }
        ++it;
    }

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            iVariables.insert("TO", JGV_IINTEGER(p)->getMax());
        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            dVariables.insert("TO", JGV_IFLOAT(p)->getMax());
        }
        else {
            d->warnForBadInterface("IntConverter getMax()", p, "IInteger or IFloat");
        }
    }

    const qint64 result =  d->formulaFrom->evaluateAsInteger(iVariables, dVariables);
    //    d->formulaFrom->debugOutput("IntConverter " + featureName());
    //    qDebug("RESULT : %lld", result);

    return result;
}

qint64 Object::getInc() const
{
    Q_D(const Object);

    QMap<QByteArray, qint64> iVariables;
    QMap<QByteArray, double> dVariables;
    QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator it = d->pVariables.constBegin();
    while (it != d->pVariables.constEnd()) {
        QSharedPointer<JgvInode::Object> p = (*it).toStrongRef();
        if (p) {
            if (JGV_ITYPE(p) == Jgv::IInteger) {
                iVariables.insert(it.key().toAscii(), JGV_IINTEGER(p)->getValue());
            }
            else {
                d->warnForBadInterface("IntConverter getValue()", p, "IInteger");
            }
        }
        ++it;
    }

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        if (JGV_ITYPE(p) == Jgv::IInteger) {
            iVariables.insert("TO", JGV_IINTEGER(p)->getInc());
        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            dVariables.insert("TO", JGV_IFLOAT(p)->getInc());
        }
        else {
            d->warnForBadInterface("IntConverter getInc()", p, "IInteger or IFloat");
        }
    }

    const qint64 result =  d->formulaFrom->evaluateAsInteger(iVariables, dVariables);
    //    d->formulaFrom->debugOutput("IntConverter " + featureName());
    //    qDebug("RESULT : %lld", result);
    return result;
}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}

