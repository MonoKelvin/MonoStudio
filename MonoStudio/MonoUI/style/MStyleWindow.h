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

#define MUI_WINDOW      "mui_window"
#define MUI_WINDOW_BG   "mui_window_bg"

class QBoxLayout;

namespace mui
{
    class MStyleTitleBar;

    class MONOUI_API MStyleWindow : public QWidget
    {
        Q_OBJECT
    public:
        explicit MStyleWindow(QWidget* parent = nullptr, Qt::WindowType type = Qt::Window);

        virtual ~MStyleWindow();

        inline bool getResizable(void) const noexcept
        {
            return mIsResizable;
        }

        inline void setResizable(bool resizable) noexcept
        {
            mIsResizable = resizable;
        }

        inline bool getMovable() const noexcept
        {
            return mIsMovable;
        }

        /**
         * @brief 设置窗口移动属性
         * @param movable 是否可以移动窗口
         *  - true: 如果窗口没有标题栏则设置为true也无效
         *  - false: 让窗口无法移动
         */
        void setMovable(bool movable);

        void setShadow(const SShadowParam& shadowParam);

        inline const SShadowParam getShadow(void) const noexcept
        {
            return mShadowParam;
        }

        /**
         * @brief 设置标题栏
         * @param titleBar MonoUI风格标题栏控件
         * @note
         *  - titleBar可以设置为nullptr，这样标题栏就会被自动删除
         *  - 设置一个空的标题栏后窗口就不可移动了
         *  - 设置一个有效的标题栏后窗口会激活移动属性，要想保持不移动，需要调用setMovable(false)
         */
        void setTitleBar(MStyleTitleBar* titleBar);

        inline MStyleTitleBar* getTitleBar(void) const noexcept
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
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        void changeEvent(QEvent* event) override;
        bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

    protected:
        // 内容控件
        QWidget* mContent;

    private:
        void _updateResizeType(int ex, int ey);

    private:
        enum EResizeType {
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

        // 阴影参数
        SShadowParam mShadowParam;

        // 圆角参数
        SBorder<int> mBorderRadius;

        // 原尺寸
        QRect mOldGeometry;

        // 阴影背景控件
        QWidget* mShadowBg;

        // 主要布局
        QBoxLayout* mMainLayout;

        // 标题栏
        MStyleTitleBar* mTitleBar;

        // 鼠标按下的相对于整个屏幕的位置
        QPoint mMousePressed;

        // 重置尺寸的类型
        EResizeType mResizeType;

        // 是否可以缩放尺寸
        bool mIsResizable;

        // 是否可以移动
        bool mIsMovable;

        // 是否准备缩放尺寸
        bool mIsResizeReady;
    };

} // namespace mui

#endif // MSTYLEWINDOW_H
