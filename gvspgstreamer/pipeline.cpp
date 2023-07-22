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

#include "pipeline.h"
#include "pipeline_p.h"
#include "gstreamerutils.h"
#include "overlayrenderer.h"
#include "gstsinkbin.h"
#include "gevinfos.h"
#include "gstgevoverlay.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/video/videooverlay.h>
#include <gst/video/video-overlay-composition.h>
#include <gst/gstplugin.h>

#include <QPainter>
#include <QTime>
#include <QDebug>

const char * VIEW_BRANCH =     "viewBranch";
const char * RECORD_BRANCH =   "recordBranch";
const char * RTP_BRANCH =      "rtpBranch";

const char * BAYER_NAME = "bayer";
const char * INCRUST_NAME = "incrust";
const char * CONVERT_NAME = "convert";
const char * QUEUE_NAME = "queue";
const char * PIPELINE_TEE_NAME = "pipelineTee";

struct SourceBin {
    GstAppSrc *src;
    GstBin *sourceBin;
    GstCaps *caps;
    GstBus *bus;
};

struct BranchBin {
    GstBin *pipeline;
    GstElement *bin;
};


static volatile gint probingCounter;
static volatile gint bufferCounter;

GST_PLUGIN_STATIC_DECLARE(sink_bin);

static GstPadProbeReturn addBranch_cb(GstPad *pad, GstPadProbeInfo *info, gpointer data)
{
    if  (g_atomic_int_compare_and_exchange(&probingCounter, 0, 1)) {
        // on retire la sonde
        gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));
        // cast des données
        const BranchBin *contener = static_cast<BranchBin *>(data);
        // test si le bin n'est pas déjà la
        GstElement *bin = NULL;
        bin = gst_bin_get_by_name(contener->pipeline, GST_ELEMENT_NAME(contener->bin));
        if (bin == NULL) {
            gst_bin_add(contener->pipeline, contener->bin);
            // synchronise le bin
            gst_element_sync_state_with_parent(contener->bin);
            // connect le pad au bin
            gst_pad_link(pad, gst_element_get_static_pad(contener->bin, "sink"));
        }

        // libère les ressources
        gst_object_unref(contener->bin);
        gst_object_unref(contener->pipeline);
        delete contener;

        g_atomic_int_set(&probingCounter, 0);
        return GST_PAD_PROBE_DROP;
    }
    else {
        qDebug("pass");
        return GST_PAD_PROBE_PASS;
    }
}

static GstPadProbeReturn removeBranch_cb(GstPad *pad, GstPadProbeInfo *info, gpointer data)
{
    if  (g_atomic_int_compare_and_exchange(&probingCounter, 0, 1)) {
        // on retire la sonde
        gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));
        // cast des données
        GstElement *bin = static_cast<GstElement *>(data);
        GstPad *sinkPad = gst_element_get_static_pad(bin, "sink");
        // deconnecte le bin
        gst_pad_unlink(pad, sinkPad);
        // injecte l'EOS
        gst_pad_send_event(sinkPad, gst_event_new_eos());
        // libère les ressources
        gst_object_unref(sinkPad);
        // libère le tee
        gst_element_release_request_pad(GST_PAD_PARENT(pad), pad);

        g_atomic_int_set(&probingCounter, 0);
        return GST_PAD_PROBE_DROP;
    }
    else {
        return GST_PAD_PROBE_PASS;
    }
}

//static GstPadProbeReturn bufferAdded_cb(GstPad *pad, GstPadProbeInfo *info, gpointer data)
//{
//    g_atomic_int_inc(&bufferCounter);
//    return GST_PAD_PROBE_OK;
//}


