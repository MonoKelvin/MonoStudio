/****************************************************************************
 * @file    MStyleTitleBar.cpp
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
#include "MStyleTitleBar.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "common/MonoUI.h"

namespace mui
{
    MStyleTitleBar::MStyleTitleBar(QWidget* parent, const FTitleButtonsHints& hints)
        : QWidget(parent)
    {
        Q_ASSERT(nullptr != parent);

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setObjectName(QStringLiteral("mono_title_bar"));
        setStyleSheet("background:white");
        setFixedHeight(40);

        mMainLayout = new QBoxLayout(QBoxLayout::RightToLeft, this);
        mMainLayout->setSpacing(0);
        mMainLayout->setMargin(0);

        mTitle = new QLabel(this);
        mTitle->setObjectName(QStringLiteral(MUI_TITLEBAR_LB_TITLE));
        mTitle->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

        _applyButtonsHint(hints);
    }

    MStyleTitleBar::~MStyleTitleBar()
    {
    }

    void MStyleTitleBar::setTitle(const QString& title, ETitleTextAlignment alignment)
    {
        mTitle->setText(title);
        mTextAlignment = alignment;

        resize(width() + 1, height());
        resize(width() - 1, height());
    }

    void MStyleTitleBar::setButtonEnable(const FTitleButtonsHints& hints, bool enabled)
    {
        if (hints & CloseButton) {
            findChild<QPushButton*>(QStringLiteral(MUI_TITLEBAR_BTN_CLOSE))->setEnabled(enabled);
        }

        if (hints & MaximizeButton) {
            findChild<QPushButton*>(QStringLiteral(MUI_TITLEBAR_BTN_MAX))->setEnabled(enabled);
        }

        if (hints & MinimizeButton) {
            findChild<QPushButton*>(QStringLiteral(MUI_TITLEBAR_BTN_MIN))->setEnabled(enabled);
        }

        if (hints & HelpButton) {
            findChild<QPushButton*>(QStringLiteral(MUI_TITLEBAR_BTN_HELP))->setEnabled(enabled);
        }
    }

    void MStyleTitleBar::mximizedExclusion(void)
    {
        if (parentWidget()->isMaximized()) {
            parentWidget()->setWindowState(Qt::WindowNoState);
        } else {
            parentWidget()->setWindowState(Qt::WindowMaximized);
        }
    }

    void MStyleTitleBar::paintEvent(QPaintEvent* event)
    {
        M_USE_STYLE_SHEET()

        return QWidget::paintEvent(event);
    }

    void MStyleTitleBar::resizeEvent(QResizeEvent* event)
    {
        QMargins margins = mMainLayout->contentsMargins();
        const int othersWidth = _getChildrenWidthExceptTitle();

        if (mTextAlignment == AlignmentCenter) {
            mTitle->setAlignment(Qt::AlignCenter);
            margins.setLeft(othersWidth);
        } else {
            mTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            margins.setLeft(0);
        }

        mMainLayout->setContentsMargins(margins);
        ElidedTextForLabel(mTitle);

        return QWidget::resizeEvent(event);
    }

    void MStyleTitleBar::_applyButtonsHint(const FTitleButtonsHints& hints)
    {
        if (hints & CloseButton) {
            QPushButton* btn = _createTitleButtons(QStringLiteral(MUI_TITLEBAR_BTN_CLOSE));
            connect(btn, &QPushButton::clicked, parentWidget(), &QWidget::destroyed);
        }

        if (hints & MaximizeButton) {
            QPushButton* btn = _createTitleButtons(QStringLiteral(MUI_TITLEBAR_BTN_MAX));
            connect(btn, &QPushButton::clicked, this, &MStyleTitleBar::mximizedExclusion);
        }

        if (hints & MinimizeButton) {
            QPushButton* btn = _createTitleButtons(QStringLiteral(MUI_TITLEBAR_BTN_MIN));
            connect(btn, &QPushButton::clicked, parentWidget(), &QWidget::showMinimized);
        }

        if (hints & HelpButton) {
            QPushButton* btn = _createTitleButtons(QStringLiteral(MUI_TITLEBAR_BTN_HELP));
            connect(btn, &QPushButton::clicked, [&] {emit help();});
        }
    }

    int MStyleTitleBar::_getChildrenWidthExceptTitle()
    {
        int totalWidth = 0;

        for (const auto& child : children()) {
            const auto& childWidget = qobject_cast<QWidget*>(child);
            if (nullptr != childWidget) {
                totalWidth += childWidget->width();
            }
        }

        return totalWidth - mTitle->width();
    }

    QPushButton* MStyleTitleBar::_createTitleButtons(const QString& objName)
    {
        QPushButton* btn = new QPushButton(this);

        btn->setObjectName(objName);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        btn->setFixedSize(height() * 1.5, height());
        mMainLayout->addWidget(btn);

        return btn;
    }
}

