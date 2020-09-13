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
#include "qimageblur.h"

QT_BEGIN_NAMESPACE

QImage qt_halfScaled(const QImage& source)
{
    if (source.width() < 2 || source.height() < 2)
    {
        return QImage();
    }

    QImage srcImage = source;

    if (source.format() == QImage::Format_Indexed8 || source.format() == QImage::Format_Grayscale8)
    {
        // assumes grayscale
        QImage dest(source.width() / 2, source.height() / 2, srcImage.format());
        dest.setDevicePixelRatio(source.devicePixelRatioF());

        const uchar* src = reinterpret_cast<const uchar*>(const_cast<const QImage&>(srcImage).bits());
        int sx = srcImage.bytesPerLine();
        int sx2 = sx << 1;

        uchar* dst = reinterpret_cast<uchar*>(dest.bits());
        int dx = dest.bytesPerLine();
        int ww = dest.width();
        int hh = dest.height();

        for (int y = hh; y; --y, dst += dx, src += sx2)
        {
            const uchar* p1 = src;
            const uchar* p2 = src + sx;
            uchar* q = dst;
            for (int x = ww; x; --x, ++q, p1 += 2, p2 += 2)
            {
                *q = ((int(p1[0]) + int(p1[1]) + int(p2[0]) + int(p2[1])) + 2) >> 2;
            }
        }

        return dest;
    }
    else if (source.format() == QImage::Format_ARGB8565_Premultiplied)
    {
        QImage dest(source.width() / 2, source.height() / 2, srcImage.format());
        dest.setDevicePixelRatio(source.devicePixelRatioF());

        const uchar* src = reinterpret_cast<const uchar*>(const_cast<const QImage&>(srcImage).bits());
        int sx = srcImage.bytesPerLine();
        int sx2 = sx << 1;

        uchar* dst = reinterpret_cast<uchar*>(dest.bits());
        int dx = dest.bytesPerLine();
        int ww = dest.width();
        int hh = dest.height();

        for (int y = hh; y; --y, dst += dx, src += sx2)
        {
            const uchar* p1 = src;
            const uchar* p2 = src + sx;
            uchar* q = dst;
            for (int x = ww; x; --x, q += 3, p1 += 6, p2 += 6)
            {
                // alpha
                q[0] = AVG(AVG(p1[0], p1[3]), AVG(p2[0], p2[3]));
                // rgb
                const quint16 p16_1 = (p1[2] << 8) | p1[1];
                const quint16 p16_2 = (p1[5] << 8) | p1[4];
                const quint16 p16_3 = (p2[2] << 8) | p2[1];
                const quint16 p16_4 = (p2[5] << 8) | p2[4];
                const quint16 result = AVG16(AVG16(p16_1, p16_2), AVG16(p16_3, p16_4));
                q[1] = result & 0xff;
                q[2] = result >> 8;
            }
        }

        return dest;
    }
    else if (source.format() != QImage::Format_ARGB32_Premultiplied && source.format() != QImage::Format_RGB32)
    {
        srcImage = source.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }

    QImage dest(source.width() / 2, source.height() / 2, srcImage.format());
    dest.setDevicePixelRatio(source.devicePixelRatioF());

    const quint32* src = reinterpret_cast<const quint32*>(const_cast<const QImage&>(srcImage).bits());
    int sx = srcImage.bytesPerLine() >> 2;
    int sx2 = sx << 1;

    quint32* dst = reinterpret_cast<quint32*>(dest.bits());
    int dx = dest.bytesPerLine() >> 2;
    int ww = dest.width();
    int hh = dest.height();

    for (int y = hh; y; --y, dst += dx, src += sx2)
    {
        const quint32* p1 = src;
        const quint32* p2 = src + sx;
        quint32* q = dst;
        for (int x = ww; x; --x, q++, p1 += 2, p2 += 2)
        {
            *q = AVG(AVG(p1[0], p1[1]), AVG(p2[0], p2[1]));
        }
    }

    return dest;
}

void qt_blurImage(QPainter* p, QImage& blurImage, qreal radius, bool quality, bool alphaOnly, int transposed)
{
    if (blurImage.format() != QImage::Format_ARGB32_Premultiplied && blurImage.format() != QImage::Format_RGB32)
    {
        blurImage = blurImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }

    qreal scale = 1;
    if (radius >= 4 && blurImage.width() >= 2 && blurImage.height() >= 2)
    {
        blurImage = qt_halfScaled(blurImage);
        scale = 2;
        radius *= qreal(0.5);
    }

    if (alphaOnly)
    {
        expblur<12, 10, true>(blurImage, radius, quality, transposed);
    }
    else
    {
        expblur<12, 10, false>(blurImage, radius, quality, transposed);
    }

    if (p)
    {
        p->scale(scale, scale);
        p->setRenderHint(QPainter::SmoothPixmapTransform);
        p->drawImage(QRect(QPoint(0, 0), blurImage.size() / blurImage.devicePixelRatioF()), blurImage);
    }
}

void qt_blurImage(QImage& blurImage, qreal radius, bool quality, int transposed)
{
    if (blurImage.format() == QImage::Format_Indexed8 || blurImage.format() == QImage::Format_Grayscale8)
    {
        expblur<12, 10, true>(blurImage, radius, quality, transposed);
    }
    else
    {
        expblur<12, 10, false>(blurImage, radius, quality, transposed);
    }
}

void grayscale(const QImage& image, QImage& dest, const QRect& rect)
{
    QRect destRect = rect;
    QRect srcRect = rect;
    if (rect.isNull())
    {
        srcRect = dest.rect();
        destRect = dest.rect();
    }
    if (&image != &dest)
    {
        destRect.moveTo(QPoint(0, 0));
    }

    const unsigned int* data = (const unsigned int*)image.bits();
    unsigned int* outData = (unsigned int*)dest.bits();

    if (dest.size() == image.size() && image.rect() == srcRect)
    {
        // a bit faster loop for grayscaling everything
        int pixels = dest.width() * dest.height();
        for (int i = 0; i < pixels; ++i)
        {
            int val = qGray(data[i]);
            outData[i] = qRgba(val, val, val, qAlpha(data[i]));
        }
    }
    else
    {
        int yd = destRect.top();
        for (int y = srcRect.top(); y <= srcRect.bottom() && y < image.height(); y++)
        {
            data = (const unsigned int*)image.scanLine(y);
            outData = (unsigned int*)dest.scanLine(yd++);
            int xd = destRect.left();
            for (int x = srcRect.left(); x <= srcRect.right() && x < image.width(); x++)
            {
                int val = qGray(data[x]);
                outData[xd++] = qRgba(val, val, val, qAlpha(data[x]));
            }
        }
    }
}

QT_END_NAMESPACE