void Pipeline::async_message_application(GstBus *bus, GstMessage *message, Pipeline *self)
{
    Q_UNUSED(bus)
    if (gst_message_has_name(message, "restartsource")) {
        gst_element_set_state(self->d_ptr->sourceBin, GST_STATE_READY);
        gst_element_set_state(self->d_ptr->sourceBin, GST_STATE_PLAYING);
    }
    else if ((gst_message_has_name(message, "removesender"))) {
        GstElement *bin = GST_ELEMENT(GST_MESSAGE_SRC(message));
        gst_bin_remove(GST_BIN(self->d_ptr->pipeline), bin);
        if (g_str_has_prefix(GST_MESSAGE_SRC_NAME(message), VIEW_BRANCH)) {
            emit self->viewBranchActived(NULL);
            emit self->viewStateChanged(false);
        }
        else  if (g_str_has_prefix(GST_MESSAGE_SRC_NAME(message), RECORD_BRANCH)) {
            emit self->isRecording(false);
        }
        // destruction
        gst_element_set_state(bin, GST_STATE_NULL);

    }
    else {
        qDebug("message from %s", gst_element_get_name(GST_MESSAGE_SRC(message)));
    }
}

void Pipeline::sync_message_element(GstBus *bus, GstMessage *message, Pipeline *self)
{
    Q_UNUSED(bus);
    if (gst_is_video_overlay_prepare_window_handle_message(message)) {
        qDebug("Prepare overlay");
        emit self->viewBranchActived(GST_ELEMENT(GST_MESSAGE_SRC(message)));
    }
}

void Pipeline::async_message_state(GstBus *bus, GstMessage *message, Pipeline *self)
{
    Q_UNUSED(bus);
    // parsing du message state-changed
    GstState oldstate, newstate, pending;
    gst_message_parse_state_changed(message,&oldstate,&newstate,&pending);
    // la branche vision a demarré
    if (newstate == GST_STATE_PLAYING) {
        if (g_strcmp0(GST_MESSAGE_SRC_NAME(message), VIEW_BRANCH) == 0) {
            emit self->viewStateChanged(true);
        }
        else if (g_strcmp0(GST_MESSAGE_SRC_NAME(message), RECORD_BRANCH) == 0) {
            emit self->isRecording(true);
        }
    }

}

PipelinePrivate::PipelinePrivate()
    : pipeline(NULL),
      source(NULL),
      tee(NULL)
{}

PipelinePrivate::~PipelinePrivate()
{}

Pipeline::Pipeline(QObject * parent)
    : QObject(parent),IView(this),IRecord(this),IRtp(this),
      d_ptr(new PipelinePrivate)
{
    setenv("GST_DEBUG_DUMP_DOT_DIR", "/home/cyril/", 1);
    setenv("GST_DEBUG", "*:3", 1);
    createMinimalPipeline();
    // active par défaut la vue
    toggleView();

}

Pipeline::~Pipeline()
{
    Q_D(Pipeline);
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(d->pipeline),  GST_DEBUG_GRAPH_SHOW_ALL, "kk.dot");
    gst_element_set_state(d->pipeline, GST_STATE_NULL);
    gst_object_unref(d->source);
    gst_object_unref(d->tee);
    gst_object_unref(d->sourceBin);
    gst_object_unref(d->pipeline);
    qDebug("Deleting Pipeline");
}

GstAppSrc *Pipeline::appSrc()
{
    Q_D(Pipeline);
    return GST_APP_SRC(d->source);
}

int Pipeline::framesCount() const
{
    return g_atomic_int_get(&bufferCounter);
}

void Pipeline::allocate(GvspImage &image)
{
    Q_D(Pipeline);
    // allocation malloc
    image.datas.p = static_cast<quint8 *>(g_malloc(image.datas.size));
    // si besoin de demosaic
    image.isBayer = d->demosaic.needDemosaic(image.geometry.pixelFormat);
    // si bayer alloue la mémoire pour la debayerisation
    if (image.isBayer) {
        d->demosaic.allocate(image);
    }
}

