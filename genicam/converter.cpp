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
#include "converter.h"
#include "converter_p.h"
#include "ifloat.h"
#include "iinteger.h"
#include "genicamobjectbuilder.h"
#include "formula.h"
#include "xmlhelper.h"
#include "genicam.h"

#include <limits>
#include <QMap>
#include <QStringList>
#include <QtEndian>
#include <QVariant>

using namespace JgvConverter;


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

    QStringList vars = helper.getProperties(Properties::pVariable);

    foreach (QString var, vars) {
        // obtient l'attribut name
        QString nameAttribute = helper.getChildNameAttribute(Properties::pVariable, var);
        if (!nameAttribute.isEmpty()) {
            QWeakPointer<JgvInode::Object> ii = builder.buildInode(var);
            QSharedPointer<JgvInode::Object> inode = ii.toStrongRef();
            if (inode) {
                d->pVariables.insert(nameAttribute, ii);
            }

        }
    }

    const QString slope = helper.getProperty(Properties::Slope);
    if (slope.startsWith(Slope::AUTOMATIC)) d->slope = Slope::Automatic;
    else if (slope.startsWith(Slope::VARYING)) d->slope = Slope::Varying;
    else if (slope.startsWith(Slope::DECREASING)) d->slope = Slope::Decreasing;
    else (d->slope = Slope::Increasing);
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
    if (p) {
        return p->isWritable();
    }
    return true;
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



double Object::getValue()
{
    Q_D(Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        // distribue les valeurs des variables
        QMap<QByteArray, qint64> iVariables;
        QMap<QByteArray, double> dVariables;
        QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator vit = d->pVariables.constBegin();
        while (vit != d->pVariables.constEnd()) {
            QSharedPointer<JgvInode::Object> p = (*vit).toStrongRef();
            if (p) {
                if (JGV_ITYPE(p) == Jgv::IInteger) {
                    iVariables.insert(vit.key().toAscii(), JGV_IINTEGER(p)->getValue());
                }
                else if (JGV_ITYPE(p) == Jgv::IFloat) {
                    dVariables.insert(vit.key().toAscii(), JGV_IFLOAT(p)->getValue());
                }
                else {
                    d->warnForBadInterface("Converter getValue() pVariable", p, "IInteger or IFloat");
                }
            }
            ++vit;
        }

        if (JGV_ITYPE(p) == Jgv::IInteger) {
            // obtient la valeur à convertir
            iVariables.insert("TO", JGV_IINTEGER(p)->getValue());
            double result = d->formulaFrom->evaluateAsDouble(iVariables, dVariables);;
            //            d->formulaFrom->debugOutput("Converter " + featureName());
            //            qDebug("RESULT : %f", result);
            return result;

        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            // obtient la valeur à convertir
            dVariables.insert("TO", JGV_IFLOAT(p)->getValue());
            double result = d->formulaFrom->evaluateAsDouble(iVariables, dVariables);
            //            d->formulaFrom->debugOutput("Converter " + featureName());
            //            qDebug("RESULT : %f", result);
            return result;
        }
        else {
            d->warnForBadInterface("Converter getValue() pValue", p, "IInteger or IFloat");
        }
    }
    return 0.;
}

void Object::setValue(double value)
{
    Q_D(Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        // distribue les valeurs des variables
        QMap<QByteArray, qint64> iVariables;
        QMap<QByteArray, double> dVariables;
        QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator vit = d->pVariables.constBegin();
        while (vit != d->pVariables.constEnd()) {
            QSharedPointer<JgvInode::Object> p = (*vit).toStrongRef();
            if (p) {
                if (JGV_ITYPE(p) == Jgv::IInteger) {
                    iVariables.insert(vit.key().toAscii(), JGV_IINTEGER(p)->getValue());
                }
                else if (JGV_ITYPE(p) == Jgv::IFloat) {
                    dVariables.insert(vit.key().toAscii(), JGV_IFLOAT(p)->getValue());
                }
                else {
                    d->warnForBadInterface("Converter setValue() pVariable", p, "IInteger or IFloat");
                }
            }
            ++vit;
        }
        // ajoute la valeur aux variables
        dVariables.insert("FROM", value);

        double result = d->formulaTo->evaluateAsDouble(iVariables, dVariables);
        //            d->formulaTo->debugOutput("Converter " + featureName());
        //            qDebug("set RESULT : %f",result);

        if (JGV_ITYPE(p) == Jgv::IInteger) {
            JGV_IINTEGER(p)->setValue(qRound64(result));
        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            JGV_IFLOAT(p)->setValue(result);
        }
        else {
            d->warnForBadInterface("Converter setValue() pValue", p, "IInteger or IFloat");
        }
    }
}

