/****************************************************************************
 * @file    defines.h
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
#ifndef DEFINES_H
#define DEFINES_H

#include <QtCore/qglobal.h>
#include <QWidget>
#include <QStyleOption>
#include <QStylePainter>

#if defined(MONOUI_LIBRARY)
    #define MONOUI_API Q_DECL_EXPORT
#else
    #define MONOUI_API Q_DECL_IMPORT
#endif

/** @brief 通过icon图标名称（不带后缀）找到QIcon */
#define M_ICON(_Name_) QIcon::fromTheme(QStringLiteral(_Name_))

/**
 * @brief 继承自QWidget的类如果不实现自己的paintEvent方法，那么该类就无法使用样式表
 * 改变样式。通过该宏可以在样式表中改变当前控件的样式
 * @note 该宏必须放在`paintEvent`函数中。
 */
#define M_USE_STYLE_SHEET()                            \
{                                                      \
    QStylePainter _Painter_(this);                     \
    QStyleOption _Opt_;                                \
    _Opt_.initFrom(this);                              \
    _Opt_.rect = rect();                               \
    _Painter_.drawPrimitive(QStyle::PE_Widget, _Opt_); \
}

#endif // DEFINES_H
