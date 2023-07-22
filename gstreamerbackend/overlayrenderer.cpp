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

#include "overlayrenderer.h"

#include <gst/video/videooverlay.h>

#include <QPainter>
//#include <QPaintEvent>
#include <QApplication>
#include <QThread>

OverlayRenderer::OverlayRenderer(QWidget *parent)
    : QWidget(parent),
      m_sinker(NULL),
      m_sinkIsPainter(false)
{
    installEventFilter(this);
}

void OverlayRenderer::setSinker(GstElement *sink)
{
    Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());
    if (sink != NULL) {
        if (m_sinker == NULL) {
            m_windowID = winId();
            QApplication::syncX();
            //installEventFilter(this);
            setAttribute(Qt::WA_NoSystemBackground, true);
            setAttribute(Qt::WA_PaintOnScreen, true);
            update();
        }
        if (m_sinker) {
            releaseSinker();
        }
        gst_object_ref_sink(sink);
        m_sinker = sink;
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(m_sinker), m_windowID);
    }
    else {
        releaseSinker();
    }

}

void OverlayRenderer::setSinkerIsPainter(bool isPainter)
{
    m_sinkIsPainter = isPainter;
    update();
}

void OverlayRenderer::setFullscreen(bool fullscreen, const QPoint &origin)
{
    if (fullscreen) {
        setWindowFlags(Qt::Window);
        move(origin);
        move(100,100);
        showFullScreen();
    }
    else {
        setWindowFlags(Qt::Widget);
        showNormal();
    }
    update();
}

void OverlayRenderer::releaseSinker()
{
    if (m_sinker) {
        //removeEventFilter(this);
        setAttribute(Qt::WA_NoSystemBackground, false);
        setAttribute(Qt::WA_PaintOnScreen, false);
        gst_object_unref(m_sinker);
        m_sinker = NULL;
        update();
    }
}

bool OverlayRenderer::eventFilter(QObject *filteredObject, QEvent *event)
{
    if (filteredObject == this) {
        switch (event->type()) {
        case QEvent::Paint:
            if (m_sinker != NULL) {
                if (m_sinkIsPainter) {
                    gst_video_overlay_expose(GST_VIDEO_OVERLAY(m_sinker));
                } else {
                    QPainter p(this);
                    p.fillRect(this->rect(), Qt::blue);
                    p.drawText(this->rect(), Qt::AlignCenter, "PAUSED");
                }
            }
            else {
                QPainter p(this);
                p.fillRect(this->rect(), Qt::green);
                p.drawText(this->rect(), Qt::AlignCenter, "NO SINK");
            }
            return true;
        case QEvent::WinIdChange:
            if (m_sinker != NULL) {
                gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(m_sinker), winId());
            }
            break;

        case QEvent::Show:
            emit showChanged(isWindow());
            break;

        default:
            break;

        }
    }
    return QWidget::eventFilter(filteredObject, event);
}

void OverlayRenderer::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (isWindow()) {
        setFullscreen(false, QPoint());
    }
    else {
        setFullscreen(true, pos());
    }
}
