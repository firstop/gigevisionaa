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
#ifndef FORMULA_H
#define FORMULA_H

#include <QScopedPointer>
#include <QString>

template <class T, class U>
class QMap;
namespace Token {class Object;}
class QByteArray;

class FormulaPrivate;
class Formula
{
public:
    Formula(const QString &formula);
    ~Formula();

    qint64 evaluateAsInteger(const QMap<QByteArray, qint64> &intVariables, const QMap<QByteArray, double> &floatVariables);
    double evaluateAsDouble(const QMap<QByteArray, qint64> &intVariables, const QMap<QByteArray, double> &floatVariables);
    void debugOutput(const QString &additionnalInfos) const;

protected:
    const QScopedPointer<FormulaPrivate> d;

private:
    //Q_DISABLE_COPY(Formula)

}; // Formula


#endif // FORMULA_H
