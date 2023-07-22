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

#ifndef __SINK_BIN_H__
#define __SINK_BIN_H__

#include <gst/gst.h>

G_BEGIN_DECLS
#define TYPE_SINK_BIN                   (sink_bin_get_type())
#define SINK_BIN(obj)                   (G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_SINK_BIN, SinkBin))
#define SINK_BIN_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_SINK_BIN, SinkBinClass))
#define IS_SINK_BIN(obj)                (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYPE_SINK_BIN))
#define IS_SINK_BIN_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_SINK_BIN))

gboolean sink_bin_register_static();

typedef struct _SinkBin      SinkBin;
typedef struct _SinkBinClass SinkBinClass;

struct _SinkBin {
    GstBin parent;
};

struct _SinkBinClass {
    GstBinClass parent_class;
};

GType sink_bin_get_type(void);


G_END_DECLS

#endif /* __SINK_BIN_H__ */
