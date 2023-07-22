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

#ifndef DEMOSAIC_H
#define DEMOSAIC_H

#include <QScopedPointer>

typedef struct _GstCaps GstCaps;
typedef struct _GstBuffer GstBuffer;

namespace cv {class Mat;}
class GvspImage;

class DemosaicPrivate;
class Demosaic {

public:
    Demosaic();
    ~Demosaic();
    bool needDemosaic(quint32 format);
    void allocate(GvspImage &image);
    GstBuffer *transform(GvspImage &image);
    GstCaps *capsForTransform(const GvspImage &image) const;

protected:
    const QScopedPointer<DemosaicPrivate> d;

private:
    Q_DISABLE_COPY(Demosaic)

};

#endif // DEMOSAIC_H
