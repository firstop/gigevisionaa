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

#include "maskedintreg.h"
#include "maskedintreg_p.h"
#include "xmlhelper.h"
#include "genicam.h"

#include <QVariant>
#include <QtEndian>
#include <limits>


using namespace JgvMaskedIntReg;

const quint64 MASK[65] = {
    0x0000000000000000,
    0x0000000000000001,0x0000000000000003,0x0000000000000007,0x000000000000000F,0x000000000000001F,0x000000000000003F,0x000000000000007F,0x00000000000000FF,
    0x00000000000001FF,0x00000000000003FF,0x00000000000007FF,0x0000000000000FFF,0x0000000000001FFF,0x0000000000003FFF,0x0000000000007FFF,0x000000000000FFFF,
    0x000000000001FFFF,0x000000000003FFFF,0x000000000007FFFF,0x00000000000FFFFF,0x00000000001FFFFF,0x00000000003FFFFF,0x00000000007FFFFF,0x0000000000FFFFFF,
    0x0000000001FFFFFF,0x0000000003FFFFFF,0x0000000007FFFFFF,0x000000000FFFFFFF,0x000000001FFFFFFF,0x000000003FFFFFFF,0x000000007FFFFFFF,0x00000000FFFFFFFF,
    0x00000001FFFFFFFF,0x00000003FFFFFFFF,0x00000007FFFFFFFF,0x0000000FFFFFFFFF,0x0000001FFFFFFFFF,0x0000003FFFFFFFFF,0x0000007FFFFFFFFF,0x000000FFFFFFFFFF,
    0x000001FFFFFFFFFF,0x000003FFFFFFFFFF,0x000007FFFFFFFFFF,0x00000FFFFFFFFFFF,0x00001FFFFFFFFFFF,0x00003FFFFFFFFFFF,0x00007FFFFFFFFFFF,0x0000FFFFFFFFFFFF,
    0x0001FFFFFFFFFFFF,0x0003FFFFFFFFFFFF,0x0007FFFFFFFFFFFF,0x000FFFFFFFFFFFFF,0x001FFFFFFFFFFFFF,0x003FFFFFFFFFFFFF,0x007FFFFFFFFFFFFF,0x00FFFFFFFFFFFFFF,
    0x01FFFFFFFFFFFFFF,0x03FFFFFFFFFFFFFF,0x07FFFFFFFFFFFFFF,0x0FFFFFFFFFFFFFFF,0x1FFFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFFF,0x7FFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF,
};
const quint64 BIT_MASK[64] = {
    0x0000000000000001,0x0000000000000002,0x0000000000000004,0x0000000000000008,0x0000000000000010,0x0000000000000020,0x0000000000000040,0x0000000000000080,
    0x0000000000000100,0x0000000000000200,0x0000000000000400,0x0000000000000800,0x0000000000001000,0x0000000000002000,0x0000000000004000,0x0000000000008000,
    0x0000000000010000,0x0000000000020000,0x0000000000040000,0x0000000000080000,0x0000000000100000,0x0000000000200000,0x0000000000400000,0x0000000000800000,
    0x0000000001000000,0x0000000002000000,0x0000000004000000,0x0000000008000000,0x0000000010000000,0x0000000020000000,0x0000000040000000,0x0000000080000000,
    0x0000000100000000,0x0000000200000000,0x0000000400000000,0x0000000800000000,0x0000001000000000,0x0000002000000000,0x0000004000000000,0x0000008000000000,
    0x0000010000000000,0x0000020000000000,0x0000040000000000,0x0000080000000000,0x0000100000000000,0x0000200000000000,0x0000400000000000,0x0000800000000000,
    0x0001000000000000,0x0002000000000000,0x0004000000000000,0x0008000000000000,0x0010000000000000,0x0020000000000000,0x0040000000000000,0x0080000000000000,
    0x0100000000000000,0x0200000000000000,0x0400000000000000,0x0800000000000000,0x1000000000000000,0x2000000000000000,0x4000000000000000,0x8000000000000000,
};

Object::Object(QSharedPointer<JgvIport::Interface> iport)
    : JgvInodeRegister::Object(*new ObjectPrivate(iport))
{}

void Object::prepare(InterfaceBuilder &builder, const XMLHelper &helper)
{
    Q_D(Object);
    JgvInodeRegister::Object::prepare(builder, helper);
    d->sType = sType;

    bool ok;
    int val = helper.getProperty(Properties::Bit).toInt(&ok, 10);
    d->bit = ok ? val : -1;

    val = helper.getProperty(Properties::LSB).toInt(&ok, 10);
    d->lsb = ok ? val : -1;

    val = helper.getProperty(Properties::MSB).toInt(&ok, 10);
    d->msb = ok ? val : -1;

}

