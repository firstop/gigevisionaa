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
#include "formula.h"
#include "formula_p.h"
#include "token.h"

#include <QQueue>
#include <QStack>
#include <QtCore/qmath.h>
#include <QDebug>

FormulaPrivate::FormulaPrivate(const QString &formula)
    : formula(formula)
{
    QStack<Token::Object> stack;

    int position = 0;
    while (position < formula.size()) {

        Token::Object token = getNextToken(position);
        position += token.getSize();

        // cas nombre
        if (token.isNumber()) {
            output.append(token);
        }

        // cas opérateur
        else if (token.isOperator()) {
            // traitement des autres token GenICam
            while (!stack.isEmpty()             // la pile n'est pas vide
                   && stack.top().isOperator()  // le haut de la pile contient un opérateur
                   && (
                       // associativité gauche droite et priorité égale
                       (token.isLeftAssociative() && predecendenceIsEqual(token, stack.top()))
                       // priorité plus faible
                       || predecendenceIsLess(token, stack.top()))
                   )
            {
                // on dépile la pile vers la sortie
                output.append(stack.pop());
            }

            // le traitement de la pile est fini, on empile le token
            stack.push(token);
        }

        // cas parenthèse gauche
        else if (token.isLeftParenthesis()) {
            stack.push(token);      // parenthèse gauche toujours sur la pile
        }

        // cas parenthèse droite
        else if (token.isRightParenthesis()) {
            bool leftParenthesisFounded = false;

            // on traite la pile
            while (!stack.isEmpty() ) {
                // tant qu'on ne trouve pas une parenthèse gauche, on dépile
                if (!stack.top().isLeftParenthesis()) {
                    // on empile les opérateurs
                    output.append(stack.pop());
                }
                else {
                    // on supprime juste la parenthèse gauche
                    stack.pop();
                    leftParenthesisFounded = true;
                    break;
                }

            }
            if (!leftParenthesisFounded) {
                qWarning("ERROR : Left parenthesis not found on the stack !");
                stack.clear();
                debugOutput("Formula::setFormula (!leftParenthesisFounded)");
                break;
            }
        }
    }



    // tous les tokens on été lu, on pousse la pile vers la sortie
    while (!stack.isEmpty()) {
        // si il reste des parenthèses sur la pile, on a une erreur
        if (stack.top().isLeftParenthesis() || stack.top().isRightParenthesis()) {
            qWarning("ERROR : Parenthesis left on the stack !");
            debugOutput("Formula::setFormula (stack.top().isLeftParenthesis() || stack.top().isRightParenthesis())");
            break;
        }
        else {
            output.append(stack.pop());
        }
    }
}

FormulaPrivate::~FormulaPrivate()
{}

Token::Object FormulaPrivate::getNextToken(int fromPosition) const
{
    Token::Object token;

    const QChar current = formula.at(fromPosition);
    if (current == ' ') {
    }
    else if (current == '(') {
        token.setType(Token::LEFT_BRACKET);
    }
    else if (current == ')') {
        token.setType(Token::RIGHT_BRACKET);
    }
    else if (current == '+') {
        token.setType(Token::ADDITION);
    }
    else if (current == '-') {
        QChar next = formula.at(fromPosition + 1);
        // pour que le token soit une soustraction,
        // il faut qu'il soit suivi d'un blanc
        if (next == ' ') {
            token.setType(Token::SUBSTRACTION);
        }
        // dans le cas contraire on doit avoir un nombre négatif
        // si il est directement suivi d'un digit
        else if (next.isDigit()) {
            QByteArray var = getNumber(fromPosition);
            token.setType(var.contains('.') ? Token::DOUBLE : Token::INT64, var);
        }
    }
    else if (current == '*') {
        if (formula.at(fromPosition + 1) == '*') {
            token.setType(Token::POWER);
        }
        else {
            token.setType(Token::MULTIPLICATION);
        }
    }
    else if (current == '/') {
        token.setType(Token::DIVISION);
    }
    else if (current == '%') {
        token.setType(Token::REMAINDER);
    }
    else if (current == '&') {
        if (formula.at(fromPosition + 1) == '&') {
            token.setType(Token::LOGICAL_AND);
        }
        else {
            token.setType(Token::BITWISE_AND);
        }
    }
    else if (current == '|') {
        if (formula.at(fromPosition + 1) == '|') {
            token.setType(Token::LOGICAL_OR);
        }
        else {
            token.setType(Token::BITWISE_OR);
        }
    }
    else if (current == '^') {
        token.setType(Token::BITWISE_XOR);
    }
    else if (current == '~') {
        token.setType(Token::BITWISE_NOT);
    }
    else if (current == '<') {
        const QChar next = formula.at(fromPosition + 1);
        if (next == '>') {
            token.setType(Token::LOGICAL_NOT_EQUAL);
        }
        else if (next == '=') {
            token.setType(Token::LOGICAL_LESS_OR_EQUAL);
        }
        else if (next == '<') {
            token.setType(Token::SHIFT_LEFT);
        }
        else {
            token.setType(Token::LOGICAL_LESS);
        }
    }
    else if (current == '>') {
        const QChar next = formula.at(fromPosition + 1);
        if (next == '=') {
            token.setType(Token::LOGICAL_GREATER_OR_EQUAL);
        }
        else if (next == '>') {
            token.setType(Token::SHIFT_RIGHT);
        }
        else {
            token.setType(Token::LOGICAL_GREATER);
        }
    }
    else if (current == '=') {
        token.setType(Token::LOGICAL_EQUAL);
    }
    else if (current == '?') {
        token.setType(Token::TERNARY_QUESTION_MARK);
    }
    else if (current == ':') {
        token.setType(Token::TERNARY_COLON);
    }

    // le token est un chiffre
    // on est dans le cas d'une valeur décimale ou hexa
    else if (current.isDigit()) {
        QByteArray var = getNumber(fromPosition);
        token.setType(var.contains('.') ? Token::DOUBLE : Token::INT64, var);
    }
    // le dernier cas possible est d'avoir une variable nommée
    else {
        token = getVariable(fromPosition);
    }

    return token;
}