void Pipeline::push(GvspImage &image)
{
    Q_D(Pipeline);

    // changement de format
    if (d->geometry != image.geometry) {
        d->geometry = image.geometry;
        GstCaps *caps;
        if (image.isBayer) {
            caps = d->demosaic.capsForTransform(image);
        }
        else {
            const char *f = "";
            caps = gst_caps_new_empty_simple("video/x-raw");
            switch (image.geometry.pixelFormat) {
            case GVSP_PIX_MONO8:            f="GRAY8"; break;
            case GVSP_PIX_RGB8_PACKED:      f="RGB"; break;
            case GVSP_PIX_BGR8_PACKED:      f="BGR"; break;
            case GVSP_PIX_RGBA8_PACKED:     f="RGBA"; break;
            case GVSP_PIX_BGRA8_PACKED:     f="BGRA"; break;
            case GVSP_PIX_YUV411_PACKED:    f="IYU1"; break;
            case GVSP_PIX_YUV422_PACKED:    f="UYVY"; break;
            default:
                qDebug("Pipeline: bad pixelformat %u", image.geometry.pixelFormat);
                gst_caps_unref(caps);
                caps = gst_caps_new_empty();
            }
            if (!gst_caps_is_empty(caps)) {
                gst_caps_set_simple(caps,
                                    "format", G_TYPE_STRING, f,
                                    "width", G_TYPE_INT, image.geometry.width,
                                    "height", G_TYPE_INT, image.geometry.height,
                                    "framerate", GST_TYPE_FRACTION, 0, 1,
                                    NULL);


            }
        }
        gst_app_src_set_caps(GST_APP_SRC(d->source), caps);
        gst_caps_unref(caps);
    }

    GstBuffer *buf = image.isBayer
                     ? d->demosaic.transform(image)
                     : gst_buffer_new_wrapped(image.datas.p, image.datas.size);

    gst_buffer_add_gev_infos_meta(buf, image.timestamp);

    GstFlowReturn flowReturn = gst_app_src_push_buffer(GST_APP_SRC(d->source), buf);
    if (Q_UNLIKELY(flowReturn != GST_FLOW_OK)) {
        qWarning("GstreamerBackend : push buffer return %d", flowReturn);
    }
}

void Pipeline::trash(GvspImage &image) const
{
    g_free(image.datas.p);
}


void Pipeline::toggleView()
{
    Q_D(Pipeline);

    // obtient un nouveau pad sur le tee
    GstElement *bin = gst_bin_get_by_name(GST_BIN(d->pipeline), VIEW_BRANCH);
    if (bin) {
        GstPad *pad = gst_pad_get_peer(gst_element_get_static_pad(bin, "sink"));
        if (pad) {
            // envoie la sonde pour retrait
            gst_pad_add_probe(pad, GstPadProbeType(GST_PAD_PROBE_TYPE_BUFFER), removeBranch_cb, bin, NULL);
            gst_object_unref(pad);
        }
        else qDebug("Pas de peer");
    }
    else {
        QStringList list;
        list << "queue" << "videoconvert" << "xvimagesink";
        //if (GstreamerUtils::areInstalled(list)) {
        // notre sink modifié
        bin = gst_element_factory_make("sinkbin", VIEW_BRANCH);
        GstElement *queue = gst_element_factory_make("queue", "viewqueue");
        GstElement *convert = gst_element_factory_make("videoconvert", "viewvideoconvert");
        GstElement *sink = gst_element_factory_make("xvimagesink", NULL);
        gst_bin_add_many(GST_BIN (bin), queue, convert, sink, NULL);
        gst_element_link_many(queue, convert, sink, NULL);

        GstPad *pad = gst_element_get_static_pad(queue, "sink");
        gst_element_add_pad(bin, gst_ghost_pad_new("sink", pad));
        gst_object_unref(pad);

        // contruit un conteneur pour l'ajout du bin
        BranchBin *contener = new BranchBin;
        contener->pipeline = GST_BIN(gst_object_ref(d->pipeline));
        contener->bin = GST_ELEMENT(gst_object_ref(bin));

        // obtient un nouveau pad sur le tee
        GstElement *tee = gst_bin_get_by_name(GST_BIN(d->pipeline), PIPELINE_TEE_NAME);
        pad = gst_element_get_request_pad(tee, "src_%u");
        gst_object_unref(tee);

        // insert la sonde sur le pad nouvellement créé
        gst_pad_add_probe(pad, GstPadProbeType(GST_PAD_PROBE_TYPE_BUFFER), addBranch_cb, contener, NULL);
        gst_object_unref(pad);

        //}
    }
}

