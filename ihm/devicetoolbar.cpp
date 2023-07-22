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

#include "devicetoolbar.h"
#include "devicetoolbar_p.h"
#include "genicamcommandaction.h"

#include <QAction>
#include <QAbstractItemModel>

DeviceToolBarPrivate::DeviceToolBarPrivate()
    : model(NULL)
{}

DeviceToolBarPrivate::~DeviceToolBarPrivate()
{}


DeviceToolBar::DeviceToolBar(QWidget *parent)
    : QToolBar(trUtf8("Commandes GVSP"), parent), d_ptr(new DeviceToolBarPrivate)
{}

DeviceToolBar::~DeviceToolBar()
{
    qDebug("Deleting GVSP ToolBar");
}

void DeviceToolBar::addModel(GenicamModel *model)
{
    GenicamCommandAction *start =  new GenicamCommandAction("AcquisitionStart", model, this);
    start->setIcon(QIcon("://ressources/icons/run.png"));
    start->setText(trUtf8("Démarrer GVSP"));
    addAction(start);
    GenicamCommandAction *stop = new GenicamCommandAction("AcquisitionStop", model, this);
    stop->setIcon(QIcon("://ressources/icons/stop.png"));
    stop->setText(trUtf8("Stopper GVSP"));
    addAction(stop);

}


