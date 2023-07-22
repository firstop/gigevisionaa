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

#include "demosaic.h"
#include "demosaic_p.h"
#include "gvspclient.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <gst/gst.h>
#include <omp.h>

//#include "tmmintrin.h"

#include <time.h>

#include <QDebug>

const double U12_TO_U8 = 255./4095.;
const double U16_TO_U8 = 255./65535.;
#define SIMD_CHAR_AS_LONGLONG(a) (((long long)a) & 0xFF)
#define SIMD_LL_SETR_EPI8(a, b, c, d, e, f, g, h) \
    SIMD_CHAR_AS_LONGLONG(a) | (SIMD_CHAR_AS_LONGLONG(b) << 8) | \
    (SIMD_CHAR_AS_LONGLONG(c) << 16) | (SIMD_CHAR_AS_LONGLONG(d) << 24) | \
    (SIMD_CHAR_AS_LONGLONG(e) << 32) | (SIMD_CHAR_AS_LONGLONG(f) << 40) | \
    (SIMD_CHAR_AS_LONGLONG(g) << 48) | (SIMD_CHAR_AS_LONGLONG(h) << 56)
#define SIMD_MM_SETR_EPI8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab, ac, ad, ae, af) \
{SIMD_LL_SETR_EPI8(a0, a1, a2, a3, a4, a5, a6, a7), SIMD_LL_SETR_EPI8(a8, a9, aa, ab, ac, ad, ae, af)}

