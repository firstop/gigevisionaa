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

#ifndef TOKEN_H
#define TOKEN_H

//#include <QByteArray>
//#include <QVariant>

namespace Token {
enum Type {
    UNKNOW,
    LEFT_BRACKET, RIGHT_BRACKET,
    ADDITION, SUBSTRACTION, MULTIPLICATION, DIVISION,
    REMAINDER,
    POWER,
    BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT,
    LOGICAL_NOT_EQUAL, LOGICAL_EQUAL, LOGICAL_GREATER, LOGICAL_LESS, LOGICAL_LESS_OR_EQUAL, LOGICAL_GREATER_OR_EQUAL,
    LOGICAL_AND, LOGICAL_OR,
    SHIFT_LEFT, SHIFT_RIGHT,
    TERNARY_QUESTION_MARK, TERNARY_COLON,
    SGN, NEG,
    ATAN, COS, SIN, TAN, ABS, EXP, LN, SQRT, TRUNC, FLOOR, CEIL, ROUND, ASIN, ACOS,
    E, PI,
    INT64, DOUBLE,
    VARIABLE, VARIABLE_INT64, VARIABLE_DOUBLE
};

enum Associativity {
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT
};

struct Info {
    int precedence;
    Associativity associativity;
    int operandsCount;
    QByteArray geniCamToken;
};

static Info INFOS[] = {
    {     0,     LEFT_TO_RIGHT,       1,        "#"     },  // UNKNOW
    {    -1,     LEFT_TO_RIGHT,       0,        "("     },  // LEFT_BRACKET
    {   990,     LEFT_TO_RIGHT,       0,        ")"     },  // RIGHT_BRACKET
    {   100,     LEFT_TO_RIGHT,       2,        "+"     },  // ADDITION
    {   100,     LEFT_TO_RIGHT,       2,        "-"     },  // SUBSTRACTION
    {   110,     LEFT_TO_RIGHT,       2,        "*"     },  // MULTIPLICATION
    {   110,     LEFT_TO_RIGHT,       2,        "/"     },  // DIVISION
    {   110,     LEFT_TO_RIGHT,       2,        "%"     },  // REMAINDER
    {   120,     RIGHT_TO_LEFT,       2,        "**"    },  // POWER
    {    60,     LEFT_TO_RIGHT,       2,        "&"     },  // BITWISE_AND
    {    40,     LEFT_TO_RIGHT,       2,        "|"     },  // BITWISE_OR
    {    50,     LEFT_TO_RIGHT,       2,        "^"     },  // BITWISE_XOR
    {    30,     LEFT_TO_RIGHT,       1,        "~"     },  // BITWISE_NOT
    {    70,     LEFT_TO_RIGHT,       2,        "<>"    },  // LOGICAL_NOT_EQUAL
    {    70,     LEFT_TO_RIGHT,       2,        "="     },  // LOGICAL_EQUAL
    {    80,     LEFT_TO_RIGHT,       2,        ">"     },  // LOGICAL_GREATER
    {    80,     LEFT_TO_RIGHT,       2,        "<"     },  // LOGICAL_LESS
    {    80,     LEFT_TO_RIGHT,       2,        "<="    },  // LOGICAL_LESS_OR_EQUAL
    {    80,     LEFT_TO_RIGHT,       2,        ">="    },  // LOGICAL_GREATER_OR_EQUAL
    {    20,     LEFT_TO_RIGHT,       2,        "&&"    },  // LOGICAL_AND
    {    10,     LEFT_TO_RIGHT,       2,        "||"    },  // LOGICAL_OR
    {    90,     LEFT_TO_RIGHT,       2,        "<<"    },  // SHIFT_LEFT
    {    90,     LEFT_TO_RIGHT,       2,        ">>"    },  // SHIFT_RIGHT
    {     5,     RIGHT_TO_LEFT,       3,        "?"     },  // TERNARY_QUESTION_MARK
    {     5,     RIGHT_TO_LEFT,       1,        ":"     },  // TERNARY_COLON
    {   200,     LEFT_TO_RIGHT,       1,        "SGN"   },  // SGN
    {   200,     LEFT_TO_RIGHT,       1,        "NEG"   },  // NEG
    {   200,     LEFT_TO_RIGHT,       1,        "ATAN"  },  // ATAN
    {   200,     LEFT_TO_RIGHT,       1,        "COS"   },  // COS
    {   200,     LEFT_TO_RIGHT,       1,        "SIN"   },  // SIN
    {   200,     LEFT_TO_RIGHT,       1,        "TAN"   },  // TAN
    {   200,     LEFT_TO_RIGHT,       1,        "ABS"   },  // ABS
    {   200,     LEFT_TO_RIGHT,       1,        "EXP"   },  // EXP
    {   200,     LEFT_TO_RIGHT,       1,        "LN"    },  // LN
    {   200,     LEFT_TO_RIGHT,       1,        "SQRT"  },  // SQRT
    {   200,     LEFT_TO_RIGHT,       1,        "TRUNC" },  // TRUNC
    {   200,     LEFT_TO_RIGHT,       1,        "FLOOR" },  // FLOOR
    {   200,     LEFT_TO_RIGHT,       1,        "CEIL"  },  // CEIL
    {   200,     LEFT_TO_RIGHT,       1,        "ROUND" },  // ROUND
    {   200,     LEFT_TO_RIGHT,       1,        "ASIN"  },  // ASIN
    {   200,     LEFT_TO_RIGHT,       1,        "ACOS"  },  // ACOS
    {   200,     LEFT_TO_RIGHT,       0,        "E"     },  // E
    {   200,     LEFT_TO_RIGHT,       0,        "PI"    },  // PI
    {   200,     LEFT_TO_RIGHT,       0,        ""      },  // INT64
    {   200,     LEFT_TO_RIGHT,       0,        ""      },  // DOUBLE
    {   200,     LEFT_TO_RIGHT,       0,        ""      },  // INT64_VARIABLE
    {   200,     LEFT_TO_RIGHT,       0,        ""      }   // DOUBLE_VARIABLE
};

class Object
{
    Token::Type m_type;
    QByteArray m_genicamToken;
    union Value {
        double floatValue;
        qint64 intValue;
    };
    Value m_value;

public:
    Object()
        : m_type(UNKNOW)
    {}

