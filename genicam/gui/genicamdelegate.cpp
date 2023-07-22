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

#include "genicamdelegate.h"
#include "inode.h"
#include "iinterface.h"
#include "iinteger.h"
#include "ifloat.h"
#include "ienumeration.h"
#include "iboolean.h"
#include "istring.h"
#include "enumentry.h"
#include "genicameditor.h"

#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QList>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QPainter>

GenicamDelegate::GenicamDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

QWidget *GenicamDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    QVariant variant = index.model()->data(index, Qt::UserRole);
    QSharedPointer<JgvInode::Object> p = variant.value<QWeakPointer<JgvInode::Object> >().toStrongRef();

    switch (JGV_ITYPE(p)) {
    case Jgv::ICommand: {
        GenICam::CommandEditor *editor = GenICam::EditorFactory::createCommandEditor(p->featureName(), p->displayName(), parent);
        editor->setMinimumHeight(editor->sizeHint().height());
        editor->setFocusPolicy(Qt::StrongFocus);
        connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
        return editor;
    }

    case Jgv::IInteger: {
        GenICam::IntegerEditor *editor = GenICam::EditorFactory::createIntegerEditor(p->featureName(), p->displayName(), parent);
        editor->setRange(JGV_IINTEGER(p)->getMin(), JGV_IINTEGER(p)->getMax());
        editor->setMinimumHeight(editor->sizeHint().height());
        editor->setFocusPolicy(Qt::StrongFocus);
        connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
        return editor;
    }

    case Jgv::IFloat: {
        GenICam::FloatEditor *editor = GenICam::EditorFactory::createDoubleEditor(p->featureName(), p->displayName(), parent);
        editor->setRange(JGV_IFLOAT(p)->getMin(), JGV_IFLOAT(p)->getMax());
        editor->setMinimumHeight(editor->sizeHint().height());
        editor->setFocusPolicy(Qt::StrongFocus);
        connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
        return editor;
    }

    case Jgv::IEnumeration: {
        GenICam::EnumerationEditor *editor = GenICam::EditorFactory::createEnumerationEditor(p->featureName(), p->displayName(), parent);
        editor->setMinimumHeight(editor->sizeHint().height());
        editor->setFocusPolicy(Qt::StrongFocus);
        connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
        return editor;
    }

    case Jgv::IBoolean: {
        GenICam::BooleanEditor *editor = GenICam::EditorFactory::createBooleanEditor(p->featureName(), p->displayName(), parent);
        editor->setMinimumHeight(editor->sizeHint().height());
        editor->setFocusPolicy(Qt::StrongFocus);
        connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
        return editor;
    }
    case Jgv::IString: {
        GenICam::StringEditor *editor = GenICam::EditorFactory::createStringEditor(p->featureName(), p->displayName(), parent);
        editor->setMinimumHeight(editor->sizeHint().height());
        editor->setFocusPolicy(Qt::StrongFocus);
        connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
        return editor;
    }

    default:
        // qWarning("%s", qPrintable(trUtf8("GenicamItemDelegate::createEditor %0 %1").arg(inode->featureName()).arg(inode->interface()->interfaceType())));
        break;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void GenicamDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant variant = index.model()->data(index, Qt::UserRole);
    QSharedPointer<JgvInode::Object> p = variant.value<QWeakPointer<JgvInode::Object> >().toStrongRef();
    if (p) {

        switch (JGV_ITYPE(p)) {

        case Jgv::IInteger: {
            GenICam::IntegerEditor *widget = qobject_cast<GenICam::IntegerEditor *>(editor);
            widget->setValue(JGV_IINTEGER(p)->getValue());
            break;
        }
        case Jgv::IFloat: {
            GenICam::FloatEditor *widget = qobject_cast<GenICam::FloatEditor *>(editor);
            widget->setValue(JGV_IFLOAT(p)->getValue());
            break;
        }
        case Jgv::IEnumeration: {
            GenICam::EnumerationEditor *widget = qobject_cast<GenICam::EnumerationEditor *>(editor);
            QList<JgvEnumentry::Object *> entries = JGV_IENUMERATION(p)->getEntries();
            qint64 current = JGV_IENUMERATION(p)->getIntValue();
            for (int index=0; index<entries.count(); ++index) {
                widget->addItem(entries.at(index)->getStringValue(), entries.at(index)->getIntValue());
                if (entries.at(index)->getIntValue() == current) {
                    widget->setCurrentIndex(index);
                }
            }
            break;
        }
        case Jgv::IBoolean: {
            GenICam::BooleanEditor *widget = qobject_cast<GenICam::BooleanEditor *>(editor);
            widget->setValue(JGV_IBOOLEAN(p)->getValue());
            break;
        }
        case Jgv::IString: {
            GenICam::StringEditor *widget = qobject_cast<GenICam::StringEditor *>(editor);
            widget->setValue(JGV_ISTRING(p)->getValue());
            break;
        }
        default:
            break;

        }
    }
}

void GenicamDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QVariant variant = index.model()->data(index, Qt::UserRole);
    QSharedPointer<JgvInode::Object> p = variant.value<QWeakPointer<JgvInode::Object> >().toStrongRef();
    if (p) {

        switch (JGV_ITYPE(p)) {

        case Jgv::ICommand: {
            GenICam::CommandEditor *widget = qobject_cast<GenICam::CommandEditor *>(editor);
            if (widget->isClicked()) {
                model->setData(index, 0, Qt::EditRole);
            }
            break;
        }

        case Jgv::IInteger: {
            GenICam::IntegerEditor *widget = qobject_cast<GenICam::IntegerEditor *>(editor);
            model->setData(index, widget->value(), Qt::EditRole);
            break;
        }

        case Jgv::IFloat: {
            GenICam::FloatEditor *widget = qobject_cast<GenICam::FloatEditor *>(editor);
            model->setData(index, widget->value(), Qt::EditRole);
            break;
        }

        case Jgv::IEnumeration: {
            GenICam::EnumerationEditor *widget = qobject_cast<GenICam::EnumerationEditor *>(editor);
            model->setData(index, widget->value(), Qt::EditRole);
            break;
        }

        case Jgv::IBoolean: {
            GenICam::BooleanEditor *widget = qobject_cast<GenICam::BooleanEditor *>(editor);
            model->setData(index, widget->value(), Qt::EditRole);
            break;
        }

        case Jgv::IString: {
            GenICam::StringEditor *widget = qobject_cast<GenICam::StringEditor *>(editor);
            model->setData(index, widget->value(), Qt::EditRole);
            break;
        }

        default:
            break;
        }
    }

}

void GenicamDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = editor->rect();
    rect.setWidth(option.rect.width());
    rect.moveCenter(option.rect.center());
    editor->setGeometry(rect);
}

void GenicamDelegate::commitAndCloseEditor()
{
    GenICam::Editor *editor = qobject_cast<GenICam::Editor *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}



//QSize GenicamItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    QSize size = QStyledItemDelegate::sizeHint(option, index);
//    size.rheight() *= 1.25;
//    return size;
//}

