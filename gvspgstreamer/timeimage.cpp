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

#include "timeimage.h"

#include <gst/gstbuffer.h>

#include <QImage>
#include <QFontMetrics>
#include <QPainter>

#include <QDebug>

const uint ZERO = 0x30;

TimeImage::TimeImage()
    : m_time("00:00:00.00"),
      m_pen(Qt::white),
      m_font("monospace", 100, QFont::Bold),
      m_backColor(0,0,0,0),
      m_image(new QImage),
      m_buffer(NULL)
{}

TimeImage::~TimeImage()
{
    if (m_buffer) gst_buffer_unref(m_buffer);
}

void TimeImage::map(GstBuffer *buf, int width, int height)
{
    GstBuffer *writable = gst_buffer_make_writable(buf);
    GstMapInfo map_info;
    if (gst_buffer_map(writable, &map_info, GST_MAP_WRITE)) {
        m_image.reset(new QImage(map_info.data, width, height, QImage::Format_ARGB32_Premultiplied));
        gst_buffer_unmap(writable, &map_info);
    }
    gst_buffer_replace(&m_buffer, writable);
}

void TimeImage::setTime(quint64 time)
{

    quint64 nano = time % 1000000000ll;
    time /= 1000000000ll;
    quint64 sec = time % 60;
    quint64 min = (time % 3600) / 60;
    quint64 hour = (time / 3600) % 24;

    m_time[0] = QChar(ZERO + static_cast<uint>(hour / 10));
    m_time[1] = QChar(ZERO + static_cast<uint>(hour % 10));
    m_time[3] = QChar(ZERO + static_cast<uint>(min / 10));
    m_time[4] = QChar(ZERO + static_cast<uint>(min % 10));
    m_time[6] = QChar(ZERO + static_cast<uint>(sec / 10));
    m_time[7] = QChar(ZERO + static_cast<uint>(sec % 10));
    m_time[9] = QChar(ZERO + static_cast<uint>(nano / 100000000));
    m_time[10] = QChar(ZERO + static_cast<uint>((nano % 100000000) / 10000000));


    m_image->fill(m_backColor);
    QPainter painter(m_image.data());
    painter.setPen(m_pen);
    painter.setFont(m_font);
    painter.drawText(m_origin, m_time);
}

void TimeImage::setOpacity(int value)
{
    m_backColor.setAlpha(value*255/100);
}

QSize TimeImage::setFontPixelSize(int pixelSize)
{
    m_font.setPixelSize(pixelSize);
    QFontMetrics metrics = QFontMetrics(m_font);
    // la taille complète
    QSize size  = metrics.size(Qt::TextSingleLine, m_time);
    m_origin.setX(0);
    m_origin.setY(pixelSize - (size.height() - pixelSize) / 2) ;

    return QSize(size.width(), pixelSize);
}




