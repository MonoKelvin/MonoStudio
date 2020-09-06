/****************************************************************************
 * @file    MStyleWindow.cpp
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
#include "MStyleWindow.h"
#include "MTitleBar.h"

#include <QBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QScreen>

namespace mui
{

    MStyleWindow::MStyleWindow(QWidget* parent, Qt::WindowType type)
        : QWidget(parent, type)
        , mResizeType(RT_NoResize)
        , mBoundaryWidth(6)
        , mResizable(true)
        , mMovable(false)
        , mMoveReady(false)
        , mResizeReady(false)
    {
        resize(640, 480);
        setMinimumSize(200, 200);
        setMaximumSize(QGuiApplication::primaryScreen()->size());
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlag(Qt::FramelessWindowHint);
        setMouseTracking(true);
        setObjectName(QStringLiteral("mono_style_win"));

        // 主窗口布局
        QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
        mainLayout->setSpacing(0);

        // 阴影背景
        mShadowBg = new QWidget(this);
        mShadowBg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mShadowBg->setMouseTracking(true);
        mShadowBg->setObjectName(QStringLiteral("mono_style_win_bg"));
        mShadowBg->setStyleSheet("#mono_style_win_bg{background:#222223;}");
        mainLayout->addWidget(mShadowBg);

        // 背景布局
        QBoxLayout* bgLayout = new QBoxLayout(QBoxLayout::TopToBottom, mShadowBg);
        bgLayout->setMargin(0);
        bgLayout->setSpacing(0);

        // 标题栏
        mTitleBar = new MTitleBar(mShadowBg);
        mTitleBar->installEventFilter(this);
        mTitleBar->setMouseTracking(true);
        bgLayout->addWidget(mTitleBar);

        // 内容控件
        mContent = new QWidget(mShadowBg);
        mContent->setMouseTracking(true);
        bgLayout->addWidget(mContent);

        // 调整显示层次
        mShadowBg->lower();
        mTitleBar->raise();

        setShadowEnabled(true);
    }

    MStyleWindow::~MStyleWindow()
    {

    }

    bool MStyleWindow::eventFilter(QObject* watched, QEvent* event)
    {
        if (watched == mTitleBar)
        {
            const auto& msEvt = static_cast<QMouseEvent*>(event);
            if (!(msEvt->buttons() & Qt::LeftButton))
            {
                return false;
            }

            switch (event->type())
            {
            case QMouseEvent::MouseMove:
                if (mMoveReady && mResizeType == RT_NoResize)
                {
                    if (isMaximized())
                    {
                        if ((msEvt->globalPos() - mMousePressed).manhattanLength() > 5)
                        {
                            showNormal();
                            setShadowEnabled(true);

                            const auto& mg = layout()->contentsMargins();
                            const int& dx = msEvt->globalX() * (1.0 - double(geometry().width()) / mOldGeometry.width());

                            move(dx - mg.left(), msEvt->globalY() - msEvt->y() - mg.top());
                            mOldGeometry = geometry();
                        }
                    }
                    else
                    {
                        move(mOldGeometry.x() + msEvt->globalPos().x() - mMousePressed.x(),
                             mOldGeometry.y() + msEvt->globalPos().y() - mMousePressed.y());
                    }

                    return true;
                }
                break;
            case QMouseEvent::MouseButtonPress:
                if (!mResizeReady)
                {
                    mMoveReady = true;
                }
                break;
            case QMouseEvent::MouseButtonDblClick:
                if (mResizable && mResizeType == RT_NoResize)
                {
                    if (isMaximized())
                    {
                        showNormal();
                        setShadowEnabled(true);
                    }
                    else
                    {
                        showMaximized();
                        setShadowEnabled(false);
                    }

                    return true;
                }
                break;
            case QMouseEvent::MouseButtonRelease:
                // 不让标题栏超出屏幕上方
                if (mMoveReady && y() + layout()->contentsMargins().top() < 0)
                {
                    move(x(), 0);
                }
                break;
            default:
                break;
            }
        }

        return QWidget::eventFilter(watched, event);
    }

    void MStyleWindow::setShadowEnabled(bool enabled)
    {
        if (enabled)
        {
            setShadow(mShadowParam);
        }
        else
        {
            mShadowBg->graphicsEffect()->deleteLater();
            mShadowBg->setGraphicsEffect(nullptr);

            layout()->setMargin(0);
        }
    }

    void MStyleWindow::setShadow(const SShadowParam& shadowParam)
    {
        if (nullptr != mShadowBg->graphicsEffect())
        {
            mShadowBg->graphicsEffect()->deleteLater();
            mShadowBg->setGraphicsEffect(nullptr);
        }

        QGraphicsDropShadowEffect* pEffect = new QGraphicsDropShadowEffect;

        pEffect->setBlurRadius(shadowParam.Radius);
        pEffect->setXOffset(shadowParam.XOffset);
        pEffect->setYOffset(shadowParam.YOffset);
        pEffect->setColor(shadowParam.Color);

        layout()->setContentsMargins(
            qMax(0, int(shadowParam.Radius - shadowParam.XOffset)),
            qMax(0, int(shadowParam.Radius - shadowParam.YOffset)),
            qMax(0, int(shadowParam.Radius + shadowParam.XOffset)),
            qMax(0, int(shadowParam.Radius + shadowParam.XOffset))
        );

        mShadowBg->setGraphicsEffect(pEffect);
        mShadowParam = shadowParam;
    }

    void MStyleWindow::setTitleBar(MTitleBar* titleBar)
    {
        if (nullptr != mTitleBar)
        {
            mTitleBar->deleteLater();
            mTitleBar = nullptr;
        }

        if (nullptr == titleBar)
        {
            mShadowBg->layout()->removeWidget(mTitleBar);
            mMovable = false;
        }
        else
        {
            mTitleBar = titleBar;
            mMovable = true;
        }

        mContent->updateGeometry();
    }

    void MStyleWindow::setBorderRadius(int radius)
    {
        if (radius <= 0)
        {
            return;
        }

        QString style = mShadowBg->styleSheet();
        style.insert(style.lastIndexOf('}'), QString(
                         ";border-radius:%1px;"
                         "MTitleBar{border-top-left-radius:%1px;"
                         "border-top-right-radius:%1px;}").arg(radius));
        mBorderRadius = {radius};
        mShadowBg->setStyleSheet(style);
    }

    void MStyleWindow::setBorderRadius(const SBorder<int>& borderRadius)
    {
        if (!borderRadius.isValid())
        {
            return;
        }

        // TODO: Create StyleSheetDocument Class - MQssDocument
        QString style = mShadowBg->styleSheet();
        const QString br = QStringLiteral("border-top-left-radius:%1px;"
                                          "border-top-right-radius:%2px;"
                                          "border-bottom-left-radius:%3px;"
                                          "border-bottom-right-radius:%4px;")
                           .arg(borderRadius.topLeft)
                           .arg(borderRadius.topRight)
                           .arg(borderRadius.bottomLeft)
                           .arg(borderRadius.bottomRight);

        style.insert(style.lastIndexOf('}'), QStringLiteral(
                         ";%1MTitleBar{border-top-left-radius:%2px;"
                         "border-top-right-radius:%3px;}")
                     .arg(br)
                     .arg(borderRadius.topLeft)
                     .arg(borderRadius.topRight));

        mShadowBg->setStyleSheet(style);
    }

    void MStyleWindow::mousePressEvent(QMouseEvent* event)
    {
        mMousePressed = event->globalPos();
        mOldGeometry = geometry();

        if (mResizeType != RT_NoResize)
        {
            mResizeReady = true;
        }

        return QWidget::mousePressEvent(event);
    }

    void MStyleWindow::mouseReleaseEvent(QMouseEvent* event)
    {
        mResizeReady = false;
        mMoveReady = false;

        return QWidget::mouseReleaseEvent(event);
    }

    void MStyleWindow::mouseMoveEvent(QMouseEvent* event)
    {
        if (mResizable == RT_NoResize)
        {
            setCursor(Qt::ArrowCursor);
        }
        else
        {
            updateResizeType(event);

            if (mResizeReady)
            {
                const int& dx = event->globalPos().x() - mMousePressed.x();
                const int& dy = event->globalPos().y() - mMousePressed.y();

                if (mResizeType & RT_Left)
                {
                    const int& nWidth = mOldGeometry.width() - dx;
                    if (nWidth >= minimumWidth() && nWidth <= maximumWidth())
                    {
                        setGeometry(mOldGeometry.x() + dx, y(), nWidth, height());
                    }
                }
                else if (mResizeType & RT_Right)
                {
                    const int& nWidth = mOldGeometry.width() + dx;
                    if (nWidth >= minimumWidth() && nWidth <= maximumWidth())
                    {
                        resize(mOldGeometry.width() + dx, height());
                    }
                }

                if (mResizeType & RT_Top)
                {
                    const int& nHeight = mOldGeometry.height() - dy;
                    if (nHeight >= minimumHeight() && nHeight <= maximumHeight())
                    {
                        setGeometry(x(), mOldGeometry.y() + dy, width(), nHeight);
                    }
                }
                else if (mResizeType & RT_Bottom)
                {
                    const int& nHeight = mOldGeometry.height() + dy;
                    if (nHeight >= minimumHeight() && nHeight <= maximumHeight())
                    {
                        resize(width(), mOldGeometry.height() + dy);
                    }
                }
            }
        }

        return QWidget::mouseMoveEvent(event);
    }

    void MStyleWindow::updateResizeType(QMouseEvent* event)
    {
        if (!mResizeReady && !isMaximized() && mResizable && !isFullScreen())
        {
            const auto& g = mShadowBg->geometry();
            const int& bw = mBoundaryWidth;

            if (g.adjusted(-bw, -bw, bw, bw).contains(event->pos()))
            {
                const int& ex = event->x();
                const int& ey = event->y();

                if (g.right() <= ex && ex <= g.right() + bw && g.bottom() <= ey && ey <= g.bottom() + bw)    //右下角
                {
                    mResizeType = RT_BottomRight;
                }
                else if (g.right() <= ex && ex <= g.right() + bw && g.top() >= ey && ey + bw >= g.top())     //右上角
                {
                    mResizeType = RT_TopRight;
                }
                else if (g.left() <= ex + bw && ex <= g.left() && g.top() >= ey && ey + bw >= g.top())       //左上角
                {
                    mResizeType = RT_TopLeft;
                }
                else if (g.left() <= ex + bw && ex <= g.left() && g.bottom() <= ey && ey <= g.bottom() + bw)     //左下角
                {
                    mResizeType = RT_BottomLeft;
                }
                else if (g.right() <= ex && ex <= g.right() + bw)            //右边
                {
                    mResizeType = RT_Right;
                }
                else if (g.top() >= ey && ey + bw >= g.top())                //上边
                {
                    mResizeType = RT_Top;
                }
                else if (g.bottom() <= ey && ey <= g.bottom() + bw)          //下边
                {
                    mResizeType = RT_Bottom;
                }
                else if (g.left() <= ex + bw && ex <= g.left())              //左边
                {
                    mResizeType = RT_Left;
                }
                else
                {
                    mResizeType = RT_NoResize;
                }
            }
            else
            {
                mResizeType = RT_NoResize;
            }

            switch (mResizeType)
            {
            case RT_Left:
            case RT_Right:
                setCursor(Qt::SizeHorCursor);
                break;
            case RT_Top:
            case RT_Bottom:
                setCursor(Qt::SizeVerCursor);
                break;
            case RT_TopLeft:
            case RT_BottomRight:
                setCursor(Qt::SizeFDiagCursor);
                break;
            case RT_TopRight:
            case RT_BottomLeft:
                setCursor(Qt::SizeBDiagCursor);
                break;
            default:
                setCursor(Qt::ArrowCursor);
                break;
            }
        }
    }
}