Token::Object FormulaPrivate::getVariable(int fromPosition) const
{
    // token nul
    Token::Object ret;

    // une variable commence par une lettre
    if (formula.at(fromPosition).isLetter()) {
        int pos = fromPosition + 1;
        while (pos < formula.size()) {
            const QChar c = formula.at(pos);
            // una variable contient lettres, chiffres et underscore
            if (!(c.isLetterOrNumber() || (c == '_')))
            { break;}
            ++pos;
        }
        QString var = formula.mid(fromPosition, pos - fromPosition);
        ret.setType(Token::VARIABLE, var.toAscii());
    }
    return ret;
}

QByteArray FormulaPrivate::getNumber(int fromPosition) const
{
    int index = fromPosition;

    // pour les nombres négatifs
    if (formula.at(index) == '-') {
        ++index;
    }

    while (index < formula.size()) {
        if (formula.at(index).isLetterOrNumber() || formula.at(index) == '.') {
            ++index;
        }
        else {
            break;
        }
    }
    return formula.mid(fromPosition, index - fromPosition).toAscii();
}

bool FormulaPrivate::predecendenceIsEqual(const Token::Object &left, const Token::Object &right) const
{
    return (left.precedence() == right.precedence());
}

bool FormulaPrivate::predecendenceIsLess(const Token::Object &left, const Token::Object &right) const
{
    return (left.precedence() < right.precedence());
}

void FormulaPrivate::debugOutput(const QString &additionnalInfos) const
{
    QString rpn;
    QQueue<Token::Object>::ConstIterator it = output.constBegin();
    while (it != output.constEnd()) {
        QString post;
        if ((*it).isINT64()) {
            post = QString("{%0}").arg((*it).toInteger());
        }
        else if ((*it).isDOUBLE()) {
            post = QString("{%0}").arg((*it).toFloat());
        }
        rpn.append((*it).getGenicamToken() + post + " ");
        ++it;
    }
    qDebug("\n------------ Formula DEBUG ------------");
    qDebug("source : %s", qPrintable(additionnalInfos));
    QMap<QByteArray, double>::const_iterator fit = floatVariables.constBegin();
    while (fit != floatVariables.constEnd()) {
        qWarning("* FLOAT VAR : %s", qPrintable(QString("%0 %1").arg(QString(fit.key())).arg(fit.value())));
        ++fit;
    }
    QMap<QByteArray, qint64>::const_iterator iit = intVariables.constBegin();
    while (iit != intVariables.constEnd()) {
        qWarning("* INT64 VAR : %s", qPrintable(QString("%0 0x%1").arg(QString(iit.key())).arg(iit.value(), 0, 16)));
        ++iit;
    }

    qDebug("infix  : %s", qPrintable(formula));
    qDebug("outfix : %s", qPrintable(rpn));
    qDebug("------------ END DEBUG ------------\n");
}

