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

#include "gstreamerutils.h"

#include <QList>
#include <QStringList>

#include <gst/gst.h>

bool GstreamerUtils::ready()
{
    bool initialized = gst_is_initialized();
    if (!initialized) {
        qWarning("GStreamer is not initialized !");
    }
    return initialized;
}

PropertyList GstreamerUtils::getPropertyList(const QString &elementName)
{
    GstElement *el = NULL;
    PropertyList list;
    if (ready() && (el = gst_element_factory_make(qPrintable(elementName), NULL)) != NULL) {
        list = getPropertyList(*el);
        gst_object_unref(el);
    }
    return list;
}

PropertyList GstreamerUtils::getPropertyList(const GstElement &element)
{
    PropertyList list;
    guint propertyCount;
    GParamSpec **propertySpecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(&element), &propertyCount);
    for (guint i = 0; i < propertyCount; ++i) {
        ElementProperty property;
        GParamSpec *param = propertySpecs[i];
        property.name = g_param_spec_get_name(param);


        switch(param->value_type) {
        case G_TYPE_UINT:
            property.defaultValue.setValue(G_PARAM_SPEC_UINT(param)->default_value);
            property.min.setValue(G_PARAM_SPEC_UINT(param)->minimum);
            property.max.setValue(G_PARAM_SPEC_UINT(param)->maximum);
            break;

        case G_TYPE_STRING:
            property.defaultValue.setValue(QString(G_PARAM_SPEC_STRING(param)->default_value));
            break;

        case G_TYPE_INT:
            property.defaultValue.setValue(G_PARAM_SPEC_INT(param)->default_value);
            property.min.setValue(G_PARAM_SPEC_INT(param)->minimum);
            property.max.setValue(G_PARAM_SPEC_INT(param)->maximum);
            break;

        case G_TYPE_FLOAT:
            property.defaultValue.setValue(G_PARAM_SPEC_FLOAT(param)->default_value);
            property.min.setValue(G_PARAM_SPEC_FLOAT(param)->minimum);
            property.max.setValue(G_PARAM_SPEC_FLOAT(param)->maximum);
            break;

        case G_TYPE_DOUBLE:
            property.defaultValue.setValue(G_PARAM_SPEC_DOUBLE(param)->default_value);
            property.min.setValue(G_PARAM_SPEC_DOUBLE(param)->minimum);
            property.max.setValue(G_PARAM_SPEC_DOUBLE(param)->maximum);
            break;

        case G_TYPE_BOOLEAN:
            property.defaultValue.setValue(G_PARAM_SPEC_BOOLEAN(param)->default_value);
            break;

        default:
            GValue value = { 0, };
            g_value_init(&value, param->value_type);

            if (G_IS_PARAM_SPEC_ENUM(param)) {
                const GEnumValue *values = G_ENUM_CLASS (g_type_class_ref (param->value_type))->values;
                gint current = g_value_get_enum(&value);
                property.enumName.setValue(QString(g_type_name(G_VALUE_TYPE(&value))));

                QStringList vals;
                int j = 0;
                while (values[j].value_name) {
                    vals.append(QString (values[j].value_nick));
                    if (values[j].value == current) {
                        property.defaultValue.setValue(QString(values[j].value_nick));

                       // qDebug() << values[j].value_name << "current" << QString(values[j].value_nick);
                    }
                    ++j;
                }
                property.enumValues.setValue(vals);
            }
            break;
        }
        list.append(property);
    }
    g_free(propertySpecs);
    return list;
}

bool GstreamerUtils::isInstalled(const QString &name)
{
    bool installed = false;
    if (ready()) {
        GstElementFactory *factory = gst_element_factory_find(qPrintable(name));
        installed = (factory != NULL);
        if (installed) {
            gst_object_unref(factory);
        }
        else {
            qWarning("%s", qPrintable(QObject::trUtf8("GStreamer : le plugin %0 n'est pas intallé !").arg(name)));
        }

    }

    return installed;
}

bool GstreamerUtils::areInstalled(const QStringList &names)
{
    bool installed = !names.isEmpty();

    foreach (QString name, names) {
        installed &= isInstalled(name);
    }
    return installed;
}


