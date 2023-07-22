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

#ifndef CAMERASESSION_P_H
#define CAMERASESSION_P_H

#include <QSharedPointer>

template <class T>
class QList;
class QWidget;
class GenICamTreeView;
class QDockWidget;
class GenicamModel;
class GvspReceiver;
namespace GVCP {class Channel;}
namespace GVSP {class Channel;}
class QAction;
class DefaultBackend;
class DeviceToolBar;
class QToolBar;
namespace IStream {class Interface;}
class CameraSessionPrivate
{
public:
    CameraSessionPrivate();

    GenICamTreeView *genicamWidget;
    QDockWidget *genicamWidgetDock;
    GenicamModel *model;
    GvspReceiver *gvsp;
    QAction *action;

    DeviceToolBar *deviceToolBar;

    // le backend
    DefaultBackend *backend;
    QWidget *backendMainWidget;
    QToolBar *backendToolBar;
    QList<QDockWidget *> backendConfigurationWidgets;

};

#endif // CAMERASESSION_P_H
