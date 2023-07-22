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

#ifndef RECORDPAGE_H
#define RECORDPAGE_H

#include <QWidget>
#include <QScopedPointer>


template <class T>
class QSharedPointer;

class IRecord
{
    QObject * m_parent;

public:
    explicit IRecord(QObject *parent) : m_parent(parent) {}
    virtual ~IRecord(){}
    QObject * qObject() {return m_parent;}

    virtual void toggleRecording(const QString &filename) = 0;
    virtual void isRecording(bool recording) = 0;
};


class RecordPagePrivate;
class RecordPage : public QWidget
{
    Q_OBJECT

public:
    RecordPage(QSharedPointer<IRecord> record, QWidget *parent = 0);
    virtual ~RecordPage();
    QAction *visibilityAction();

public slots:
    void clientActived(bool actived);

signals:
    void startRecording(const QString &filename);

private slots:
    void onChangeDirectory();
    void onRecordClicked();

protected:
    const QScopedPointer<RecordPagePrivate> d_ptr;

private:
    void createWidgets(QSharedPointer<IRecord> client);

    Q_DISABLE_COPY(RecordPage)
    Q_DECLARE_PRIVATE(RecordPage)

};

#endif // RECORDPAGE_H
