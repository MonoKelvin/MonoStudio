/****************************************************************************
 * @file    MonoUI.cpp
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
#include "defines.h"

#include <QPainterPath>
#include <QLabel>

namespace mui
{
    void DrawRoundRect(QPainter* painter, const QRect& rect, int lt, int rt, int lb, int rb)
    {
        if (rect.width() == 0 || rect.height() == 0) {
            return;
        }

        QPainterPath path;

        path.moveTo(rect.bottomRight() - QPointF(0, rb));
        path.lineTo(rect.topRight() + QPointF(0, rt));
        path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(rt * 2.0, 0)), QSize(rt * 2, rt * 2)), 0, 90);
        path.lineTo(rect.topLeft() + QPointF(lt, 0));
        path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(lt * 2, lt * 2)), 90, 90);
        path.lineTo(rect.bottomLeft() - QPointF(0, lb));
        path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, lb * 2.0)), QSize(lb * 2, lb * 2)), 180, 90);
        path.lineTo(rect.bottomLeft() + QPointF(lb, 0));
        path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(lb * 2.0, lb * 2.0)), QSize(lb * 2, lb * 2)), 270, 90);

        painter->fillPath(path, painter->brush());
        painter->drawPath(path);
    }

    void ElidedTextForLabel(QLabel* label, int maxWidth, Qt::TextElideMode mode)
    {
        if (maxWidth < 0) {
            maxWidth = label->width();
        }

        const QFontMetrics fm = label->fontMetrics();
        const QString et = fm.elidedText(label->text(), mode, maxWidth - 3 * fm.averageCharWidth());

        label->setText(et);
        label->adjustSize();
    }

}
