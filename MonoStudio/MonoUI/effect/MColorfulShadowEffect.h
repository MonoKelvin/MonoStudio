/****************************************************************************
 * @file    MColorfulShadowEffect.h
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
#ifndef MCOLORFULSHADOWEFFECT_H
#define MCOLORFULSHADOWEFFECT_H

#include "defines.h"
#include <QGraphicsEffect>

/** @brief 模糊半径的缩放率 */
#define M_BLUR_RADIUS_SCALE qreal(2.5)

namespace mui
{
    namespace effect
    {
        /**
         * @brief 彩色阴影特效
         */
        class MONOUI_API MColorfulShadowEffect : public QGraphicsEffect
        {
            Q_OBJECT
            Q_PROPERTY(QPointF offset READ getOffset WRITE setOffset NOTIFY offsetChanged)
            Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity NOTIFY opacityChanged)
            Q_PROPERTY(qreal blurRadius READ getBlurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
            Q_PROPERTY(FBlurHints blurHints READ getBlurHints WRITE setBlurHints NOTIFY blurHintsChanged)
        public:
            /**
             * @brief 模糊策略
             * @see QGraphicsBlurEffect::BlurHint
             */
            enum EBlurHint
            {
                PerformanceHint = 0x00,
                QualityHint = 0x01,
                AnimationHint = 0x02
            };
            Q_FLAG(EBlurHint)
            Q_DECLARE_FLAGS(FBlurHints, EBlurHint)
            Q_FLAG(FBlurHints)

            MColorfulShadowEffect(QObject* parent = nullptr);
            ~MColorfulShadowEffect();

            QRectF boundingRectFor(const QRectF& rect) const override;

            inline QPointF getOffset(void) const noexcept
            {
                return mOffset;
            }

            void setOffset(const QPointF& offset);
            void setOffset(qreal x, qreal y);

            inline qreal getOpacity(void) const noexcept
            {
                return mOpacity;
            }

            void setOpacity(qreal opacity);

            inline qreal getBlurRadius(void) const noexcept
            {
                return mBlurRadius;
            }

            void setBlurRadius(qreal blurRadius);

            inline FBlurHints getBlurHints() const noexcept
            {
                return mBlurHints;
            }

            void setBlurHints(const FBlurHints& blurHints);

        Q_SIGNALS:
            void offsetChanged(void);
            void opacityChanged(void);
            void blurRadiusChanged(void);
            void blurHintsChanged(void);

        private:
            void draw(QPainter* painter) override;
            void draw(QPainter* p, const QPointF& pos, const QPixmap& px, const QRectF& rect = QRectF());

        private:
            /** @brief 方向偏移 */
            QPointF mOffset;

            /** @brief 模糊透明度 */
            qreal mOpacity;

            /** @brief 阴影半径 */
            qreal mBlurRadius;

            /** @brief 模糊策略 */
            FBlurHints mBlurHints;

        private:
            Q_DISABLE_COPY(MColorfulShadowEffect)
        };

    } // namespace effect
} // namespace mui

#endif // MCOLORFULSHADOWEFFECT_H
