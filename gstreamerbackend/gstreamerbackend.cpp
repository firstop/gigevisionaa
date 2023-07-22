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

#include "gstreamerbackend.h"
#include "gstreamerbackend_p.h"

#include "viewpage.h"
#include "recordpage.h"
#include "rtppage.h"
#include "overlayrenderer.h"
#include "pipeline.h"


#include <QDir>
#include <QDirIterator>
#include <QCoreApplication>
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <QDockWidget>
#include <QDebug>


GstreamerBackend::GstreamerBackend(QObject *parent)
    : DefaultBackend(*new GstreamerBackendPrivate, parent)
{
    Q_D(GstreamerBackend);
    d->pipeline = QSharedPointer<Pipeline>(new Pipeline);
}

GstreamerBackend::~GstreamerBackend()
{
    qDebug("Deleting GStreamer Backend");
}

QSharedPointer<GvspClient> GstreamerBackend::gvspClient()
{
    Q_D(GstreamerBackend);
    return d->pipeline;
}

BackendWidgets GstreamerBackend::widgets()
{
    Q_D(GstreamerBackend);

    BackendWidgets widgets;
    OverlayRenderer *renderer = new OverlayRenderer;
    widgets.main = renderer;
    connect(d->pipeline.data(), SIGNAL(viewBranchActived(GstElement*)),
            renderer, SLOT(setSinker(GstElement*)));
    connect(d->pipeline.data(), SIGNAL(viewStateChanged(bool)),
            renderer, SLOT(setSinkerIsPainter(bool)));

    widgets.toolbar = new QToolBar(trUtf8("Sortie GStreamer"));

    QDockWidget *dock = new QDockWidget;
    ViewPage *view = new ViewPage(d->pipeline);
    dock->setWidget(view);
    dock->setVisible(false);
    widgets.configurations << dock;
    widgets.toolbar->addAction(view->visibilityAction());
    connect(view->visibilityAction(), SIGNAL(toggled(bool)), dock, SLOT(setVisible(bool)));

    dock = new QDockWidget;
    RecordPage *record = new RecordPage(d->pipeline);
    dock->setWidget(record);
    dock->setVisible(false);
    widgets.configurations << dock;
    widgets.toolbar->addAction(record->visibilityAction());
    connect(record->visibilityAction(), SIGNAL(toggled(bool)), dock, SLOT(setVisible(bool)));

    return widgets;
}


//void GstreamerBackend::onChangeIncrust(int type)
//{
//    Q_D(GstreamerBackend);
//    switch (type) {
//    case NONE:
//        d->pipeline->enableIncrust(false);
//        break;
//    case SYSTEM_TIME:
//        d->pipeline->enableIncrust(true);
//        break;
//    default:
//        break;
//    }

//}

//void GstreamerBackend::setEncoderProfil(const QString &profil, bool record)
//{
//    Q_D(GstreamerBackend);
//    //    if (record) {
//    //        // détermination de l'index du nom de fichier
//    //        int index = 0;
//    //        QDirIterator it(QDir::homePath(), QStringList() << "jiguiviou_????.ts");
//    //        while (it.hasNext()) {
//    //            QFileInfo fi(it.next());
//    //            int i = fi.baseName().remove(0, 10).toInt();
//    //            if (i>index) {
//    //                index = i;
//    //            }
//    //        }
//    //        d->pipeline->recordToFile(QString("%0/jiguiviou_%1.ts").arg(QDir::homePath()).arg(++index, 4, 10, QLatin1Char('0')));

//    //    } else {
//    //        d->pipeline->stopRecord();
//    //    }
//}