void Pipeline::setRenderFramerate(double framerate)
{
    //    if (m_bins.contains(TYPE_VIEW)) {
    //        GstElement * sinkElement = gst_bin_get_by_name(GST_BIN (m_pipeline), qPrintable (VIDEO_SINK_NAME));
    //        if (sinkElement != NULL) {
    //            quint64 periode = 1000000000.0 / framerate;
    //            g_object_set(G_OBJECT (sinkElement),
    //                         "throttle-time", periode,
    //                         NULL);
    //        }
    //    }

}

void Pipeline::setRenderOutput(bool fullscreen, const QPoint &origin)
{
    //emit changeView(fullscreen, origin);
}

void Pipeline::enableIncrust(bool enable)
{
    Q_D(Pipeline);
    GstElement * incrust = NULL;
    if (d->sourceBin && (incrust = gst_bin_get_by_name (GST_BIN (d->sourceBin), INCRUST_NAME)) != NULL ) {
        g_object_set(G_OBJECT(incrust), "silent", !enable, NULL);
        gst_object_unref(incrust);
    }
}


void Pipeline::changeIncrustPosition(int x, int y, int fontSize, int opacity)
{
    Q_D(Pipeline);

    // obtient un nouveau pad sur le tee
    GstElement *incrust = gst_bin_get_by_name(GST_BIN(d->sourceBin), INCRUST_NAME);
    g_object_set(G_OBJECT(incrust),
                 "xpos", x,
                 "ypos", y,
                 "fontsize", fontSize,
                 "opacity", opacity,
                 NULL);
}

void Pipeline::toggleRecording(const QString &filename)
{
    Q_D(Pipeline);

    // obtient un nouveau pad sur le tee
    GstElement *bin = gst_bin_get_by_name(GST_BIN(d->pipeline), RECORD_BRANCH);
    if (bin) {
        GstPad *pad = gst_pad_get_peer(gst_element_get_static_pad(bin, "sink"));
        if (pad) {
            // envoie la sonde pour retrait
            gst_pad_add_probe(pad, GstPadProbeType(GST_PAD_PROBE_TYPE_BUFFER), removeBranch_cb, bin, NULL);
            gst_object_unref(pad);
        }
        else qDebug("Pas de peer");
    }
    else {
        QStringList list;
        list << "queue" << "videoconvert" << "x264enc" << "mpegtsmux" << "filesink";
        //if (GstreamerUtils::areInstalled(list)) {

        // notre bin spécialisé
        GstElement *bin = gst_element_factory_make("sinkbin", RECORD_BRANCH);
        GstElement *queue = gst_element_factory_make("queue", NULL);
        GstElement *convert = gst_element_factory_make("videoconvert", NULL);
        //GstElement *post = gst_element_factory_make("vaapipostproc", NULL);
        //GstElement *enc = gst_element_factory_make("vaapiencode_h264", NULL);
        //            GstElement *enc = gst_element_factory_make("jpegenc", NULL);
        //            GstElement *mux = gst_element_factory_make("avimux", NULL);
        GstElement *enc = gst_element_factory_make("x264enc", NULL);
        //GstElement *mux = gst_element_factory_make("mpegtsmux", NULL);
        //GstElement *mux = gst_element_factory_make("matroskamux", NULL);
        GstElement *mux = gst_element_factory_make("mp4mux", NULL);
        GstElement *sink = gst_element_factory_make("filesink", NULL);

        gst_util_set_object_arg(G_OBJECT (enc), "speed-preset", "veryfast");
        g_object_set(G_OBJECT (enc),
                     "bitrate", 4096u,
                     //"b-adapt", false,
                     "key-int-max", 1,
                     //"ref", 12,
                     NULL);

        g_object_set(G_OBJECT (sink),
                     "location", qPrintable(filename),
                     "async", true,
                     NULL);


        gst_bin_add_many(GST_BIN (bin), queue, convert, /*post, */enc, mux, sink, NULL);
        gst_element_link_many(queue, convert, /*post, */enc, mux, sink, NULL);

        GstPad *pad = gst_element_get_static_pad(queue, "sink");
        gst_element_add_pad(bin, gst_ghost_pad_new("sink", pad));
        gst_object_unref(pad);

        // contruit un conteneur pour l'ajout du bin
        BranchBin *contener = new BranchBin;
        contener->pipeline = GST_BIN(gst_object_ref(d->pipeline));
        contener->bin = GST_ELEMENT(gst_object_ref(bin));

        // obtient un nouveau pad sur le tee
        GstElement *tee = gst_bin_get_by_name(GST_BIN(d->pipeline), PIPELINE_TEE_NAME);
        pad = gst_element_get_request_pad(tee, "src_%u");
        gst_object_unref(tee);

        // insert la sonde sur le pad nouvellement créé
        gst_pad_add_probe(pad, GstPadProbeType(GST_PAD_PROBE_TYPE_BUFFER), addBranch_cb, contener, NULL);
        gst_object_unref(pad);

        //}
    }
}

