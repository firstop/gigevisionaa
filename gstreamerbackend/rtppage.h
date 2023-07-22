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

#ifndef RTPPAGE_H
#define RTPPAGE_H

#include <QWidget>

class QComboBox;
class QSpinBox;
class QPushButton;


struct RtpDesc {
    quint32 bindAddress;
    quint32 streamPort;
};

class IRtp
{
    QObject * m_parent;

public:
    explicit IRtp(QObject *parent) : m_parent(parent) {}
    virtual ~IRtp(){}
    QObject * qObject() {return m_parent;}
    virtual void rtpActived(bool actived) = 0;
    virtual void toggleRtpStream(const RtpDesc &desc) = 0;

};

class RtpPage : public QWidget
{
    Q_OBJECT

    QComboBox *m_bindAddress;
    QSpinBox *m_port;
    QPushButton *m_streamButton;

public:
    RtpPage(QWidget *parent = 0);
    void setClient(IRtp *client);

signals:
    void startStream(const RtpDesc &desc);

private slots:
    void onStartClicked();


};

#endif // VIEWPAGE_H