Formula::Formula(const QString &formula)
    : d(new FormulaPrivate(formula))
{}

Formula::~Formula()
{}


qint64 Formula::evaluateAsInteger(const QMap<QByteArray, qint64> &intVariables, const QMap<QByteArray, double> &floatVariables)
{
    d->intVariables = intVariables;
    d->floatVariables = floatVariables;


    QStack<Token::Object> stack;
    QQueue<Token::Object>::Iterator it = d->output.begin();
    qint64 result = 0;
    qint64 o1, o2, o3;


    while (it != d->output.end()) {
        // variable on renseigne la valeur et on pousse
        if ( (*it).isVariable() ) {
            QByteArray genicamToken = (*it).getGenicamToken();
            if (intVariables.contains(genicamToken)) {
                qint64 value = intVariables.value(genicamToken);
                (*it).setInteger(value);
            }
            // variable int64 on renseigne la valeur et on pousse
            else if (floatVariables.contains(genicamToken)) {
                double value = floatVariables.value(genicamToken);
                (*it).setFloat(value);
            }
            else {
                qCritical("%s",qPrintable(QString("Formula::evaluateAsInteger variable inconnue").arg(QString(genicamToken))));
            }

            stack.push(*it);
        }
        else if ((*it).isNumber()) {
            stack.push(*it);
        }
        else {
            const int args = (*it).operandsCount();
            if (stack.size() < args) {
                qDebug("error evaluate stack size");
                debugOutput("Formula::evaluate() (stack.size() < args)");
                break;
            }

            Token::Object shortEval;
            shortEval.setType(Token::INT64);

            if (args >= 1) {
                o1 = stack.pop().toInteger();
            }
            if (args >= 2) {
                o2 = stack.pop().toInteger();
            }
            if (args == 3) {
                o3 = stack.pop().toInteger();
            }

            switch ((*it).getType()) {
            case Token::ADDITION:
                shortEval.setInteger(o2 + o1);
                break;
            case Token::SUBSTRACTION:
                shortEval.setInteger(o2 - o1);
                break;
            case Token::MULTIPLICATION:
                shortEval.setInteger(o2 * o1);
                break;
            case Token::DIVISION:
                if (o1 != 0) {
                    shortEval.setInteger(o2 / o1);
                }
                break;
            case Token::REMAINDER:
                shortEval.setInteger(o2 % o1);
                break;
            case Token::POWER:
                shortEval.setInteger(qPow(o2, o1));
                break;
            case Token::BITWISE_AND:
                shortEval.setInteger(o2 & o1);
                break;
            case Token::BITWISE_OR:
                shortEval.setInteger(o2 | o1);
                break;
            case Token::BITWISE_XOR:
                shortEval.setInteger(o2 ^ o1);
                break;
            case Token::BITWISE_NOT:
                shortEval.setInteger(~o1);
                break;
            case Token::LOGICAL_NOT_EQUAL:
                shortEval.setInteger(o2 != o1);
                break;
            case Token::LOGICAL_EQUAL:
                shortEval.setInteger(o2 == o1);
                break;
            case Token::LOGICAL_GREATER:
                shortEval.setInteger(o2 > o1);
                break;
            case Token::LOGICAL_LESS:
                shortEval.setInteger(o2 < o1);
                break;
            case Token::LOGICAL_LESS_OR_EQUAL:
                shortEval.setInteger(o2 <= o1);
                break;
            case Token::LOGICAL_GREATER_OR_EQUAL:
                shortEval.setInteger(o2 >= o1);
                break;
            case Token::LOGICAL_AND:
                shortEval.setInteger(o2 && o1);
                break;
            case Token::LOGICAL_OR:
                shortEval.setInteger(o2 || o1);
                break;
            case Token::SHIFT_LEFT:
                shortEval.setInteger(o2 << o1);
                break;
            case Token::SHIFT_RIGHT:
                shortEval.setInteger(o2 >> o1);
                break;
            case Token::TERNARY_QUESTION_MARK:
                shortEval.setInteger(o3 ? o2 : o1);
                break;
            case Token::TERNARY_COLON:
                shortEval.setInteger(o1);
                break;
            }

            stack.push(shortEval);

        }

        ++it;
    }
    if (stack.size() == 1) {
        result = stack.pop().toFloat();
    }
    else {
        qDebug("error evaluate as integer result stack");
    }

    return result;
}