void Object::invalidate()
{
    Q_D(Object);

    d->cache.isValid = false;
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

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    qCritical("MaskedIntReg::getValue error: machine en BigEndian !");
    return 0;
#endif

    if (!d->cache.isValid) {
        const uchar *p = data();
        if (p != 0) {
            const qint64 pSize = dataSize();
            // obtient la valeur du registre
            qint64 reg = 0;
            if (pSize == 4) {
                reg = (d->isLittleEndian) ? qFromLittleEndian<qint32>(p) : qFromBigEndian<qint32>(p);
            }
            else if (pSize == 8) {
                reg = (d->isLittleEndian) ? qFromLittleEndian<qint64>(p) : qFromBigEndian<qint64>(p);
            }
            else {
                qWarning("%s", qPrintable(QString("MaskedIntReg::getValue le buffer a une taille non reconnue %0").arg(pSize)));
                return 0;
            }

            // ********* BIG ENDIAN ***************
            // _____________________________________
            // |   0    |   1    |   2    |   3    |
            // 0        7        15       23       31
            //
            // ********* LITTLE ENDIAN *************
            // _____________________________________
            // |   3    |   2    |   1    |   0    |
            // 31       23       15       7        0
            //

            // msb max
            const int msbMax = (pSize * 8) - 1;

            if ((d->bit>=0) && (d->bit<=msbMax)) {
                // le numéro de bit est indiqué dans l'endianisme du registre (normalement big si pas précisé)
                // on calcule le masque
                const int bit = (d->isLittleEndian) ? d->bit : msbMax - d->bit;
                const quint64 mask = BIT_MASK[bit];

                // retourne la valeur du bit
                d->cache.value = ((reg & mask) > 0) ? 1 : 0;
                d->cache.isValid = d->readUpdateCache;
            }
            else if ((d->lsb>=0) && (d->msb>=0)) {
                // conversion de la numérotation de bits en littleEndian
                const int lsb = (d->isLittleEndian) ? d->lsb : msbMax - d->lsb;
                const int msb = (d->isLittleEndian) ? d->msb : msbMax - d->msb;

                // en littleEndian msb > lsb
                if (Q_LIKELY(msb>lsb)) {
                    const int maskSize = msb - lsb + 1;
                    // on alligne
                    qint64 res = reg >> lsb;
                    d->cache.value = res & MASK[maskSize];
                    d->cache.isValid = d->readUpdateCache;
                }
                else {
                    qCritical("MaskedIntReg::getValue msb:%d lsb:%d", msb, lsb);
                }
            }
            else {
                qWarning("MaskedIntReg::getValue error: %s", qPrintable(featureName()));
            }
        }
        else {
            qWarning("MaskedIntreg %s, InodeRegister pointer failed !", qPrintable(featureName()));
        }
    }
    return d->cache.value;
}



void Object::setValue(qint64 value)
{
    Q_D(Object);

    // sauvegarde la valeur dans le cache
    d->cache.value = value;

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    qCritical("MaskedIntReg::setValue error: machine en BigEndian !");
    return;
#endif

    uchar *p = data();
    if (p == 0) {
        qWarning("MaskedIntreg %s, InodeRegister pointer failed !", qPrintable(featureName()));
        return;
    }

    const qint64 size = dataSize();
    // obtient la valeur du registre
    qint64 reg = 0;
    if (size == 4) {
        reg = (d->isLittleEndian) ? qFromLittleEndian<qint32>(p) : qFromBigEndian<qint32>(p);
    }
    else if (size == 8) {
        reg = (d->isLittleEndian) ? qFromLittleEndian<qint64>(p) : qFromBigEndian<qint64>(p);
    }
    else {
        qCritical("%s", qPrintable(QString("MaskedIntReg::setValue le buffer a une taille non reconnue %0").arg(size)));
        return;
    }

    // ********* BIG ENDIAN ***************
    // _____________________________________
    // |   0    |   1    |   2    |   3    |
    // 0        7        15       23       31
    //
    // ********* LITTLE ENDIAN *************
    // _____________________________________
    // |   3    |   2    |   1    |   0    |
    // 31       23       15       7        0
    //


    // msb max
    const int msbMax = (size * 8) - 1;

    // 1 seul bit
    if (d->bit >= 0) {
        // le numéro de bit est indiqué dans l'endianisme du registre (normalement big si pas précisé)
        int bit = (d->isLittleEndian) ? d->bit : msbMax - d->bit;
        // on constitue le masque
        const quint64 mask = BIT_MASK[bit];
        // modifie la copie du registre
        if (value == 0) {
            reg &= (~mask);
        }
        else if (value == 1) {
            reg |= mask;
        }
        else {
            qWarning("MaskedIntReg %s setValue on one bit, value != (0 or 1): %lld", qPrintable(featureName()), value);
        }
    }
    else if ((d->lsb>=0) && (d->msb>=0)) {

        // conversion de la numérotation de bits en littleEndian
        const int lsb = (d->isLittleEndian) ? d->lsb : msbMax - d->lsb;
        const int msb = (d->isLittleEndian) ? d->msb : msbMax - d->msb;

        // en littleEndian msb > lsb
        if (Q_LIKELY(msb>lsb)) {
            // construction du masque
            const int maskSize = msb - lsb + 1;
            quint64 mask = MASK[maskSize];
            mask <<= lsb;

            // mise à zéro des bits concernés
            reg &= (~mask);

            // alignement de la valeur
            value <<= lsb;

            // modifie la copie du registre
            reg != value;
        }
        else {
            qCritical("MaskedIntReg::setValue msb:%d lsb:%d", msb, lsb);
        }

    }
    else {
        qWarning("MaskedIntReg::setValue error: %s", qPrintable(featureName()));
    }

    // provoque l'écriture sur le registre sous-jacent
    if (size == 4) {
        if (d->isLittleEndian) {
            qToLittleEndian<qint32>(reg, p);
        }
        else {
            qToBigEndian<qint32>(reg, p);
        }
    }
    else if (size == 8) {
        if (d->isLittleEndian) {
            qToLittleEndian<qint64>(reg, p);
        }
        else {
            qToBigEndian<qint64>(reg, p);
        }
    }
    else {
        qWarning("%s", qPrintable(QString("IntReg::setValue le buffer a une taille non reconnue %0").arg(size)));
    }
    updateData();
    d->cache.isValid = d->writeUpdateCache;
}

Jgv::Interface *Object::interface()
{
    return this;
}

const Jgv::Interface *Object::constInterface() const
{
    return this;
}