void Pipeline::toggleRtpStream(const RtpDesc &desc)
{
    //    if (m_bins.contains(TYPE_RTP)) {
    //        GstBus * bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    //        gst_bus_post(bus, gst_message_new_application(
    //                         GST_OBJECT_CAST (m_pipeline),
    //                         gst_structure_new(
    //                             qPrintable(BIN_MESSAGE),
    //                             "action", G_TYPE_INT, REMOVE,
    //                             "sinkbin", G_TYPE_POINTER, m_bins[TYPE_RTP],
    //                             NULL)));
    //        gst_object_unref(bus);
    //        m_bins.remove(TYPE_RTP);
    //    }
    //    else {
    //        QStringList list;
    //        list << "queue" << "x264enc" << "rtph264pay" << "rtpbin" << "udpsink" << "udpsrc";
    //        if (GstreamerUtils::areInstalled(list)) {
    //            GstElement *bin = gst_bin_new(qPrintable(RTP_BRANCH));
    //            GstElement *queue = gst_element_factory_make("queue", NULL);
    //            GstElement *convert = gst_element_factory_make("videoconvert", NULL);
    //            GstElement *h264enc = gst_element_factory_make("x264enc", NULL);
    //            g_object_set(h264enc,
    //                         "bitrate", 4096,
    //                         NULL);

    //            gst_util_set_object_arg(G_OBJECT (h264enc), "speed-preset", "ultrafast");
    //            gst_util_set_object_arg(G_OBJECT (h264enc), "tune", "zerolatency");

    //            GstElement *h264pay = gst_element_factory_make("rtph264pay", NULL);
    //            GstElement *rtpsink = gst_element_factory_make("udpsink", NULL);
    //            g_object_set(rtpsink,
    //                         "port", desc.streamPort,
    //                         "host", "239.255.43.21",
    //                         "auto-multicast", true,
    //                         "bind-address", qPrintable(QHostAddress(desc.bindAddress).toString()),
    //                         NULL);

    //            gst_bin_add_many(GST_BIN (bin), queue, convert, h264enc, h264pay, rtpsink, NULL);
    //            gst_element_link_many(queue, convert, h264enc, NULL);
    //            gst_element_link_many(h264pay, rtpsink, NULL);
    //            GstCaps *caps = gst_caps_new_simple(
    //                                "video/x-h264",
    //                                "stream-format", G_TYPE_STRING, "avc",
    //                                "profile", G_TYPE_STRING, "baseline",
    //                                NULL);
    //            gst_element_link_filtered(h264enc, h264pay, caps);
    //            gst_caps_unref(caps);


    //            GstPad *pad = gst_element_get_static_pad(queue, "sink");
    //            gst_element_add_pad(bin, gst_ghost_pad_new("sink", pad));
    //            gst_object_unref(GST_OBJECT (pad));

    //            SinkBin *sinkBin = new SinkBin(bin, m_pipeline, m_srcTee);
    //            m_bins.insert(TYPE_RTP, sinkBin);
    //            //            connect(sinkBin, SIGNAL(isActive(bool)),
    //            //                    this, SIGNAL(viewActived(bool)));

    //            GstBus * bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    //            gst_bus_post(bus, gst_message_new_application(
    //                             GST_OBJECT_CAST (m_pipeline),
    //                             gst_structure_new(qPrintable(BIN_MESSAGE),
    //                                               "action", G_TYPE_INT, ADD,
    //                                               "sinkbin", G_TYPE_POINTER, m_bins[TYPE_RTP],
    //                                               NULL)));
    //            gst_object_unref(bus);

    //        }
    //    }
}


