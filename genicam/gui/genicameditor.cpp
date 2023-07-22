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

#include "genicameditor.h"
#include "genicamint64editor.h"
#include "genicamdoubleeditor.h"
#include "genicambuttoneditor.h"
#include "genicamcomboboxeditor.h"
#include "genicambooleditor.h"
#include "genicamlineeditor.h"
#include "genicamipv4editor.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QFont>

using namespace GenICam;


Editor::Editor(const QString &name, QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::AlternateBase);
    setAttribute(Qt::WA_NoMousePropagation);
    QVBoxLayout *layout = new QVBoxLayout;
    QLabel *label = new QLabel(name);
    label->setAlignment(Qt::AlignCenter);
    QFont font = QFrame::font();
    font.setBold(true);
    label->setFont(font);
    layout->addWidget(label);
    setLayout(layout);
    setToolTip(trUtf8("Entrée valide, Échap ignore"));
}

Editor::~Editor()
{}


IntegerEditor *EditorFactory::createIntegerEditor(const QString &featureName, const QString &displayName, QWidget *parent)
{
    if (featureName == "GevCurrentIPAddress"
            || featureName == "GevCurrentSubnetMask"
            || featureName == "GevCurrentDefaultGateway"
            || featureName == "GevPersistentIPAddress"
            || featureName == "GevPersistentSubnetMask"
            || featureName == "GevPersistentDefaultGateway"
            || featureName == "GevPrimaryApplicationIPAddress"
            || featureName == "GevMCDA"
            || featureName == "GevSCDA") {
        return new GenICam::Ipv4Editor(displayName, parent);
    }
    return new GenICam::Int64Editor(displayName, parent);
}

FloatEditor *EditorFactory::createDoubleEditor(const QString &featureName, const QString &displayName, QWidget *parent)
{
    return new GenICam::DoubleEditor(displayName, parent);
}

CommandEditor *EditorFactory::createCommandEditor(const QString &featureName, const QString &displayName, QWidget *parent)
{
    return new GenICam::ButtonEditor(displayName, parent);
}

EnumerationEditor *EditorFactory::createEnumerationEditor(const QString &featureName, const QString &displayName, QWidget *parent)
{
    return new GenICam::ComboboxEditor(displayName, parent);
}

BooleanEditor *EditorFactory::createBooleanEditor(const QString &featureName, const QString &displayName, QWidget *parent)
{
    return new GenICam::BoolEditor(displayName, parent);
}

StringEditor *EditorFactory::createStringEditor(const QString &featureName, const QString &displayName, QWidget *parent)
{
    return new GenICam::LineEditor(displayName, parent);
}



