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

#ifndef __GST_GEVOVERLAY_H__
#define __GST_GEVOVERLAY_H__

#include <gst/gst.h>
#include <gst/video/video-info.h>

class TimeImage;

G_BEGIN_DECLS

#define GST_TYPE_GEVOVERLAY                 (gst_gev_overlay_get_type())
#define GST_GEVOVERLAY(obj)                 (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GEVOVERLAY, GstGevOverlay))
#define GST_GEVOVERLAY_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_GEVOVERLAY, GstGevOverlayClass))
#define GST_IS_GEVOVERLAY(obj)              (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GEVOVERLAY))
#define GST_IS_GEVOVERLAY_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_GEVOVERLAY))

typedef struct _GstGevOverlay      GstGevOverlay;
typedef struct _GstGevOverlayClass GstGevOverlayClass;

struct _GstGevOverlay
{
    GstElement element;
    GstPad *sinkpad, *srcpad;
    TimeImage *timeImage;

    GstVideoInfo sink_video_info;       /* infos from sink pad */
    GstVideoOverlayComposition *compo;  /* composition */

    gboolean silent;
    gint xpos;
    gint ypos;
    gint fontsize;
    gint opacity;

    GMutex mutex;

};

struct _GstGevOverlayClass
{
    GstElementClass parent_class;
};

gboolean gevoverlay_register_static();

GType gst_gev_overlay_get_type (void);

G_END_DECLS

#endif /* __GST_GEVOVERLAY_H__ */
