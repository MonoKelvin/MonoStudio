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

#include <QGraphicsBlurEffect>
#include <qnamespace.h>

namespace mui
{
    /**
     * @brief UI状态类型
     */
    enum EUIStateHint
    {
        // 静态
        StaticUI,

        // 半静态
        SemiStaticUI,

        // 动态
        DynamicUI,

        // 手动更新
        ManualUpdate,

        // 自动更新
        AutoUpdate,
    };

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

    /** @brief 阴影参数 */
    struct MONOUI_API SShadowParam
    {
        /** @brief 阴影颜色 */
        QColor Color;

        /** @brief 模糊半径 */
        qreal Radius;

        /** @brief X方向偏移 */
        qreal XOffset;

        /** @brief Y方向偏移 */
        qreal YOffset;

        SShadowParam(void) noexcept
            : Color("#191919")
            , Radius(20.0)
            , XOffset(0.0)
            , YOffset(0.0)
        {}
    };

    /** @brief 彩色阴影参数 */
    struct MONOUI_API SColorfulShadowParam
    {
        /** @brief 透明度 */
        qreal Opacity;

        /** @brief 模糊半径 */
        qreal Radius;

        /** @brief X方向偏移 */
        qreal XOffset;

        /** @brief Y方向偏移 */
        qreal YOffset;

        QGraphicsBlurEffect::BlurHints BlurHints;

        EUIStateHint UIStateHint;

        SColorfulShadowParam(void) noexcept
            : Opacity(0.6)
            , Radius(20.0)
            , XOffset(0.0)
            , YOffset(0.0)
        {}
    };

    /** @brief 边角参数 */
    template<class _Ty>
    struct MONOUI_API SBorder
    {
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

}

#endif // MONOUI_H
