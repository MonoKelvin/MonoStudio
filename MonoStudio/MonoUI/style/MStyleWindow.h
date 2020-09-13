/****************************************************************************
 * @file    MStyleWindow.h
 * @date    2020-08-11
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
#ifndef MSTYLEWINDOW_H
#define MSTYLEWINDOW_H

#include "common/MonoUI.h"

namespace mui
{
    class MTitleBar;

    class MONOUI_API MStyleWindow : public QWidget
    {
        Q_OBJECT
    public:
        explicit MStyleWindow(QWidget* parent = nullptr, Qt::WindowType type = Qt::Window);

        virtual ~MStyleWindow();

        inline bool getResizable(void) const noexcept
        {
            return mResizable;
        }

        inline void setResizable(bool resizable) noexcept
        {
            mResizable = resizable;
        }

        inline bool getMovable() const noexcept
        {
            return mMovable;
        }

        /**
         * @brief 设置窗口移动属性
         * @param movable 是否可以移动窗口
         *  - true: 如果窗口没有标题栏则设置为true也无效
         *  - false: 让窗口无法移动
         */
        inline void setMovable(bool movable) noexcept
        {
            if (movable && nullptr == mTitleBar)
            {
                return;
            }

            mMovable = movable;
        }

        void setShadow(const SShadowParam& shadowParam);

        /**
         * @brief 设置标题栏
         * @param titleBar Mono风格标题栏控件
         * @note
         *  - titleBar可以设置为nullptr，这样标题栏就会被自动删除
         *  - 设置一个空的标题栏后窗口就不可移动了
         *  - 设置一个有效的标题栏后窗口会激活移动属性，要想保持不移动，需要调用setMovable(false)
         */
        void setTitleBar(MTitleBar* titleBar);

        inline MTitleBar* getTitleBar(void) const noexcept
        {
            return mTitleBar;
        }

        /**
         * @brief 设置窗口边框圆角
         * @param radius 圆角像素半径。当该值过大时，边角区域的缩放将会失效
         * @note 尽量使用该方式设置圆角，而不要在样式表中设置，否则在窗口全屏后圆角信息会丢失
         */
        void setBorderRadius(int radius);

        /**
         * @brief 设置窗口边框圆角。
         * @param borderRadius 圆角参数。当该值过大时，对应的边角区域的缩放将会失效
         * @note 尽量使用该方式设置圆角，而不要在样式表中设置，否则在窗口全屏后圆角信息会丢失
         */
        void setBorderRadius(const SBorder<int>& borderRadius);

        inline SBorder<int> getBorderRadius(void) const noexcept
        {
            return mBorderRadius;
        }

    protected:
        void setShadowEnabled(bool enabled);

        bool eventFilter(QObject* watched, QEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;

    protected:
        // 内容控件
        QWidget* mContent;

    private:
        void updateResizeType(QMouseEvent* event);

    private:
        enum EResizeType
        {
            RT_NoResize = 0x00,
            RT_Left = 0x01,
            RT_Right = 0x02,
            RT_Top = 0x04,
            RT_Bottom = 0x08,
            RT_TopLeft = RT_Top | RT_Left,
            RT_TopRight = RT_Top | RT_Right,
            RT_BottomLeft = RT_Bottom | RT_Left,
            RT_BottomRight = RT_Bottom | RT_Right,
        };

        // 阴影be背景控件
        QWidget* mShadowBg;

        // 标题栏
        MTitleBar* mTitleBar;

        // 重置尺寸的类型
        EResizeType mResizeType;

        // 修改窗口大小的边界宽度，最大只能为16
        int mBoundaryWidth;

        // 是否可以缩放尺寸
        bool mResizable;

        // 是否可以移动
        bool mMovable;

        // 是否准备移动
        bool mMoveReady;

        // 是否准备缩放尺寸
        bool mResizeReady;

        // 阴影参数
        SShadowParam mShadowParam;

        // 圆角参数
        SBorder<int> mBorderRadius;

        // 鼠标按下的相对于整个屏幕的位置
        QPoint mMousePressed;

        // 原尺寸
        QRect mOldGeometry;
    };

}

#endif // MSTYLEWINDOW_H
