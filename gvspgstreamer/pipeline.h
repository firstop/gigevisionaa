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

#ifndef PIPELINE_H
#define PIPELINE_H


#include <QObject>
#include <QScopedPointer>

#include "gvsp.h"
#include "viewpage.h"
#include "recordpage.h"
#include "rtppage.h"


typedef struct _GstAppSrc GstAppSrc;
typedef struct _GstBus GstBus;
typedef struct _GstMessage GstMessage;
typedef struct _GstCaps GstCaps;
typedef struct _GstElement GstElement;
typedef struct _GstBuffer GstBuffer;

class PipelinePrivate;
class Pipeline : public QObject, public GvspClient, public IView, public IRecord, public IRtp
{
    Q_OBJECT

    static void sync_message_element(GstBus *bus, GstMessage *message, Pipeline *self);
    static void async_message_state(GstBus *bus, GstMessage *message, Pipeline *self);
    static void async_message_application(GstBus *bus, GstMessage *message, Pipeline *self);

public:
    explicit Pipeline(QObject * parent = 0);
    ~Pipeline();

    GstAppSrc *appSrc();

    int framesCount() const;

    // ########################### Interface GvspClient ##################################
    void allocate(GvspImage &image);
    void push(GvspImage &image);
    void trash(GvspImage &image) const;


signals: // ########################### Interface IView ##################################
    void viewStateChanged(bool actived);
public slots:
    void toggleView();
    void setRenderFramerate(double framerate);
    void setRenderOutput(bool fullscreen, const QPoint &origin);
    void enableIncrust(bool enable);
    void changeIncrustPosition(int x, int y, int fontSize, int opacity);                       // Interface IPipeline

signals: // ########################## Interface IRecord ###################################
    void isRecording(bool recording);
public slots:
    void toggleRecording(const QString &filename); // Interface IRecord

signals: // ########################## Interface IRtp ###################################
    void rtpActived(bool actived);
public slots:
    void toggleRtpStream(const RtpDesc &desc); // Interface IRtp


signals:
    void pipelineDrawVideo(bool isDrawing);
    void pipelineIsActive(bool active);
    void gstreamerInitialized();
    void viewBranchActived(GstElement *overlay);
    void changeView(bool fullscreen, const QPoint &origin);

private:
    void createMinimalPipeline();

protected:
    const QScopedPointer<PipelinePrivate> d_ptr;

private:
    Q_DISABLE_COPY(Pipeline)
    Q_DECLARE_PRIVATE(Pipeline)
};

#endif // PIPELINE_H
