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

#ifndef GEV_INFOS_H
#define GEV_INFOS_H

#include <gst/gst.h>

G_BEGIN_DECLS

typedef struct _GevInfosMeta GevInfosMeta;

struct _GevInfosMeta {
  GstMeta       meta;
  glong         timestamp;

};

GType gev_infos_meta_api_get_type (void);
#define GEV_INFOS_META_API_TYPE (gev_infos_meta_api_get_type())
#define gst_buffer_get_gev_infos_meta(b) ((GevInfosMeta*)gst_buffer_get_meta((b), GEV_INFOS_META_API_TYPE))

GevInfosMeta *gst_buffer_add_gev_infos_meta (GstBuffer *buffer, glong timestamp);


G_END_DECLS

#endif /* GEV_INFOS_H */
