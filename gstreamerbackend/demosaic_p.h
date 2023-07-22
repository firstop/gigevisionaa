/***************************************************************************
 *   Copyright (C) 2014 by Cyril BALETAUD                                  *
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

#ifndef DEMOSAIC_P_H
#define DEMOSAIC_P_H

#include <QHash>
#include <boost/function.hpp>
#include <glib.h>

typedef boost::function<void(const unsigned char*, unsigned char* , int)> Functor;

struct RGB_Buffer
{
    RGB_Buffer() : width(0), height(0), pixelsCount(0), data(NULL), functor(NULL), haveFuntor(false) {}

    void initType(int w, int h, const Functor &f)
    {
        width = w;
        height = h;
        pixelsCount  = h*w;
        data = static_cast<uchar *>(g_malloc(pixelsCount*3));
        functor = f;
        haveFuntor = (f != NULL);
    }

    bool needTransform() const {
        return haveFuntor;
    }

    void buffer2U8(const unsigned char *in, unsigned char *out) {
        functor(in, out, width*height);
    }


    int width;
    int height;
    int pixelsCount;
    uchar *data;
private:
    Functor functor;
    bool haveFuntor;

};

class DemosaicPrivate
{
public:
    QHash<char*, RGB_Buffer> hash;
};


#endif // DEMOSAIC_P_H
