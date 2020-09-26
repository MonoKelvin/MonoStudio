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
#include <QPushButton>
#include <QMouseEvent>

#include "common/MonoUI.h"
#include "common/MElidedLabel.h"

namespace mui
{
    MStyleTitleBar::MStyleTitleBar(QWidget* parent, const FTitleButtonsHints& hints)
        : QWidget(parent)
        , mBtnHints(ETitleButtonsHint::StandardWindowButtons)
        , mTextAlignment(ETitleTextAlignment::AlignCenter)
    {
        Q_ASSERT(nullptr != parent);

        setFixedHeight(32);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setObjectName(QStringLiteral("mono_title_bar"));
        setStyleSheet("*{background:white;border:1px solid red;}");

        mMainLayout = new QBoxLayout(QBoxLayout::RightToLeft, this);
        mMainLayout->setSpacing(0);
        mMainLayout->setMargin(0);

        mTitle = new MElidedLabel(QStringLiteral("Mono Studio - v") + MONOUI_VERSION_STR, this);
        mTitle->setObjectName(QStringLiteral(MUI_TITLEBAR_LB_TITLE));

        _applyButtonsHint(hints);
        mMainLayout->addWidget(mTitle);
    }

    MStyleTitleBar::~MStyleTitleBar()
    {
    }

    void MStyleTitleBar::setTitle(const QString& title, ETitleTextAlignment alignment)
    {
        mTitle->setText(title);
        mTextAlignment = alignment;

        _updateTitlePosition();
    }

    void MStyleTitleBar::setButtonEnable(const FTitleButtonsHints& hints, bool enabled)
    {
        if (hints & CloseButton) {
            findChild<QPushButton*>(QStringLiteral(MUI_TITLEBAR_BTN_CLOSE), Qt::FindDirectChildrenOnly)->setEnabled(enabled);
        }

        if (hints & MaximizeButton) {
            findChild<QPushButton*>(QStringLiteral(MUI_TITLEBAR_BTN_MAX), Qt::FindDirectChildrenOnly)->setEnabled(enabled);
        }

        if (hints & MinimizeButton) {
            findChild<QPushButton*>(QStringLiteral(MUI_TITLEBAR_BTN_MIN), Qt::FindDirectChildrenOnly)->setEnabled(enabled);
        }

        if (hints & HelpButton) {
            findChild<QPushButton*>(QStringLiteral(MUI_TITLEBAR_BTN_HELP), Qt::FindDirectChildrenOnly)->setEnabled(enabled);
        }
    }

    void MStyleTitleBar::mximizedExclusion(void)
    {
        if (parentWidget()->isMaximized()) {
            parentWidget()->setWindowState(Qt::WindowNoState);
            emit maximized(false);
        } else {
            parentWidget()->setWindowState(Qt::WindowMaximized);
            emit maximized(true);
        }
    }

    void MStyleTitleBar::paintEvent(QPaintEvent* event)
    {
        M_USE_STYLE_SHEET()

        return QWidget::paintEvent(event);
    }

    void MStyleTitleBar::resizeEvent(QResizeEvent* event)
    {
        _updateTitlePosition();
        return QWidget::resizeEvent(event);
    }

    QSize MStyleTitleBar::sizeHint() const
    {
        return QSize(qMax(_getChildrenWidthExceptTitle(), 128), 32);
    }

    void MStyleTitleBar::_applyButtonsHint(const FTitleButtonsHints& hints)
    {
        if (hints & CloseButton) {
            QPushButton* btn = _createTitleButtons(QStringLiteral(MUI_TITLEBAR_BTN_CLOSE));
            connect(btn, &QPushButton::clicked, this, &MStyleTitleBar::closed);
        }

        if (hints & MaximizeButton && nullptr != parentWidget()) {
            QPushButton* btn = _createTitleButtons(QStringLiteral(MUI_TITLEBAR_BTN_MAX));
            connect(btn, &QPushButton::clicked, this, &MStyleTitleBar::mximizedExclusion);
        }

        if (hints & MinimizeButton && nullptr != parentWidget()) {
            QPushButton* btn = _createTitleButtons(QStringLiteral(MUI_TITLEBAR_BTN_MIN));
            connect(btn, &QPushButton::clicked, this, &MStyleTitleBar::minimized);
        }

        if (hints & HelpButton) {
            QPushButton* btn = _createTitleButtons(QStringLiteral(MUI_TITLEBAR_BTN_HELP));
            connect(btn, &QPushButton::clicked, this, &MStyleTitleBar::help);
        }
    }

    void MStyleTitleBar::_updateTitlePosition()
    {
        QMargins margins = mMainLayout->contentsMargins();

        if (mTextAlignment == ETitleTextAlignment::AlignCenter) {
            mTitle->setAlignment(Qt::AlignCenter);
            margins.setLeft(_getChildrenWidthExceptTitle());
        } else {
            mTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            margins.setLeft(0);
        }

        mMainLayout->setContentsMargins(margins);
    }

    int MStyleTitleBar::_getChildrenWidthExceptTitle() const
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

