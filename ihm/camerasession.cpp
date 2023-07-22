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

#include "camerasession.h"
#include "camerasession_p.h"
#include "gui/genicammodel.h"
#include "gui/genicamdelegate.h"
#include "genicamtreeview.h"
#include "defaultbackend.h"
#include "gstreamerbackend.h"
#include "devicetoolbar.h"
#include "jgvwizardpage.h"
#include "gevmonitor.h"
#include "geviport.h"
#include "gvcpclient.h"
#include "gvspreceiver.h"
#include "pipeline.h"

#include <QMainWindow>
#include <QAbstractItemModel>
#include <QAction>
#include <QStackedWidget>
#include <QDockWidget>
#include <QHostAddress>

const char * PERSISTENT_VISIBILITY = "pv";

CameraSessionPrivate::CameraSessionPrivate()
    : backendMainWidget(NULL),
      genicamWidget(NULL),
      genicamWidgetDock(NULL),
      model(NULL),
      action(NULL),
      deviceToolBar(NULL),
      backend(NULL),
      backendToolBar(NULL)
{}


CameraSession::CameraSession(QMainWindow *parent)
    : QObject(parent),
      d(new CameraSessionPrivate)
{}

CameraSession::~CameraSession()
{
    // destruction du backend
    if (d->backend != NULL) {
        delete d->backend;
        delete d->backendMainWidget;
        delete d->backendToolBar;
        qDeleteAll(d->backendConfigurationWidgets);

    }

    if (d->deviceToolBar != NULL) delete d->deviceToolBar;
    if (d->genicamWidget != NULL) delete d->genicamWidget;
    if (d->genicamWidgetDock != NULL) delete d->genicamWidgetDock;
    if (d->model != NULL) delete d->model;

}

QAction *CameraSession::initSession(const QHostAddress &controller, const QHostAddress &transmitter)
{
    // se connecte en mode monitor afin de récuperer l'XML
    GevMonitor monitor;
    if (!monitor.connectTo(controller, transmitter)) {
        qWarning("Failed to connect transmitter %s", qPrintable(transmitter.toString()));
        return NULL;
    }

    QByteArray firstURL = monitor.getFirstUrl();
    QString fileName = GevMonitor::filenameFromFirstURL(firstURL);


    QByteArray xml = monitor.getXml(monitor.getFirstUrl());

    // instance du backend
    d->backend = new GstreamerBackend(this);

    // les widgets du backends sont gérés par la session
    // on garde donc un pointeur dessus
    BackendWidgets widgets = d->backend->widgets();
    // la toolbar du backend
    d->backendToolBar = widgets.toolbar;
    // les widgets de configuration du backend
    d->backendConfigurationWidgets = widgets.configurations;
    // ajoute la propriété persistent visibility
    foreach (QDockWidget *dock, d->backendConfigurationWidgets) {
        dock->setProperty(PERSISTENT_VISIBILITY, false);
    }

    // le widget principal du backend
    if (stack()) {
        stack()->addWidget(d->backendMainWidget = widgets.main);
    }


    // création du receveur
    d->gvsp = new GvspReceiver(d->backend->gvspClient(), this);
    quint16 receiverPort = d->gvsp->getFreePort(controller);
    d->gvsp->listen(controller, receiverPort, transmitter);


    // on crée l'iport
    QSharedPointer<GevIPort> iport(new GevIPort);
    iport->connectAsController(controller, transmitter);
    // affecte le receveur
    iport->addReceiver(0, controller, receiverPort);

    // création du modèle
    d->model = new GenicamModel(iport, this);
    d->model->setGenICamXml(fileName, xml);

    // création de la vue sur le modèle
    d->genicamWidget = new GenICamTreeView;
    d->genicamWidget->setModel(d->model);

    // docking de la vue
    d->genicamWidgetDock = new QDockWidget(d->model->name(), window());
    d->genicamWidgetDock->setProperty(PERSISTENT_VISIBILITY, false);
    d->genicamWidgetDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    d->genicamWidgetDock->setWidget(d->genicamWidget);
    d->genicamWidgetDock->toggleViewAction();

    // action pour selectionner la vue
    d->action = new QAction(QIcon("://ressources/icons/jiguiviou.png"), d->model->name(), this);
    d->action->setCheckable(true);
    connect(d->action, SIGNAL(toggled(bool)),this, SLOT(setVisible(bool)));

    // la barre liée à la session
    d->deviceToolBar = new DeviceToolBar;
    QAction *action = d->genicamWidgetDock->toggleViewAction();
    action->setIcon(QIcon("://ressources/icons/configure-device.png"));
    action->setText(trUtf8("Afficher GenICam"));
    d->deviceToolBar->addAction(action);
    d->deviceToolBar->addSeparator();
    d->deviceToolBar->addModel(d->model);

    return d->action;

}

void CameraSession::select()
{
    d->action->setChecked(true);
}



bool CameraSession::isSelected() const
{
    return (d->action == NULL) ? false : d->action->isChecked();
}

void CameraSession::setVisible(bool visible)
{
    // suspend la mise à jour de l'IHM
    window()->setUpdatesEnabled(false);

    if (visible) {
        // GeniCam widget
        if (d->genicamWidgetDock != NULL) {
            window()->addDockWidget(Qt::LeftDockWidgetArea, d->genicamWidgetDock);
            d->genicamWidgetDock->setVisible(d->genicamWidgetDock->property(PERSISTENT_VISIBILITY).toBool());
        }

        // gvspToolBar
        if (d->deviceToolBar != NULL) {
            window()->addToolBar(Qt::LeftToolBarArea, d->deviceToolBar);
            d->deviceToolBar->setVisible(true);
        }

        // backend
        if (d->backend && stack()) {
            const int index = stack()->indexOf(d->backendMainWidget);
            stack()->setCurrentIndex(index);

            window()->addToolBar(Qt::RightToolBarArea, d->backendToolBar);
            d->backendToolBar->setVisible(true);
            foreach (QDockWidget *dock, d->backendConfigurationWidgets) {
                window()->addDockWidget(Qt::RightDockWidgetArea, dock);
                // restore l'état persistant
                dock->setVisible(dock->property(PERSISTENT_VISIBILITY).toBool());
            }
        }

    }
    else {
        // backend
        if (d->backend && stack()) {
            window()->removeToolBar(d->backendToolBar);
            foreach (QDockWidget *dock, d->backendConfigurationWidgets) {
                // sauvegarde l'état
                dock->setProperty(PERSISTENT_VISIBILITY, dock->isVisible());
                window()->removeDockWidget(dock);
            }
        }

        if (d->genicamWidgetDock != NULL) {
            d->genicamWidgetDock->setProperty(PERSISTENT_VISIBILITY, d->genicamWidgetDock->isVisible());
            window()->removeDockWidget(d->genicamWidgetDock);
        }
        // gvspToolBar
        if (d->deviceToolBar != NULL) {
            window()->removeToolBar(d->deviceToolBar);
        }
    }

    // restore la mise à jour de l'IHM
    window()->setUpdatesEnabled(true);

}

QMainWindow *CameraSession::window() const
{
    return qobject_cast<QMainWindow *>(parent());
}

QStackedWidget *CameraSession::stack()
{
    return qobject_cast<QStackedWidget *>(window()->centralWidget());

}


