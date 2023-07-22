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


#ifndef OVERLAYRENDERER_H
#define OVERLAYRENDERER_H

#include <QWidget>
typedef struct _GstElement GstElement;

class OverlayRenderer : public QWidget
{
    Q_OBJECT

    GstElement *m_sinker;
    WId m_windowID;
    bool m_sinkIsPainter;

public:
    explicit OverlayRenderer(QWidget *parent = 0);

signals:
    void showChanged(bool fullScreen);

public slots:
    void setSinker(GstElement *sink);
    void setSinkerIsPainter(bool isPainter);
    void setFullscreen(bool fullscreen, const QPoint &origin);


protected:
    bool eventFilter(QObject *filteredObject, QEvent *event);

private:
    void mouseDoubleClickEvent(QMouseEvent * event);
    void releaseSinker();

};


#endif

