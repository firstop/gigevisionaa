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

#include "genicamxmlfile.h"
#include "genicamxmlfile_p.h"
#include "xmlhelper.h"
#include "category.h"
#include "qtiocompressor/qtiocompressor.h"

#include <QDomDocument>
#include <QStringList>
#include <QBuffer>
#include <QTextStream>
#include <QFile>

const QString group = "Group";

void GenICamXMLFilePrivate::createIndex()
{
    index.clear();

    QDomElement docElem = dom.documentElement();
    if (!docElem.isNull()) {
        // détecte d'abord la balise group GenICam
        QDomNode n = docElem.firstChildElement(group);
        if (!n.isNull()) {
            while(!n.isNull()) {
                fillIndex(n);
                n = n.nextSiblingElement(group);
            }
        }
        fillIndex(docElem);

    } else {
        qWarning("%s", qPrintable(QObject::trUtf8("Le DOM n'est pas un document valide")));
    }
}

void GenICamXMLFilePrivate::fillIndex(const QDomNode &node)
{
    QDomNode n = node.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        // exclue les noeuds group GenICam
        if (!e.isNull() && e.tagName() != group) {
            QString key = e.attribute(Attributes::Name);
            if (!key.isEmpty()) {
                if (!index.contains(key)) {
                    index.insert(key,n);
                } else {
                    qWarning("%s", qPrintable(QObject::trUtf8("Le noeud %0 du DOM n'est pas unique").arg(key)));
                }
            } else {
                qWarning("%s", qPrintable(QObject::trUtf8("Un noeud du DOM a un nom vide %0").arg(e.tagName())));
            }
        }
        n = n.nextSibling();
    }
}

GenICamXMLFile::GenICamXMLFile()
    : d(new GenICamXMLFilePrivate)
{}

GenICamXMLFile::~GenICamXMLFile()
{}

void GenICamXMLFile::setFileName(const QString &fileName)
{
    d->fileName = fileName;
}

void GenICamXMLFile::setContent(const QByteArray &content)
{
    d->dom.clear();
    d->dom.setContent(content);
    d->createIndex();
}

void GenICamXMLFile::setZippedContent(const QByteArray &zippedContent)
{
    QByteArray zip = zippedContent;
    QBuffer buf(&zip);
    buf.open(QIODevice::ReadOnly);
    QDataStream s(&buf);
    s.setByteOrder(QDataStream::LittleEndian);

    /* Local file header:

            local file header signature     4 bytes  (0x04034b50)
            version needed to extract       2 bytes
            general purpose bit flag        2 bytes
            compression method              2 bytes
            last mod file time              2 bytes
            last mod file date              2 bytes
            crc-32                          4 bytes
            compressed size                 4 bytes
            uncompressed size               4 bytes
            file name length                2 bytes
            extra field length              2 bytes */

    quint32 signature;
    while (!s.atEnd()) {
        s >> signature;
        // on cherche la première signature
        if (signature == 0x04034b50) {
            quint32 crc, compSize, unCompSize;
            quint16 extractVersion, bitFlag, compMethod, modTime, modDate;
            quint16 nameLen, extraLen;

            s >> extractVersion >> bitFlag >> compMethod;
            s >> modTime >> modDate >> crc >> compSize >> unCompSize;
            s >> nameLen >> extraLen;

            // saute les champs à taille variable
            buf.seek(buf.pos() + nameLen + extraLen);

            // crée un buffer des données compressées
            QByteArray compData = buf.read(compSize);
            QBuffer compBuf(&compData);

            // decompresse
            QtIOCompressor compressor(&compBuf);
            compressor.setStreamFormat(QtIOCompressor::RawZipFormat);
            compressor.open(QIODevice::ReadOnly);
            QByteArray unCompData = compressor.readAll();
            compressor.close();

            // affecte au dom
            d->dom.clear();
            d->dom.setContent(unCompData);
            d->createIndex();

        }
    }
    buf.close();
}

void GenICamXMLFile::saveContent(const QString &path)
{
    if (!d->dom.isNull() && !d->fileName.isEmpty()) {
        QString fileName = d->fileName;
        if (fileName.endsWith(".zip")) {
            fileName.replace(".zip", ".xml");
        }
        QFile file(path +"/"+ fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            d->dom.save(stream, 4);
            file.close();
        }
    }
}

QDomNode GenICamXMLFile::getRootChildNodeByNameAttribute(const QString &name) const
{
    return d->index.value(name);
}




