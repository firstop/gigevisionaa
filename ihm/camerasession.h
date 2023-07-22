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

#ifndef CAMERASESSION_H
#define CAMERASESSION_H

#include <QObject>

template <class T>
class QSharedPointer;
class GevConnectionDescription;



class QMainWindow;
class QHostAddress;
class QStackedWidget;
class QAction;
class QAbstractItemModel;
class DefaultBackend;
class CameraSessionPrivate;

class CameraSession : public QObject
{
    Q_OBJECT

public:
    explicit CameraSession(QMainWindow *parent);
    ~CameraSession();

    QAction *initSession(const QHostAddress &controller, const QHostAddress &transmitter);
    void select();
    bool isSelected() const;

private slots:
    void setVisible(bool visible);

private:
    QMainWindow *window() const;
    QStackedWidget *stack();


protected:
    const QScopedPointer<CameraSessionPrivate> d;

private:
    Q_DISABLE_COPY(CameraSession)
};

#endif // CAMERASESSION_H