const __m128i FIRST_MASK = SIMD_MM_SETR_EPI8(0x0,0x2,0x4,0x6,0x8,0xA,0xC,0xE,-1,-1,-1,-1,-1,-1,-1,-1);
const __m128i SECOND_MASK = SIMD_MM_SETR_EPI8(-1,-1,-1,-1,-1,-1,-1,-1,0x0,0x2,0x4,0x6,0x8,0xA,0xC,0xE);
const __m128i PACKED_FIRST_MASK = SIMD_MM_SETR_EPI8(0x0,0x2,0x3,0x5,0x6,0x8,0x9,0xB,0xC,0xE,0xF,-1,-1,-1,-1,-1);
const __m128i PACKED_SECOND_MASK = SIMD_MM_SETR_EPI8(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0x1,0x2,0x4,0x5,0x7);
const __m128i PACKED_THIRD_MASK = SIMD_MM_SETR_EPI8(0x8,0xA,0xB,0xD,0xE,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
const __m128i PACKED_FOURTH_MASK = SIMD_MM_SETR_EPI8(-1,-1,-1,-1,-1,0x0,0x1,0x3,0x4,0x6,0x7,0x9,0xA,0xC,0xD,0xF);

bool isMono(quint32 format) {
    return (format < GVSP_PIX_COLOR);
}

bool isBayer(quint32 format) {
    if (isMono(format)) {
        switch (format) {
        case GVSP_PIX_BAYGR8:
        case GVSP_PIX_BAYRG8:
        case GVSP_PIX_BAYGB8:
        case GVSP_PIX_BAYBG8:
        case GVSP_PIX_BAYGR10:
        case GVSP_PIX_BAYRG10:
        case GVSP_PIX_BAYGB10:
        case GVSP_PIX_BAYBG10:
        case GVSP_PIX_BAYGR12:
        case GVSP_PIX_BAYRG12:
        case GVSP_PIX_BAYGB12:
        case GVSP_PIX_BAYBG12:
        case GVSP_PIX_BAYGR10_PACKED:
        case GVSP_PIX_BAYRG10_PACKED:
        case GVSP_PIX_BAYGB10_PACKED:
        case GVSP_PIX_BAYBG10_PACKED:
        case GVSP_PIX_BAYGR12_PACKED:
        case GVSP_PIX_BAYRG12_PACKED:
        case GVSP_PIX_BAYGB12_PACKED:
        case GVSP_PIX_BAYBG12_PACKED:
        case GVSP_PIX_BAYGR16:
        case GVSP_PIX_BAYRG16:
        case GVSP_PIX_BAYGB16:
        case GVSP_PIX_BAYBG16:
            return true;
        }
    }
    return false;
}


//static __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
//_mm_shuffle_epi8(__m128i a, __m128i mask)
//{
//    __m128i result;
//    __asm__("pshufb %1, %0"
//    : "=x" (result)
//            : "xm" (mask), "0" (a));
//    return result;
//}

struct U122U8
{
    void operator()(const unsigned char* src, unsigned char* dst, int pixelsCount) const
    {
        const quint16 *pIn = reinterpret_cast<const quint16 *>(src);
        for (int i=0; i< pixelsCount; i+=16) {
            // charge 8 pixels allignés
            // désature en shiftant à droite
            // réarrange les octets
            const __m128i first = _mm_shuffle_epi8 (
                                      _mm_srli_epi16(
                                          _mm_load_si128((const __m128i *)(pIn + i)), 4),
                                      FIRST_MASK);

            // même chose pour les 8 pixels suivants
            const __m128i second = _mm_shuffle_epi8 (
                                       _mm_srli_epi16(
                                           _mm_load_si128((const __m128i *)(pIn + i + 8)), 4),
                                       SECOND_MASK);

            // bitwise OR
            _mm_storeu_si128((__m128i *)(dst + i), _mm_or_si128(first, second));
        }
        int next = pixelsCount - (pixelsCount % 16);
        while (next < pixelsCount) {
            dst[next] = pIn[next] / 16;
            ++next;
        }
    }
};

struct U12PACKED2U8
{
    void operator()(const unsigned char* src, unsigned char* dst, int pixelsCount) const
    {
        for (int i=0; i< pixelsCount; i+=32) {
            const int j = (3*i)>>1;

            // charge et distribue les 16 premiers octets
            const __m128i first = _mm_shuffle_epi8(
                                      _mm_load_si128((const __m128i *)(src + j)),
                                      PACKED_FIRST_MASK);
            // charge les 16 suivants (contient une partie des 2 à rendre)
            const __m128i second = _mm_load_si128((const __m128i *)(src + j + 16));
            // charge et distibue les 16 suivants
            const __m128i third = _mm_shuffle_epi8(
                                      _mm_load_si128((const __m128i *)(src + j + 32)),
                                      PACKED_FOURTH_MASK);

            // bitwise OU avec la bonne partie du deuxième
            _mm_storeu_si128((__m128i *)(dst + i),
                             _mm_or_si128(
                                 first,
                                 _mm_shuffle_epi8(second, PACKED_SECOND_MASK)));
            // bitwise OU avec la bonne partie du deuxième
            _mm_storeu_si128((__m128i *)(dst + i +16 ),
                             _mm_or_si128(
                                 _mm_shuffle_epi8(second, PACKED_THIRD_MASK),
                                 third));
        }

        int next = pixelsCount - (pixelsCount % 32);
        for(int i=next; i<pixelsCount; i+=2) {
            const int j = (3*i)>>1;
            dst[i] = src[j];
            dst[i+1] = src[j+2];
        }
    }
};

Demosaic::Demosaic()
    : d_ptr(new DemosaicPrivate)
{}

Demosaic::~Demosaic()
{}

void Demosaic::allocate(char *src, int width, int height, quint32 format)
{
    Q_D(Demosaic);
    // alloue un tampon RGB pour le demosaic
    if (isBayer(format)) {
        RGB_Buffer rgb;
        switch (format) {
        case GVSP_PIX_BAYGR8:
        case GVSP_PIX_BAYRG8:
        case GVSP_PIX_BAYGB8:
        case GVSP_PIX_BAYBG8:
            rgb.initType(width, height, NULL);
            break;
        case GVSP_PIX_BAYGR12:
        case GVSP_PIX_BAYRG12:
        case GVSP_PIX_BAYGB12:
        case GVSP_PIX_BAYBG12:
            rgb.initType(width, height, U122U8());
            break;
        case GVSP_PIX_BAYGR12_PACKED:
        case GVSP_PIX_BAYRG12_PACKED:
        case GVSP_PIX_BAYGB12_PACKED:
        case GVSP_PIX_BAYBG12_PACKED:
            rgb.initType(width, height, U12PACKED2U8());
            break;

        default:
            qDebug("is rien");
        }


        d->hash.insert(src, rgb);
    }
}


GstBuffer *Demosaic::transform(char * inBuf)
{
    Q_D(Demosaic);
    RGB_Buffer rgb = d->hash.take(inBuf);

    // transformation
    if (rgb.needTransform()) {
        // alloue un tampon temporaire pour la conversion vers U8
        uchar tmp[rgb.pixelsCount];
        rgb.buffer2U8(reinterpret_cast<uchar *>(inBuf), tmp);
        cv::cvtColor(cv::Mat(rgb.height, rgb.width, CV_8U, tmp),
                     cv::Mat(rgb.height, rgb.width, CV_8UC3, rgb.data),
                     CV_BayerGB2RGB);
    }
    else {
        cv::cvtColor(cv::Mat(rgb.height, rgb.width, CV_8U, inBuf),
                     cv::Mat(rgb.height, rgb.width, CV_8UC3, rgb.data),
                     CV_BayerGB2RGB);
    }
    // libère le tampon bayer
    g_free(inBuf);
    // retourne un GstBuffer RGB
    return gst_buffer_new_wrapped(rgb.data, rgb.pixelsCount * 3);
}

bool Demosaic::needTranform(char *src) const
{
    Q_D(const Demosaic);
    return d->hash.contains(src);
}

GstCaps *Demosaic::capsForTransform(char *src) const
{
    Q_D(const Demosaic);
    RGB_Buffer rgb = d->hash[src];
    return gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "RGB",
                               "width", G_TYPE_INT, rgb.width,
                               "height", G_TYPE_INT, rgb.height,
                               "framerate", GST_TYPE_FRACTION, 0, 1,
                               NULL);
}





