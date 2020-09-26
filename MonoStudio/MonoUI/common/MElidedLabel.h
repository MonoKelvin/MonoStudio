/****************************************************************************
 * @file    MElidedLabel.h
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
#ifndef MELIDEDLABEL_H
#define MELIDEDLABEL_H

#include <QLabel>

#include "defines.h"

namespace mui
{
    /**
     * @brief MonoUI 自动省略文本的标签控件
     *
     * 当标签内的文本内容大于标签的宽度时，会自动省略文本，省略部分字符、省略位置等都可以自定义。
     *
     * @default 右侧省略，'.'作为省略符号，省略符号连续出现三次
     *
     * @note 如果要正常设置文本的原始（省略前的）内容，请务必使用 setOriginalText 方法，不要使用
     * QLabel::setText，因为 QLabel 的内容属于静态文本，当更改了内容（setText）时没有相关信
     * 号发出，则在 MElidedLabel 类中就无法捕获文本内容的改变，更无法拦截 setText 的过程。当然
     * 获取原始文本也要使用 getOriginalText，而 QLabel::text 获取的是省略后文本，但省略后也可能
     * 和原始文本一样，如果有需求的话可以使用 QLabel::text。
     */
    class MONOUI_API MElidedLabel : public QLabel
    {
        Q_OBJECT
    public:
        explicit MElidedLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
        explicit MElidedLabel(const QString& text, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
        ~MElidedLabel(void);

        inline void setOriginalText(const QString& text)
        {
            mOriginalText = text;
            _updateText();
        }

        inline QString getOriginalText(void) const noexcept
        {
            return mOriginalText;
        }

        inline void setElidedMode(Qt::TextElideMode mode)
        {
            mMode = mode;
            _updateText();
        }

        inline Qt::TextElideMode getElidedMode(void) const noexcept
        {
            return mMode;
        }

        inline void setElidedCharacter(const QChar character)
        {
            mCharacter = character;
            _updateText();
        }

        inline QChar getElidedCharacer(void) const noexcept
        {
            return mCharacter;
        }

    protected:
        void resizeEvent(QResizeEvent* event) override;

    private:
        void _updateText(void);

    private:
        // 原始的文本内容，可能是富文本
        QString mOriginalText;

        // 省略模式
        Qt::TextElideMode mMode;

        // 省略的字符内容
        QChar mCharacter;

    private:
        Q_DISABLE_COPY_MOVE(MElidedLabel);
    };
}

#endif // MELIDEDLABEL_H