    void setType(Token::Type type, const QByteArray &token = QByteArray()) {
        m_type = type;
        // une valeur sous forme de string décrit le Token
        if (!token.isEmpty()) {
            m_genicamToken = token;
            if (type == Token::INT64) {
                bool ok;
                m_value.intValue = m_genicamToken.startsWith("0x")
                        ? m_genicamToken.toLongLong(&ok, 16)
                        : m_genicamToken.toLongLong(&ok, 10);
                if (!ok) {
                    // certaines valeurs peuvent être en notation scientifique
                    m_value.intValue = static_cast<qint64>(m_genicamToken.toDouble(&ok));

                }
                if (!ok) {
                    qWarning("Conversion Token to QINT64 failed : %s", qPrintable(token));
                }
            }
            else if (type == Token::DOUBLE) {
                bool ok;
                m_value.floatValue = m_genicamToken.toDouble(&ok);
                if (!ok) {
                    qWarning("Conversion Token to DOUBLE failed : %s", qPrintable(token));
                }
            }
        }
    }

    Token::Type getType() const {
        return m_type;
    }


    QByteArray getGenicamToken() const {
        return isNumber() ? m_genicamToken : INFOS[m_type].geniCamToken;
    }

    int getSize() const {
        if (m_type <= Token::PI) {
            return INFOS[m_type].geniCamToken.size();
        }
        else {
            return m_genicamToken.size();
        }
    }


    bool isNumber() const {
        return (m_type >= Token::E);
    }

    bool isVariable() const {
        return (m_type >=Token::VARIABLE);
    }

    bool isINT64() const {
        return (m_type==Token::INT64 || m_type==Token::VARIABLE_INT64);
    }

    bool isDOUBLE() const {
        return (m_type==Token::DOUBLE|| m_type==Token::VARIABLE_DOUBLE);
    }

    bool isOperator() const {
        return (m_type >= Token::ADDITION) && (m_type <= Token::ACOS);
    }

    bool isLeftParenthesis() const {
        return (m_type == LEFT_BRACKET);
    }

    bool isRightParenthesis() const {
        return (m_type == RIGHT_BRACKET);
    }

    bool isLeftAssociative() const {
        return (INFOS[m_type].associativity == LEFT_TO_RIGHT);
    }

    int operandsCount() const {
        return INFOS[m_type].operandsCount;
    }

    int precedence() const {
        return INFOS[m_type].precedence;
    }

    void setFloat(double value) {
        // si variable non typée, on type
        if (m_type == Token::VARIABLE) {
            m_type = Token::VARIABLE_DOUBLE;
        }
        m_value.floatValue = value;
    }

    double toFloat() const {
        return isDOUBLE() ? m_value.floatValue : static_cast<double>(m_value.intValue);
    }
    void setInteger(qint64 value) {
        // si variable non typée, on type
        if (m_type == Token::VARIABLE) {
            m_type = Token::VARIABLE_INT64;
        }
        m_value.intValue = value;
    }

    qint64 toInteger() const {
        return isINT64() ? m_value.intValue : static_cast<qint64>(m_value.floatValue);
    }


};
}



#endif // TOKEN_H
