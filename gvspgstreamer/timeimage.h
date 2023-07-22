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

#ifndef TIMEIMAGE_H
#define TIMEIMAGE_H

#include <QString>
#include <QPen>
#include <QFont>
#include <QScopedPointer>

typedef struct _GstBuffer      GstBuffer;

class QImage;
class TimeImage
{
    QString m_time;
    int m_width;
    int m_height;
    QPen m_pen;
    QFont m_font;
    QColor m_backColor;
    QScopedPointer<QImage> m_image;
    GstBuffer *m_buffer;
    QPoint m_origin;

public:
    TimeImage();
    ~TimeImage();

    void map(GstBuffer *buf, int width, int height);

    void setTime(quint64 time);
    void setOpacity(int value);

    QSize setFontPixelSize(int pixelSize);


};

#endif // TIMEIMAGE_H
