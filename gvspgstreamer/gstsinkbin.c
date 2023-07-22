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

#include "gstsinkbin.h"




G_DEFINE_TYPE (SinkBin, sink_bin, GST_TYPE_BIN)

static void  sink_bin_handle_message (GstBin * bin, GstMessage * message);

static void sink_bin_init (SinkBin *self)
{}

static void sink_bin_class_init (SinkBinClass * klass)
{
    GstBinClass *gstbin_class;
    gstbin_class = (GstBinClass *) klass;
    gstbin_class->handle_message = GST_DEBUG_FUNCPTR(sink_bin_handle_message);
}

static gboolean sink_bin_plugin_init (GstPlugin * plugin)
{
    gboolean res;
    res = gst_element_register (plugin, "sinkbin", GST_RANK_NONE, TYPE_SINK_BIN);
    return res;
}

static void  sink_bin_handle_message (GstBin * bin, GstMessage * message)
{
    if (GST_MESSAGE_TYPE (message) == GST_MESSAGE_EOS) {
        gst_message_unref(message);
        message = gst_message_new_application(
                    GST_OBJECT_CAST (bin),
                    gst_structure_new_empty ("removesender"));
    }
    GST_BIN_CLASS (sink_bin_parent_class)->handle_message (bin, message);
}

#ifndef PACKAGE
#define PACKAGE "sinkbin"
#endif

GST_PLUGIN_DEFINE (
        GST_VERSION_MAJOR,
        GST_VERSION_MINOR,
        sink_bin,
        "Template sinkbin",
        sink_bin_plugin_init,
        "1.0.0",
        "LGPL",
        PACKAGE,
        "http://gstreamer.net/"
        )


gboolean sink_bin_register_static() {
    return gst_plugin_register_static(
                GST_VERSION_MAJOR,
                GST_VERSION_MINOR,
                "sinkbin",
                "Bin whithout src pad whose handle EOS",
                sink_bin_plugin_init,
                "1.0.0",
                "LGPL",
                "SinkBin",
                "Jiguiviou",
                "http://gstreamer.net/"
                );
}

