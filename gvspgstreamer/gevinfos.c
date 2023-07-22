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

#include "gevinfos.h"

const GstMetaInfo *gev_infos_meta_get_info (void);
#define GEV_INFOS_META_INFO (gev_infos_meta_get_info())



GType gev_infos_meta_api_get_type (void)
{
  static volatile GType type;
  static const gchar *tags[] = { "timestamp", NULL };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GevMetaMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }
  return type;
}

static gboolean gev_infos_meta_init (GstMeta * meta, gpointer params, GstBuffer * buffer)
{
  GevInfosMeta *emeta = (GevInfosMeta *) meta;

  emeta->timestamp = 0;

  return TRUE;
}

static gboolean gev_infos_meta_transform (GstBuffer * transbuf, GstMeta * meta,
    GstBuffer * buffer, GQuark type, gpointer data)
{
  GevInfosMeta *emeta = (GevInfosMeta *) meta;

  /* we always copy no matter what transform */
  gst_buffer_add_gev_infos_meta (transbuf, emeta->timestamp);

  return TRUE;
}

static void gev_infos_meta_free (GstMeta * meta, GstBuffer * buffer)
{}

const GstMetaInfo *gev_infos_meta_get_info (void)
{
  static const GstMetaInfo *meta_info = NULL;

  if (g_once_init_enter (&meta_info)) {
    const GstMetaInfo *mi = gst_meta_register (GEV_INFOS_META_API_TYPE,
        "GevInfosMeta",
        sizeof (GevInfosMeta),
        gev_infos_meta_init,
        gev_infos_meta_free,
        gev_infos_meta_transform);
    g_once_init_leave (&meta_info, mi);
  }
  return meta_info;
}

GevInfosMeta *gst_buffer_add_gev_infos_meta (GstBuffer *buffer, glong timestamp)
{
  GevInfosMeta *meta;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);

  meta = (GevInfosMeta *) gst_buffer_add_meta (buffer, GEV_INFOS_META_INFO, NULL);

  meta->timestamp = timestamp;

  return meta;
}