double Formula::evaluateAsDouble(const QMap<QByteArray, qint64> &intVariables, const QMap<QByteArray, double> &floatVariables)
{
    d->intVariables = intVariables;
    d->floatVariables = floatVariables;

    QStack<Token::Object> stack;
    QQueue<Token::Object>::Iterator it = d->output.begin();
    bool ok = true;
    double result = 0;
    double o1, o2, o3;

    while (it != d->output.end()) {
        // variable on renseigne la valeur, on change le type de token et on pousse
        if ( (*it).isVariable() ) {
            QByteArray genicamToken = (*it).getGenicamToken();
            if (intVariables.contains(genicamToken)) {
                qint64 value = intVariables.value(genicamToken);
                (*it).setInteger(value);

            }
            // variable int64 on renseigne la valeur et on pousse
            else if (floatVariables.contains(genicamToken)) {
                double value = floatVariables.value(genicamToken );
                (*it).setFloat(value);
            }
            else {
                qCritical("%s",qPrintable(QString("Formula::evaluateAsDouble variable inconnue").arg(QString(genicamToken))));
            }

            stack.push(*it);
        }
        else if ((*it).isNumber()) {
            stack.push(*it);
        }
        else {
            const int args = (*it).operandsCount();
            if (stack.size() < args) {
                debugOutput("Formula::evaluateAsDouble()");
                break;
            }

            Token::Object shortEval;
            shortEval.setType(Token::DOUBLE);

            if (args >= 1) {
                o1 = stack.pop().toFloat();
            }
            if (args >= 2) {
                o2 = stack.pop().toFloat();
            }
            if (args == 3) {
                o3 = stack.pop().toFloat();
            }

            switch ((*it).getType()) {
            case Token::ADDITION:
                shortEval.setFloat(o2 + o1);
                break;
            case Token::SUBSTRACTION:
                shortEval.setFloat(o2 - o1);
                break;
            case Token::MULTIPLICATION:
                shortEval.setFloat(o2 * o1);
                break;
            case Token::DIVISION:
                if (o1 != 0.) {
                    shortEval.setFloat(o2 / o1);
                }
                break;
            case Token::REMAINDER:
                qWarning("REMAINDER on DOUBLE");
                ok = false;
                break;
            case Token::POWER:
                shortEval.setFloat(qPow(o2, o1));
                break;
            case Token::BITWISE_AND:
                qWarning("BITWISE_AND on DOUBLE");
                ok = false;
                break;
            case Token::BITWISE_OR:
                qWarning("BITWISE_OR on DOUBLE");
                ok = false;
                break;
            case Token::BITWISE_XOR:
                qWarning("BITWISE_XOR on DOUBLE");
                ok = false;
                break;
            case Token::BITWISE_NOT:
                qWarning("BITWISE_NOT on DOUBLE");
                ok = false;
                break;
            case Token::LOGICAL_NOT_EQUAL:
                shortEval.setFloat(o2 != o1);
                break;
            case Token::LOGICAL_EQUAL:
                shortEval.setFloat(o2 == o1);
                break;
            case Token::LOGICAL_GREATER:
                shortEval.setFloat(o2 > o1);
                break;
            case Token::LOGICAL_LESS:
                shortEval.setFloat(o2 < o1);
                break;
            case Token::LOGICAL_LESS_OR_EQUAL:
                shortEval.setFloat(o2 <= o1);
                break;
            case Token::LOGICAL_GREATER_OR_EQUAL:
                shortEval.setFloat(o2 >= o1);
                break;
            case Token::LOGICAL_AND:
                shortEval.setFloat(o2 && o1);
                break;
            case Token::LOGICAL_OR:
                shortEval.setFloat(o2 || o1);
                break;
            case Token::SHIFT_LEFT:
                qWarning("SHIFT_LEFT on DOUBLE");
                ok = false;
                break;
            case Token::SHIFT_RIGHT:
                qWarning("SHIFT_LEFT on DOUBLE");
                ok = false;
                break;
            case Token::TERNARY_QUESTION_MARK:
                shortEval.setFloat(o3 ? o2 : o1);
                break;
            case Token::TERNARY_COLON:
                shortEval.setFloat(o1);
                break;
            }
            if (ok) {
                stack.push(shortEval);
            }
            else {
                debugOutput("Evaluate");
                break;
            }
        }

        ++it;
    }
    if (stack.size() == 1) {
        result = stack.pop().toFloat();
    }
    else {
        qDebug("error evaluate as double result stack");
    }

    return result;
}

void Formula::debugOutput(const QString &additionnalInfos) const
{
    d->debugOutput(additionnalInfos);
}





