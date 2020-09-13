/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmemrotate.h"
#include "qmath.h"

QT_BEGIN_NAMESPACE

template <int shift>
inline int qt_static_shift(int value)
{
    if (shift == 0)
    {
        return value;
    }
    else if (shift > 0)
    {
        return value << (uint(shift) & 0x1f);
    }
    else
    {
        return value >> (uint(-shift) & 0x1f);
    }
}

template <int aprec, int zprec>
inline void qt_blurinner(uchar* bptr, int& zR, int& zG, int& zB, int& zA, int alpha)
{
    QRgb* pixel = (QRgb*)bptr;

#define Z_MASK (0xff << zprec)
    const int A_zprec = qt_static_shift < zprec - 24 > (*pixel) & Z_MASK;
    const int R_zprec = qt_static_shift < zprec - 16 > (*pixel) & Z_MASK;
    const int G_zprec = qt_static_shift < zprec - 8 > (*pixel) & Z_MASK;
    const int B_zprec = qt_static_shift<zprec>(*pixel) & Z_MASK;
#undef Z_MASK

    const int zR_zprec = zR >> aprec;
    const int zG_zprec = zG >> aprec;
    const int zB_zprec = zB >> aprec;
    const int zA_zprec = zA >> aprec;

    zR += alpha * (R_zprec - zR_zprec);
    zG += alpha * (G_zprec - zG_zprec);
    zB += alpha * (B_zprec - zB_zprec);
    zA += alpha * (A_zprec - zA_zprec);

#define ZA_MASK (0xff << (zprec + aprec))
    *pixel = qt_static_shift < 24 - zprec - aprec > (zA & ZA_MASK) | qt_static_shift < 16 - zprec - aprec >
             (zR & ZA_MASK) | qt_static_shift < 8 - zprec - aprec > (zG & ZA_MASK) | qt_static_shift < -zprec - aprec >
             (zB & ZA_MASK);
#undef ZA_MASK
}

const int alphaIndex = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

template <int aprec, int zprec>
inline void qt_blurinner_alphaOnly(uchar* bptr, int& z, int alpha)
{
    const int A_zprec = int(*(bptr)) << zprec;
    const int z_zprec = z >> aprec;
    z += alpha * (A_zprec - z_zprec);
    *(bptr) = z >> (zprec + aprec);
}

template <int aprec, int zprec, bool alphaOnly>
inline void qt_blurrow(QImage& im, int line, int alpha)
{
    uchar* bptr = im.scanLine(line);

    int zR = 0, zG = 0, zB = 0, zA = 0;

    if (alphaOnly && im.format() != QImage::Format_Indexed8)
    {
        bptr += alphaIndex;
    }

    const int stride = im.depth() >> 3;
    const int im_width = im.width();
    for (int index = 0; index < im_width; ++index)
    {
        if (alphaOnly)
        {
            qt_blurinner_alphaOnly<aprec, zprec>(bptr, zA, alpha);
        }
        else
        {
            qt_blurinner<aprec, zprec>(bptr, zR, zG, zB, zA, alpha);
        }
        bptr += stride;
    }

    bptr -= stride;

    for (int index = im_width - 2; index >= 0; --index)
    {
        bptr -= stride;
        if (alphaOnly)
        {
            qt_blurinner_alphaOnly<aprec, zprec>(bptr, zA, alpha);
        }
        else
        {
            qt_blurinner<aprec, zprec>(bptr, zR, zG, zB, zA, alpha);
        }
    }
}

/**
 *  指数模糊算法
 *
 *  Based on exponential blur algorithm by Jani Huhtanen
 *
 *  In-place blur of image 'img' with kernel of approximate radius 'radius'.
 *  Blurs with two sided exponential impulse response.
 *
 *  aprec = precision of alpha parameter in fixed-point format 0.aprec
 *  zprec = precision of state parameters zR,zG,zB and zA in fp format 8.zprec
 */
