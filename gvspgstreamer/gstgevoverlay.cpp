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

#include "gstgevoverlay.h"
#include "timeimage.h"
#include "gevinfos.h"

#include <gst/gst.h>
#include <gst/video/video-overlay-composition.h>
#include <QPainter>




enum
{
    PROP_0,
    PROP_SILENT,
    PROP_XPOS,
    PROP_YPOS,
    PROP_FONTSIZE,
    PROP_OPACITY
};

#define gst_gev_overlay_parent_class parent_class
G_DEFINE_TYPE (GstGevOverlay, gst_gev_overlay, GST_TYPE_ELEMENT)

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
                                                                    GST_PAD_SINK,
                                                                    GST_PAD_ALWAYS,
                                                                    GST_STATIC_CAPS ("ANY")
                                                                    );
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
                                                                   GST_PAD_SRC,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS ("ANY")
                                                                   );

static void gst_gev_overlay_set_property (GObject * object, guint prop_id,
                                          const GValue * value, GParamSpec * pspec);
static void gst_gev_overlay_get_property (GObject * object, guint prop_id,
                                          GValue * value, GParamSpec * pspec);
static gboolean gst_gev_overlay_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_gev_overlay_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);
static void gst_gev_overlay_update (GstGevOverlay * filter);
static void gst_gev_overlay_finalize (GObject * object);

static void gst_gev_overlay_class_init (GstGevOverlayClass * klass)
{
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;

    gobject_class = (GObjectClass *) klass;
    gstelement_class = (GstElementClass *) klass;

    gobject_class->finalize = gst_gev_overlay_finalize;
    gobject_class->set_property = gst_gev_overlay_set_property;
    gobject_class->get_property = gst_gev_overlay_get_property;

    g_object_class_install_property (gobject_class, PROP_SILENT,
                                     g_param_spec_boolean ("silent", "Silent", "Disable incrust",
                                                           FALSE, (GParamFlags)G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class, PROP_XPOS,
                                     g_param_spec_int("xpos", "X", "Relative X position of overlay",
                                                      0,100,0, (GParamFlags)G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class, PROP_YPOS,
                                     g_param_spec_int("ypos", "Y", "Relative Y position of overlay",
                                                      0,100,0, (GParamFlags)G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class, PROP_FONTSIZE,
                                     g_param_spec_int("fontsize", "FontSize", "Relative size of font",
                                                      3,100,3, (GParamFlags)G_PARAM_READWRITE));
    g_object_class_install_property (gobject_class, PROP_OPACITY,
                                     g_param_spec_int ("opacity", "Opacity", "Background opacity",
                                                       0,100,0, (GParamFlags)G_PARAM_READWRITE));

    gst_element_class_set_details_simple(gstelement_class,
                                         "GevOverlay",
                                         "FIXME:Generic",
                                         "FIXME:Generic Template Element",
                                         " <<user@hostname.org>>");

    gst_element_class_add_pad_template (gstelement_class,
                                        gst_static_pad_template_get (&src_factory));
    gst_element_class_add_pad_template (gstelement_class,
                                        gst_static_pad_template_get (&sink_factory));
}

static void gst_gev_overlay_init (GstGevOverlay * filter)
{
    filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
    gst_pad_set_event_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_gev_overlay_sink_event));
    gst_pad_set_chain_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_gev_overlay_chain));
    GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
    gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

    filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
    GST_PAD_SET_PROXY_CAPS (filter->srcpad);
    gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

    filter->silent = FALSE;
    filter->xpos = 0;
    filter->ypos = 0;
    filter->fontsize = 3;
    filter->timeImage = new TimeImage;

    gst_gev_overlay_update(filter);

}