double Object::getMin() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        // distribue les valeurs des variables
        QMap<QByteArray, qint64> iVariables;
        QMap<QByteArray, double> dVariables;
        QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator vit = d->pVariables.constBegin();
        while (vit != d->pVariables.constEnd()) {
            QSharedPointer<JgvInode::Object> p = (*vit).toStrongRef();
            if (p) {
                if (JGV_ITYPE(p) == Jgv::IInteger) {
                    iVariables.insert(vit.key().toAscii(), JGV_IINTEGER(p)->getValue());
                }
                else if (JGV_ITYPE(p) == Jgv::IFloat) {
                    dVariables.insert(vit.key().toAscii(), JGV_IFLOAT(p)->getValue());
                }
                else {
                    qWarning("Converter %s pVariable %s have not IInteger or IFloat interface, found %s",
                             qPrintable(featureName()),
                             qPrintable(vit.key()),
                             qPrintable(p->constInterface()->interfaceTypeString()));
                }
            }
            ++vit;
        }

        if (JGV_ITYPE(p) == Jgv::IInteger) {
            // obtient la valeur à convertir
            const qint64 to = (d->slope == Slope::Decreasing)
                    ? JGV_IINTEGER(p)->getMax()
                    : JGV_IINTEGER(p)->getMin();

            iVariables.insert("TO", to);
            double result = d->formulaFrom->evaluateAsDouble(iVariables, dVariables);
            //            d->formulaFrom->debugOutput("Converter " + featureName());
            //            qDebug("RESULT : %f", result);
            return result;

        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            // obtient la valeur à convertir
            // obtient la valeur à convertir
            const qint64 to = (d->slope == Slope::Decreasing)
                    ? JGV_IFLOAT(p)->getMax()
                    : JGV_IFLOAT(p)->getMin();

            dVariables.insert("TO", to);
            double result = d->formulaFrom->evaluateAsDouble(iVariables, dVariables);
            //            d->formulaFrom->debugOutput("Converter " + featureName());
            //            qDebug("RESULT : %f", result);
            return result;
        }
    }
    return (d->slope == Slope::Decreasing)
            ? std::numeric_limits<double>::max()
            : std::numeric_limits<double>::min();
}

double Object::getMax() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        // distribue les valeurs des variables
        QMap<QByteArray, qint64> iVariables;
        QMap<QByteArray, double> dVariables;
        QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator vit = d->pVariables.constBegin();
        while (vit != d->pVariables.constEnd()) {
            QSharedPointer<JgvInode::Object> p = (*vit).toStrongRef();
            if (p) {
                if (JGV_ITYPE(p) == Jgv::IInteger) {
                    iVariables.insert(vit.key().toAscii(), JGV_IINTEGER(p)->getValue());
                }
                else if (JGV_ITYPE(p) == Jgv::IFloat) {
                    dVariables.insert(vit.key().toAscii(), JGV_IFLOAT(p)->getValue());
                }
                else {
                    qWarning("Converter %s pVariable %s have not IInteger or IFloat interface, found %s",
                             qPrintable(featureName()),
                             qPrintable(vit.key()),
                             qPrintable(p->constInterface()->interfaceTypeString()));
                }
            }
            ++vit;
        }

        if (JGV_ITYPE(p) == Jgv::IInteger) {
            // obtient la valeur à convertir
            // obtient la valeur à convertir
            const qint64 to = (d->slope == Slope::Decreasing)
                    ? JGV_IINTEGER(p)->getMin()
                    : JGV_IINTEGER(p)->getMax();

            iVariables.insert("TO", to);
            double result = d->formulaFrom->evaluateAsDouble(iVariables, dVariables);;
            //            d->formulaFrom->debugOutput("Converter " + featureName());
            //            qDebug("RESULT : %f", result);
            return result;

        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            // obtient la valeur à convertir
            const qint64 to = (d->slope == Slope::Decreasing)
                    ? JGV_IFLOAT(p)->getMin()
                    : JGV_IFLOAT(p)->getMax();

            dVariables.insert("TO", to);
            double result = d->formulaFrom->evaluateAsDouble(iVariables, dVariables);
            //            d->formulaFrom->debugOutput("Converter " + featureName());
            //            qDebug("RESULT : %f", result);
            return result;
        }
    }
    return (d->slope == Slope::Decreasing)
            ? std::numeric_limits<double>::min()
            : std::numeric_limits<double>::max();
}

double Object::getInc() const
{
    Q_D(const Object);

    QSharedPointer<JgvInode::Object> p = d->pValue.toStrongRef();
    if (p) {
        // distribue les valeurs des variables
        QMap<QByteArray, qint64> iVariables;
        QMap<QByteArray, double> dVariables;
        QMap<QString, QWeakPointer<JgvInode::Object> >::const_iterator vit = d->pVariables.constBegin();
        while (vit != d->pVariables.constEnd()) {
            QSharedPointer<JgvInode::Object> p = (*vit).toStrongRef();
            if (p) {
                if (JGV_ITYPE(p) == Jgv::IInteger) {
                    iVariables.insert(vit.key().toAscii(), JGV_IINTEGER(p)->getValue());
                }
                else if (JGV_ITYPE(p) == Jgv::IFloat) {
                    dVariables.insert(vit.key().toAscii(), JGV_IFLOAT(p)->getValue());
                }
                else {
                    qWarning("Converter %s pVariable %s have not IInteger or IFloat interface, found %s",
                             qPrintable(featureName()),
                             qPrintable(vit.key()),
                             qPrintable(p->constInterface()->interfaceTypeString()));
                }
            }
            ++vit;
        }

        if (JGV_ITYPE(p) == Jgv::IInteger) {
            // obtient la valeur à convertir
            const qint64 to = JGV_IINTEGER(p)->getInc();
            iVariables.insert("TO", to);
            double result = d->formulaFrom->evaluateAsDouble(iVariables, dVariables);;
            //            d->formulaFrom->debugOutput("Converter " + featureName());
            //            qDebug("RESULT : %f", result);
            return result;

        }
        else if (JGV_ITYPE(p) == Jgv::IFloat) {
            // obtient la valeur à convertir
            const qint64 to = JGV_IFLOAT(p)->getInc();
            dVariables.insert("TO", to);
            double result = d->formulaFrom->evaluateAsDouble(iVariables, dVariables);
            //            d->formulaFrom->debugOutput("Converter " + featureName());
            //            qDebug("RESULT : %f", result);
            return result;
        }
    }
    return 0.;
}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}

