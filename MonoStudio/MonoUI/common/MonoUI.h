/****************************************************************************
 * @file    MonoUI.h
 * @date    2020-09-05
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
#ifndef MONOUI_H
#define MONOUI_H

#include <qnamespace.h>

#include "defines.h"

class QLabel;

namespace mui
{
    /**
     * @brief UI状态类型
     */
    enum EUIStateHint {
        // 静态，控件初始设置后不会进行重绘和重新设置geometry等属性
        StaticUI,

        // 半静态，控件只在geometry发生变化才重绘或更新状态
        SemiStaticUI,

        // 动态，控件在geometry发生变化、内容变化等状态属性变化时就进行更新
        DynamicUI,

        // 手动更新，只有手动执行控件相关更新方法时才更新
        ManualUpdate,

        // 自动更新，由系统自动决定是否需要更新
        AutoUpdate,
    };

    /** @brief 阴影参数 */
    struct SShadowParam {
        /** @brief 阴影颜色 */
        QColor color;

        /** @brief 模糊半径 */
        qreal radius;

        /** @brief X方向偏移 */
        qreal xOffset;

        /** @brief Y方向偏移 */
        qreal yOffset;

        /** @brief 是否启用 */
        bool isEnabled;

        SShadowParam(void) noexcept
            : color("#191919")
            , radius(15.0)
            , xOffset(0.0)
            , yOffset(0.0)
            , isEnabled(true)
        {}
    };

    /** @brief 边角参数 */
    template<class _Ty>
    struct SBorder {
        _Ty topLeft;
        _Ty topRight;
        _Ty bottomLeft;
        _Ty bottomRight;

        SBorder(void) noexcept
            : topLeft(0)
            , topRight(0)
            , bottomLeft(0)
            , bottomRight(0)
        {}

        SBorder(_Ty value) noexcept
            : topLeft(value)
            , topRight(value)
            , bottomLeft(value)
            , bottomRight(value)
        {}

        SBorder(_Ty tl, _Ty tr, _Ty bl, _Ty br) noexcept
            : topLeft(tl)
            , topRight(tr)
            , bottomLeft(bl)
            , bottomRight(br)
        {}

        inline bool isValid(void) const noexcept
        {
            return (topLeft > 0) && (topRight > 0) && (bottomLeft > 0) && (bottomRight > 0);
        }
    };

    /**
     * @brief 为QLabel控件设置长文本时的省略字符串
     * @param label 给定的QLabel控件。
     * @param maxWidth 最大宽度。当文本内容像素宽度超过该值时就省略显示，小于0表示最大宽度为此时的label宽度
     * @param mode 省略模式。默认右侧省略
     */
    void MONOUI_API ElidedTextForLabel(QLabel* label, int maxWidth = -1, Qt::TextElideMode mode = Qt::ElideRight);

    /**
     * @brief 绘制圆角矩形
     * @param painter 画笔
     * @param rect 矩形
     * @param lt 左上角圆弧
     * @param rt 右上角圆弧
     * @param lb 左下角圆弧
     * @param rb 右下角圆弧
     */
    void MONOUI_API DrawRoundRect(QPainter* painter, const QRect& rect, int lt = 10,
                                  int rt = 10, int lb = 10, int rb = 10);

    /**
     * @brief 绘制背景图片到控件上
     * @param widget 控件
     * @param pixmap 图片
     * @note 绘制的图片不会进行缩放、适应或拉伸等
     */
    inline void DrawBackground(QWidget* widget, const QPixmap& pixmap) noexcept
    {
        QPalette pal = widget->palette();
        pal.setBrush(QPalette::Window, QBrush(pixmap));
        widget->setPalette(pal);
    }
}

#endif // MONOUI_H