static void gst_gev_overlay_set_property (GObject * object, guint prop_id,
                                          const GValue * value, GParamSpec * pspec)
{
    GstGevOverlay *filter = GST_GEVOVERLAY (object);

    switch (prop_id) {
    case PROP_SILENT:
        filter->silent = g_value_get_boolean (value);
        break;
    case PROP_XPOS:
        filter->xpos = g_value_get_int(value);
        gst_gev_overlay_update(filter);
        break;
    case PROP_YPOS:
        filter->ypos = g_value_get_int(value);
        break;
    case PROP_FONTSIZE:
        filter->fontsize = g_value_get_int(value);
        break;
    case PROP_OPACITY:
        filter->opacity = g_value_get_int(value);
        filter->timeImage->setOpacity(filter->opacity);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void gst_gev_overlay_get_property (GObject * object, guint prop_id,
                                          GValue * value, GParamSpec * pspec)
{
    GstGevOverlay *filter = GST_GEVOVERLAY (object);

    switch (prop_id) {
    case PROP_SILENT:
        g_value_set_boolean (value, filter->silent);
        break;
    case PROP_XPOS:
        g_value_set_int (value, filter->xpos);
        break;
    case PROP_YPOS:
        g_value_set_int (value, filter->ypos);
        break;
    case PROP_FONTSIZE:
        g_value_set_int (value, filter->fontsize);
        break;
    case PROP_OPACITY:
        g_value_set_int (value, filter->opacity);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static gboolean gst_gev_overlay_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
    gboolean ret;
    GstGevOverlay *filter;

    filter = GST_GEVOVERLAY (parent);

    switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
        GstCaps * caps;
        gst_event_parse_caps (event, &caps);
        gst_video_info_from_caps (&filter->sink_video_info, caps);
        gst_gev_overlay_update (filter);

        ret = gst_pad_event_default (pad, parent, event);
        break;
    }
    default:
        ret = gst_pad_event_default (pad, parent, event);
        break;
    }
    return ret;
}

static GstFlowReturn gst_gev_overlay_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
    GstGevOverlay *filter;

    filter = GST_GEVOVERLAY (parent);
    if (filter->silent == FALSE) {

        GstVideoFrame frame;
        // dessine dans le tampon local
        GevInfosMeta *meta = gst_buffer_get_gev_infos_meta(buf);

        if (meta != NULL) {

            // map le tampon arrivant
            buf = gst_buffer_make_writable (buf);
            gst_video_frame_map (&frame, &filter->sink_video_info, buf, (GstMapFlags)GST_MAP_READWRITE);

            // crée la composition
            g_mutex_lock(&filter->mutex);

            filter->timeImage->setTime(meta->timestamp);
            gst_video_overlay_composition_blend (filter->compo, &frame);
            g_mutex_unlock(&filter->mutex);

            // libère le tampon
            gst_video_frame_unmap(&frame);
        }


    }
    return gst_pad_push (filter->srcpad, buf);
}

#include <QDebug>

static void gst_gev_overlay_update (GstGevOverlay * filter)
{
    g_mutex_lock(&filter->mutex);

    gint fontSize = filter->sink_video_info.height * filter->fontsize / 100;
    // obtient la taille de l'image
    QSize size = filter->timeImage->setFontPixelSize(fontSize == 0 ? 1 : fontSize);

    // calcul les positions relatives
    gint x = (filter->sink_video_info.width - size.width()) * filter->xpos / 100;
    gint y = (filter->sink_video_info.height - size.height()) * filter->ypos / 100;


    // structure correspondant à l'image;
    GstVideoInfo video_info;
    gst_video_info_set_format (&video_info,
                               GST_VIDEO_OVERLAY_COMPOSITION_FORMAT_RGB,
                               size.width(), size.height());
    // alloue un nouveau tampon de la taille de l'image
    GstBuffer *buffer = gst_buffer_new_and_alloc(video_info.size);
    gst_buffer_add_video_meta(buffer, GST_VIDEO_FRAME_FLAG_NONE, GST_VIDEO_OVERLAY_COMPOSITION_FORMAT_RGB,
                              video_info.width, video_info.height);

    // map le tampon sur l'image
    filter->timeImage->map(buffer, video_info.width, video_info.height);

    // définit le rectangle de la composition
    GstVideoOverlayRectangle *overlay_rect = gst_video_overlay_rectangle_new_raw (
                                                 buffer,
                                                 x, y,
                                                 size.width(), size.height(),
                                                 GST_VIDEO_OVERLAY_FORMAT_FLAG_PREMULTIPLIED_ALPHA);


    // remplace l'ancien composeur
    if (GST_IS_VIDEO_OVERLAY_COMPOSITION(filter->compo)) {
        gst_video_overlay_composition_unref(filter->compo);
    }
    filter->compo = gst_video_overlay_composition_new(overlay_rect);


    // libère les ressources
    gst_video_overlay_rectangle_unref(overlay_rect);
    gst_buffer_unref(buffer);

    g_mutex_unlock(&filter->mutex);

}

static void gst_gev_overlay_finalize (GObject * object)
{
    GstGevOverlay *filter;
    filter = GST_GEVOVERLAY (object);
    delete filter->timeImage;

    G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean gevoverlay_init (GstPlugin * gevoverlay)
{
    return gst_element_register (gevoverlay, "gevoverlay", GST_RANK_NONE,
                                 GST_TYPE_GEVOVERLAY);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstgevoverlay"
#endif

/* gstreamer looks for this structure to register gevoverlays
 *
 * exchange the string 'Template gevoverlay' with your gevoverlay description
 */
//GST_PLUGIN_DEFINE (
//    GST_VERSION_MAJOR,
//    GST_VERSION_MINOR,
//    gevoverlay,
//    "Template gevoverlay",
//    gevoverlay_init,
//    "1.0.0",
//    "LGPL",
//    "GStreamer",
//    "http://gstreamer.net/"
//)

gboolean gevoverlay_register_static() {
    return gst_plugin_register_static(
                GST_VERSION_MAJOR,
                GST_VERSION_MINOR,
                "gevoverlay",
                "Overlay",
                gevoverlay_init,
                "1.0.0",
                "LGPL",
                "GevOverlay",
                "Jiguiviou",
                "http://gstreamer.net/"
                );
}
