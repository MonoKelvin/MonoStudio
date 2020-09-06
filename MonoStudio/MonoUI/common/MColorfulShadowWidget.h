/****************************************************************************
 * @file    MColorfulShadowWidget.h
 * @date    2020-09-05
 * @author  MonoKelvin
 * @email   15007083506@qq.com
 * @github  https://github.com/MonoKelvin
 * @brief
 *
 * This source file is part of MonoUI.
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
#ifndef MCOLORFULSHADOWWIDGET_H
#define MCOLORFULSHADOWWIDGET_H

#include "MonoUI.h"

namespace mui
{
    /**
     * @brief 彩色阴影附加器, 常用在头像控件中。指定该控件为父对象，则子控件将会有彩色阴影，阴影参数通过`SColorfulShadowParam`控制
     * @example
     *
     * MColorfulShadowWidget *csWidget = new MColorfulShadowWidget(mainWindow);
     * SColorfulShadowParam shadowParam = {0.5, 10.0, 0.0, 10.0};
     * QWidget *myAvatar = new QWidget(shadowParam, csWidget);
     *
     */
    class MONOUI_API MColorfulShadowAttacher : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QGraphicsBlurEffect::BlurHints blurHints READ betBlurHints WRITE setBlurHints)
        Q_PROPERTY(qreal blurRadius READ getBlurRadius WRITE setBlurRadius NOTIFY blurRadiusChanged)
        Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity NOTIFY opacityChanged)
        Q_PROPERTY(qreal xOffset READ getXOffset WRITE setXOffset NOTIFY xOffsetChanged)
        Q_PROPERTY(qreal yOffset READ getYOffset WRITE setYOffset NOTIFY yOffsetChanged)

    public:
        explicit MColorfulShadowAttacher(QWidget* parent);
        MColorfulShadowAttacher(const SColorfulShadowParam& shadowParam, QWidget* parent);

        ~MColorfulShadowAttacher(void);

        inline qreal getBlurRadius(void) const noexcept
        {
            return mShadowParam.Radius;
        }

        inline void setBlurRadius(qreal blurRadius) noexcept
        {
            mShadowParam.Radius = blurRadius;
        }

        inline QGraphicsBlurEffect::BlurHints getBlurHints(void) const noexcept
        {
            return mShadowParam.BlurHints;
        }

        inline void setBlurHints(QGraphicsBlurEffect::BlurHints blurHints) noexcept
        {
            mShadowParam.BlurHints = blurHints;
        }

        inline qreal getXOffset(void) const noexcept
        {
            return mShadowParam.XOffset;
        }

        inline void setXOffset(qreal xOffset) noexcept
        {
            mShadowParam.XOffset = xOffset;
        }

        inline qreal getYOffset(void) const noexcept
        {
            return mShadowParam.YOffset;
        }

        inline void setYOffset(qreal yOffset) noexcept
        {
            mShadowParam.YOffset = yOffset;
        }

        inline qreal getOpacity(void) const noexcept
        {
            return mShadowParam.Opacity;
        }

        inline void setOpacity(qreal opacity) noexcept
        {
            mShadowParam.Opacity = opacity;
        }

    Q_SIGNALS:
        void blurRadiusChanged(void);
        void opacityChanged(void);
        void xOffsetChanged(void);
        void yOffsetChanged(void);

    public:
        bool eventFilter(QObject* watched, QEvent* event);

    private:
        SColorfulShadowParam mShadowParam;
        QWidget* mShadowWidget;
    };

}

#endif // MCOLORFULSHADOWWIDGET_H
