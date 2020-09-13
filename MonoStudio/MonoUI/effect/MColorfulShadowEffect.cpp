/****************************************************************************
 * @file    MColorfulShadowEffect.cpp
 * @date    2020-09-13
 * @author  MonoKelvin
 * @email   15007083506@qq.com
 * @github  https://github.com/MonoKelvin
 * @brief
 *
 * This source file is part of MonoStudio.
 * Copyright (C) 2020 by MonoKelvin. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
#include "MColorfulShadowEffect.h"

#include "../common/qimageblur.h"
#include<QDebug>
namespace mui
{
    namespace effect
    {
        MColorfulShadowEffect::MColorfulShadowEffect(QObject* parent)
            : QGraphicsEffect(parent)
            , mOffset(0, 15.0)
            , mOpacity(1.0)
            , mBlurRadius(20.0)
            , mBlurHints(PerformanceHint)
        {
        }

        MColorfulShadowEffect::~MColorfulShadowEffect()
        {
        }

        QRectF MColorfulShadowEffect::boundingRectFor(const QRectF& rect) const
        {
            const qreal delta = M_BLUR_RADIUS_SCALE * mBlurRadius + 1;
            return rect.united(rect.translated(mOffset).adjusted(-delta, -delta, delta, delta));
        }

        void MColorfulShadowEffect::setOffset(const QPointF& offset)
        {
            if (mOffset != offset)
            {
                mOffset = offset;
                updateBoundingRect();
                emit offsetChanged();
            }
        }

        void MColorfulShadowEffect::setOffset(qreal x, qreal y)
        {
            if (mOffset.x() != x || mOffset.y() != y)
            {
                mOffset.setX(x);
                mOffset.setY(y);
                updateBoundingRect();
                emit offsetChanged();
            }
        }

        void MColorfulShadowEffect::setOpacity(qreal opacity)
        {
            if (!qFuzzyCompare(mOpacity, opacity))
            {
                mOpacity = opacity;
                updateBoundingRect();
                emit opacityChanged();
            }
        }

        void MColorfulShadowEffect::setBlurRadius(qreal blurRadius)
        {
            if (!qFuzzyCompare(mBlurRadius, blurRadius))
            {
                mBlurRadius = blurRadius;
                updateBoundingRect();
                emit opacityChanged();
            }
        }

        void MColorfulShadowEffect::setBlurHints(const FBlurHints& blurHints)
        {
            if (mBlurHints != blurHints)
            {
                mBlurHints = blurHints;
                updateBoundingRect();
                emit blurHintsChanged();
            }
        }

        void MColorfulShadowEffect::draw(QPainter* painter)
        {
            if (mBlurRadius <= 1 && mOffset.isNull())
            {
                drawSource(painter);
                return;
            }

            // Draw pixmap in device coordinates to avoid pixmap scaling.
            QPoint offset;
            const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset, PadToEffectiveBoundingRect);
            if (pixmap.isNull())
            {
                return;
            }

            QTransform restoreTransform = painter->worldTransform();
            painter->setWorldTransform(QTransform());
            draw(painter, offset, pixmap);
            painter->setWorldTransform(restoreTransform);
        }

        void MColorfulShadowEffect::draw(QPainter* p, const QPointF& pos, const QPixmap& px, const QRectF& rect)
        {
            if (px.isNull() || !p->isActive())
            {
                return;
            }

            QRectF srcRect = rect;
            if (srcRect.isNull())
            {
                srcRect = px.rect();
            }

            if (mBlurRadius <= 1)
            {
                p->drawPixmap(srcRect.translated(pos), px, srcRect);
                return;
            }

            QImage srcImage;
            if (srcRect == px.rect())
            {
                srcImage = px.toImage();
            }
            else
            {
                const QRect cpyRect = srcRect.toAlignedRect().intersected(px.rect());
                srcImage = px.copy(cpyRect).toImage();
            }

            const QTransform transform = p->worldTransform();
            const qreal oldOpacity = p->opacity();

            p->translate(pos + mOffset);
            p->setOpacity(mOpacity);

            qt_blurImage(p, srcImage, M_BLUR_RADIUS_SCALE * mBlurRadius, (mBlurHints & QualityHint), false);

            p->setOpacity(oldOpacity);
            p->setWorldTransform(transform);

            // Draw the actual pixmap...
            p->drawPixmap(pos, px, rect);
        }

    } // namespace effect
} // namespace mui
