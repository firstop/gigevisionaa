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
#ifndef GENICAMMODEL_H
#define GENICAMMODEL_H

#include <QAbstractItemModel>
#include <QSharedPointer>



class FakeInode;
class IPort;


template <class T>
class QList;
namespace JgvInode {class Object;}
namespace JgvIport {class Interface;}

class GenicamModelPrivate;
class GenicamModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    GenicamModel(QSharedPointer<JgvIport::Interface> iport, QObject *parent = 0);
    ~GenicamModel();

    void setGenICamXml(const QString &fileName, const QByteArray &file);
    QModelIndex searchFeature(int column, const QString &featureName) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    Qt::ItemFlags flags (const QModelIndex & index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role= Qt::EditRole );

    QString name() const;



protected:
     void timerEvent(QTimerEvent *event);

private:
    const QScopedPointer<GenicamModelPrivate> d;
    Q_DISABLE_COPY(GenicamModel)
};

#endif // GENICAMMODEL_H
