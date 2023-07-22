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

#include "rtppage.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QNetworkInterface>
#include <QPushButton>

RtpPage::RtpPage(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(trUtf8("Diffusion RTP"));
}

void RtpPage::setClient(IRtp *client)
{
    QCheckBox *enable = new QCheckBox;
    enable->setChecked(false);

    m_bindAddress = new QComboBox;
    foreach (QNetworkInterface ni, QNetworkInterface::allInterfaces()) {
        foreach (QNetworkAddressEntry ae, ni.addressEntries()) {
            if (ae.ip().protocol() == QAbstractSocket::IPv4Protocol
                && ae.ip() != QHostAddress::LocalHost) {

                m_bindAddress->addItem(ae.ip().toString());
            }
        }
    }

    m_port = new QSpinBox;
    m_port->setRange(5000, 65535);
    m_port->setValue(5000);

    m_streamButton = new QPushButton;


    QFormLayout * mainLayout = new QFormLayout;
    mainLayout->addRow(trUtf8("Activer la diffusion :"), enable);
    mainLayout->addRow(trUtf8("Source du flux :"), m_bindAddress);
    mainLayout->addRow(trUtf8("Port de diffusion :"), m_port);
    mainLayout->addRow(trUtf8("Lancer la diffusion :"), m_streamButton);

    setLayout(mainLayout);

    connect(this, SIGNAL(startStream(RtpDesc)),
            client->qObject(), SLOT(toggleRtpStream(RtpDesc)));
    connect(m_streamButton, SIGNAL(clicked()),
            this, SLOT(onStartClicked()));
    connect(client->qObject(), SIGNAL(streaming(bool)),
            m_streamButton, SLOT(setActive(bool)));
}

void RtpPage::onStartClicked()
{
    QHostAddress ad(m_bindAddress->currentText());
    if (!ad.isNull()) {
        RtpDesc desc;
        desc.bindAddress = ad.toIPv4Address();
        desc.streamPort = m_port->value();
        emit startStream(desc);
    }
}


