/****************************************************************************
 * @file    MElidedLabel.cpp
 * @date    2020-09-26
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
#include "MElidedLabel.h"

#include <QLabel>

namespace mui
{
    MElidedLabel::MElidedLabel(QWidget* parent, Qt::WindowFlags f)
        : QLabel(parent, f)
        , mMode(Qt::ElideRight)
        , mCharacter('.')
    {
    }

    MElidedLabel::MElidedLabel(const QString& text, QWidget* parent, Qt::WindowFlags f)
        : QLabel(text, parent, f)
        , mMode(Qt::ElideRight)
        , mCharacter('.')
    {
        mOriginalText = text;
    }

    MElidedLabel::~MElidedLabel()
    {
    }

    void MElidedLabel::resizeEvent(QResizeEvent* event)
    {
        _updateText();
        return QWidget::resizeEvent(event);
    }

    void MElidedLabel::_updateText()
    {
        const QFontMetrics fm = fontMetrics();
        QString et = fm.elidedText(mOriginalText, mMode, width() - 3 * fm.averageCharWidth());

        if (et != mOriginalText && mCharacter != '.') {
            et.replace(QStringLiteral("..."), QString(mCharacter).repeated(3));
        }

        setText(et);
    }
}