void Pipeline::createMinimalPipeline()
{
    Q_D(Pipeline);
    if (! gst_is_initialized()) {
        gst_init(0, 0);
        sink_bin_register_static();
        gevoverlay_register_static();
        //        GST_PLUGIN_STATIC_REGISTER(sink_bin);
    }

    // création du pipeline
    d->pipeline = gst_pipeline_new("pipeline");
    gst_object_ref_sink(d->pipeline);


    // connexion bus callbacks
    GstBus * bus = gst_pipeline_get_bus(GST_PIPELINE(d->pipeline));
    gst_bus_enable_sync_message_emission (bus);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "sync-message::element", G_CALLBACK(sync_message_element), this);
    g_signal_connect(bus, "message::application", G_CALLBACK(async_message_application), this);
    g_signal_connect(bus, "message::state-changed", G_CALLBACK(async_message_state), this);

    gst_object_unref(bus);

    // creation du bin source
    d->sourceBin = gst_bin_new(NULL);
    gst_object_ref_sink(d->sourceBin);

    // creation de la source
    d->source = gst_element_factory_make ("appsrc", "GiGE_Vision");
    gst_object_ref_sink(d->source);
    // on fixe una caps, afin que appsrc accepte les tampons même invalide
    gst_app_src_set_caps(GST_APP_SRC(d->source),
                         gst_caps_new_simple("video/x-raw",
                                             "format", G_TYPE_STRING, "UYVY",
                                             "width", G_TYPE_INT, 800,
                                             "height", G_TYPE_INT, 600,
                                             "framerate", GST_TYPE_FRACTION, 0, 1,
                                             NULL));
    g_object_set(G_OBJECT(d->source),
                 "is-live", true,
                 "min-latency", 0,
                 "do-timestamp", true,
                 "format", GST_FORMAT_TIME,
                 NULL);

    GstElement *queue = gst_element_factory_make("queue", QUEUE_NAME);
    GstElement *incrust = gst_element_factory_make("gevoverlay", INCRUST_NAME);
    GstElement *convert = gst_element_factory_make("videoconvert", CONVERT_NAME);


    gst_bin_add_many(GST_BIN(d->sourceBin),d->source, queue, incrust, convert, NULL);


    GstPad *pad = gst_element_get_static_pad(convert, "src");
    gst_element_add_pad(d->sourceBin, gst_ghost_pad_new("src", pad));
    gst_object_unref(pad);
    gst_element_link_many(d->source, queue, incrust, convert, NULL);
    gst_bin_add(GST_BIN(d->pipeline), d->sourceBin);



    g_object_set(G_OBJECT(incrust),
                 "silent", true,
                 NULL);

    // creation du tee
    d->tee = gst_element_factory_make("tee", PIPELINE_TEE_NAME);
    gst_object_ref_sink(d->tee);
    gst_bin_add(GST_BIN(d->pipeline), d->tee);
    gst_element_link(d->sourceBin, d->tee);



    // creation de la branche poubelle
    GstElement *fakesink = gst_element_factory_make("fakesink", NULL);
    gst_bin_add(GST_BIN(d->pipeline), fakesink);
    gst_element_link(d->tee, fakesink);

    //
    //    pad = gst_element_get_static_pad(fakesink, "sink");
    //    gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback) bufferAdded_cb, NULL, NULL);
    //    gst_object_unref (pad);

    //    g_atomic_int_set(&bufferCounter, 0);
    gst_element_set_state(d->pipeline, GST_STATE_PLAYING);
}







