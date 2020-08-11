/****************************************************************************
 * @file    MUI.h
 * @date    2020-08-11
 * @author  MonoKelvin
 * @email   15007083506@qq.com
 * @github  https://github.com/MonoKelvin
 * @brief
 *
 * This source file is part of MonoToolkit.
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
#ifndef MUI_H
#define MUI_H

#include "MonoToolkit.h"

#include <QStyleOption>
#include <QStylePainter>

MTK_NAMESPACE_BEGIN

/** @brief 通过icon图标名称（不带后缀）找到QIcon */
#define MONO_ICON(_Name_) QIcon::fromTheme(QStringLiteral(_Name_))

/**
 * @brief 继承自QWidget的类如果不实现自己的paintEvent方法，那么该类就无法使用样式表
 * 改变样式。通过该宏可以在样式表中改变当前控件的样式
 * @note 该宏必须放在`paintEvent`函数中。
 */
#define MONO_USE_STYLE_SHEET()                         \
{                                                      \
    QStylePainter _Painter_(this);                     \
    QStyleOption _Opt_;                                \
    _Opt_.initFrom(this);                              \
    _Opt_.rect = rect();                               \
    _Painter_.drawPrimitive(QStyle::PE_Widget, _Opt_); \
}

/**
 * @brief 绘制圆角矩形
 * @param painter 画笔
 * @param rect 矩形
 * @param lt 左上角圆弧
 * @param rt 右上角圆弧
 * @param lb 左下角圆弧
 * @param rb 右下角圆弧
 */
void MONOTOOLKIT_API DrawRoundRect(QPainter* painter, const QRect& rect, int lt = 10,
                                   int rt = 10, int lb = 10, int rb = 10);

/** @brief 阴影参数 */
struct MONOTOOLKIT_API SShadowParam {
    qreal Radius;
    qreal XOffset;
    qreal YOffset;
    QColor Color;

    SShadowParam() noexcept
        : Radius(20.0)
        , XOffset(0.0)
        , YOffset(0.0)
        , Color("#191919")
    {}
};

/** @brief 边角参数 */
template<class _Ty>
struct MONOTOOLKIT_API SBorder {
    _Ty topLeft;
    _Ty topRight;
    _Ty bottomLeft;
    _Ty bottomRight;

    SBorder() noexcept
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

MTK_NAMESPACE_END

#endif // MUI_H
