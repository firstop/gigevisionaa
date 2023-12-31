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

#ifndef GVSPRECEIVER_P_H
#define GVSPRECEIVER_P_H

#include <QHostAddress>
#include <QSharedPointer>

class GvspClient;
class GvspPacket;
class GvspBlock;
struct BlockDesc;
struct tpacket_req3;

class GvspReceiverPrivate
{
public:
    GvspReceiverPrivate(QSharedPointer<GvspClient> client);
    QSharedPointer<GvspClient> client;  // le consommateur de paquets GVSP
    volatile bool run;                  // contrôle la sortie de boucle

    QHostAddress receiver;
    QHostAddress transmitter;
    quint16 receiverPort;

    GvspBlock *block;

    static tpacket_req3 createRing();
    static int setupSocket(const tpacket_req3 &req);
    static quint8 *mapRing(int sd, const tpacket_req3 &req);
    static void setUdpPortFilter(int sd, quint16 port);
    static bool bindPacketSocket(int sd, int nicIndex);
    static void setRealtime();
    static int nicIndexFromAddress(const QHostAddress &address);

    void userStack(int socketDescriptor);
    void ringStack(quint16 port);
    void doGvspPacket(const GvspPacket &gvsp);
    void doBlock(const BlockDesc *block);


}; // GvspReceiverPrivate

#endif // GVSPRECEIVER_P_H