template <int aprec, int zprec, bool alphaOnly>
void expblur(QImage& img, qreal radius, bool improvedQuality = false, int transposed = 0)
{
    // halve the radius if we're using two passes
    if (improvedQuality)
    {
        radius *= qreal(0.5);
    }

    Q_ASSERT(img.format() == QImage::Format_ARGB32_Premultiplied || img.format() == QImage::Format_RGB32
             || img.format() == QImage::Format_Indexed8 || img.format() == QImage::Format_Grayscale8);

    // choose the alpha such that pixels at radius distance from a fully
    // saturated pixel will have an alpha component of no greater than
    // the cutOffIntensity
    const qreal cutOffIntensity = 2;
    const int alpha = radius <= qreal(1e-5)
                      ? ((1 << aprec) - 1)
                      : qRound((1 << aprec) * (1 - qPow(cutOffIntensity * (1 / qreal(255)), 1 / radius)));

    int img_height = img.height();
    for (int row = 0; row < img_height; ++row)
    {
        for (int i = 0; i <= int(improvedQuality); ++i)
        {
            qt_blurrow<aprec, zprec, alphaOnly>(img, row, alpha);
        }
    }

    QImage temp(img.height(), img.width(), img.format());
    temp.setDevicePixelRatio(img.devicePixelRatioF());
    if (transposed >= 0)
    {
        if (img.depth() == 8)
        {
            qt_memrotate270(reinterpret_cast<const quint8*>(img.bits()),
                            img.width(), img.height(), img.bytesPerLine(),
                            reinterpret_cast<quint8*>(temp.bits()),
                            temp.bytesPerLine());
        }
        else
        {
            qt_memrotate270(reinterpret_cast<const quint32*>(img.bits()),
                            img.width(), img.height(), img.bytesPerLine(),
                            reinterpret_cast<quint32*>(temp.bits()),
                            temp.bytesPerLine());
        }
    }
    else
    {
        if (img.depth() == 8)
        {
            qt_memrotate90(reinterpret_cast<const quint8*>(img.bits()),
                           img.width(), img.height(), img.bytesPerLine(),
                           reinterpret_cast<quint8*>(temp.bits()),
                           temp.bytesPerLine());
        }
        else
        {
            qt_memrotate90(reinterpret_cast<const quint32*>(img.bits()),
                           img.width(), img.height(), img.bytesPerLine(),
                           reinterpret_cast<quint32*>(temp.bits()),
                           temp.bytesPerLine());
        }
    }

    img_height = temp.height();
    for (int row = 0; row < img_height; ++row)
    {
        for (int i = 0; i <= int(improvedQuality); ++i)
        {
            qt_blurrow<aprec, zprec, alphaOnly>(temp, row, alpha);
        }
    }

    if (transposed == 0)
    {
        if (img.depth() == 8)
        {
            qt_memrotate90(reinterpret_cast<const quint8*>(temp.bits()),
                           temp.width(), temp.height(), temp.bytesPerLine(),
                           reinterpret_cast<quint8*>(img.bits()),
                           img.bytesPerLine());
        }
        else
        {
            qt_memrotate90(reinterpret_cast<const quint32*>(temp.bits()),
                           temp.width(), temp.height(), temp.bytesPerLine(),
                           reinterpret_cast<quint32*>(img.bits()),
                           img.bytesPerLine());
        }
    }
    else
    {
        img = temp;
    }
}

#define AVG(a, b) (((((a) ^ (b)) & 0xfefefefeUL) >> 1) + ((a) & (b)))
#define AVG16(a, b) (((((a) ^ (b)) & 0xf7deUL) >> 1) + ((a) & (b)))

QImage qt_halfScaled(const QImage& source);

void qt_blurImage(QPainter* p, QImage& blurImage, qreal radius, bool quality, bool alphaOnly,
                  int transposed = 0);

void qt_blurImage(QImage& blurImage, qreal radius, bool quality, int transposed = 0);

// grayscales the image to dest (could be same). If rect isn't defined
// destination image size is used to determine the dimension of grayscaling
// process.
void grayscale(const QImage& image, QImage& dest, const QRect& rect = QRect());

QT_END_NAMESPACE
