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
#include "MStyleTitleBar.h"

#include <windowsx.h>
#include <windows.h>
#include <QBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QScreen>

namespace mui
{
    const int M_RESIZE_BORDER = 6;

    MStyleWindow::MStyleWindow(QWidget* parent, Qt::WindowType type)
        : QWidget(parent, type)
        , mResizeType(RT_NoResize)
        , mIsResizable(true)
        , mIsMovable(true)
        , mIsResizeReady(false)
    {
        resize(640, 480);
        setMaximumSize(QGuiApplication::primaryScreen()->size());
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
        setAttribute(Qt::WA_TranslucentBackground, true);
        setMouseTracking(true);
        setObjectName(QStringLiteral(MUI_WINDOW));

        /**
         * 以下代码是为了将窗口去除边框并且能够让窗口自动排列
         * 参考：https://www.cnblogs.com/dongc/p/5598053.html
         */
        const HWND hwnd = (HWND)winId();
        const DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
        ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);

        // 阴影背景
        mShadowBg = new QWidget(this);
        mShadowBg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mShadowBg->setMouseTracking(true);
        mShadowBg->setObjectName(QStringLiteral(MUI_WINDOW_BG));

        // DEBUG
        mShadowBg->setStyleSheet(QString("#") + MUI_WINDOW_BG + "{background:#222223;}");

        // 主布局
        mMainLayout = new QBoxLayout(QBoxLayout::TopToBottom, mShadowBg);
        mMainLayout->setSpacing(0);
        mMainLayout->setMargin(0);

        // 标题栏
        mTitleBar = new MStyleTitleBar(mShadowBg);
        mMainLayout->addWidget(mTitleBar);

        // 内容控件
        mContent = new QWidget(mShadowBg);
        mContent->setMouseTracking(true);
        mMainLayout->addWidget(mContent, 1);

        // 调整显示层次
        mShadowBg->lower();
        mContent->raise();
        mTitleBar->raise();

        setShadow({});
    }

    MStyleWindow::~MStyleWindow()
    {
    }

    void MStyleWindow::setMovable(bool movable)
    {
        if (movable && nullptr == mTitleBar) {
            return;
        }

        mIsMovable = movable;
    }

    void MStyleWindow::setShadow(const SShadowParam& shadowParam)
    {
        if (nullptr == mShadowBg->graphicsEffect()) {
            QGraphicsDropShadowEffect* pEffect = new QGraphicsDropShadowEffect(mShadowBg);

            pEffect->setBlurRadius(shadowParam.radius);
            pEffect->setXOffset(shadowParam.xOffset);
            pEffect->setYOffset(shadowParam.yOffset);
            pEffect->setColor(shadowParam.color);

            mShadowBg->setGraphicsEffect(pEffect);
        }

        mShadowBg->graphicsEffect()->setEnabled(shadowParam.isEnabled);

        mShadowParam = shadowParam;
    }

    void MStyleWindow::setTitleBar(MStyleTitleBar* titleBar)
    {
        if (nullptr != mTitleBar) {
            mTitleBar->deleteLater();
            mTitleBar = nullptr;
        }

        if (nullptr == titleBar) {
            mShadowBg->layout()->removeWidget(mTitleBar);
            mIsMovable = false;
        } else {
            mTitleBar = titleBar;
            mIsMovable = true;
        }

        mContent->updateGeometry();
    }

    void MStyleWindow::setBorderRadius(int radius)
    {
        if (radius <= 0) {
            return;
        }

        QString style = mShadowBg->styleSheet();
        style.insert(style.lastIndexOf('}'), QString(
                         ";border-radius:%1px;"
                         "MStyleTitleBar{border-top-left-radius:%1px;"
                         "border-top-right-radius:%1px;}")
                     .arg(radius));
        mBorderRadius = {radius};
        mShadowBg->setStyleSheet(style);
    }

    void MStyleWindow::setBorderRadius(const SBorder<int>& borderRadius)
    {
        if (!borderRadius.isValid()) {
            return;
        }

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

        if (mResizeType != RT_NoResize) {
            mIsResizeReady = true;
        }

        return QWidget::mousePressEvent(event);
    }

    void MStyleWindow::mouseReleaseEvent(QMouseEvent* event)
    {
        mIsResizeReady = false;

        return QWidget::mouseReleaseEvent(event);
    }

    void MStyleWindow::mouseMoveEvent(QMouseEvent* event)
    {
        if (mIsResizable == RT_NoResize) {
            setCursor(Qt::ArrowCursor);
        } else {
            if (mIsResizeReady) {
                const int& dx = event->globalPos().x() - mMousePressed.x();
                const int& dy = event->globalPos().y() - mMousePressed.y();

                if (mResizeType & RT_Left) {
                    const int& nWidth = mOldGeometry.width() - dx;
                    if (nWidth >= minimumWidth() && nWidth <= maximumWidth()) {
                        setGeometry(mOldGeometry.x() + dx, y(), nWidth, height());
                    }
                } else if (mResizeType & RT_Right) {
                    const int& nWidth = mOldGeometry.width() + dx;
                    if (nWidth >= minimumWidth() && nWidth <= maximumWidth()) {
                        resize(mOldGeometry.width() + dx, height());
                    }
                }

                if (mResizeType & RT_Top) {
                    const int& nHeight = mOldGeometry.height() - dy;
                    if (nHeight >= minimumHeight() && nHeight <= maximumHeight()) {
                        setGeometry(x(), mOldGeometry.y() + dy, width(), nHeight);
                    }
                } else if (mResizeType & RT_Bottom) {
                    const int& nHeight = mOldGeometry.height() + dy;
                    if (nHeight >= minimumHeight() && nHeight <= maximumHeight()) {
                        resize(width(), mOldGeometry.height() + dy);
                    }
                }
            } else {
                _updateResizeType(event->x(), event->y());
            }
        }

        return QWidget::mouseMoveEvent(event);
    }

    void MStyleWindow::resizeEvent(QResizeEvent* event)
    {
        if (mShadowParam.isEnabled) {
            const qreal r = mShadowParam.radius;
            mShadowBg->setGeometry(QRect(r - mShadowParam.xOffset, r - mShadowParam.yOffset,
                                         width() - 2 * r, height() - 2 * r));
        } else if (isMaximized() || isFullScreen()) {
            mShadowBg->setGeometry(rect());
        } else {
            mShadowBg->setGeometry(M_RESIZE_BORDER, M_RESIZE_BORDER,
                                   width() - 2 * M_RESIZE_BORDER, height() - 2 * M_RESIZE_BORDER);
        }

        return QWidget::resizeEvent(event);
    }

    void MStyleWindow::changeEvent(QEvent* event)
    {
        switch (windowState()) {
        case Qt::WindowMaximized:
        case Qt::WindowMinimized:
            mShadowBg->graphicsEffect()->setEnabled(false);
            mShadowBg->setGeometry(rect());
            break;
        case Qt::WindowNoState:
            if (mShadowParam.isEnabled && mShadowBg && mShadowBg->graphicsEffect()) {
                mShadowBg->graphicsEffect()->setEnabled(true);
            }
            break;
        default:
            break;
        }

        // 防止最大化后鼠标不移动、鼠标指针不刷新。加上下面两句可以避免不刷新导致原位置拉伸的错误状态
        mResizeType = RT_NoResize;
        setCursor(Qt::ArrowCursor);

        return QWidget::changeEvent(event);
    }

    bool MStyleWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
    {
        /**
         * 以下代码是为了将窗口去除边框并且能够让窗口自动排列
         * 参考：https://www.cnblogs.com/dongc/p/5598053.html
         */

        MSG* msg = (MSG*)message;
        switch (msg->message) {
        case WM_NCHITTEST: {
            if (mIsMovable) {
                const int xPos = GET_X_LPARAM(msg->lParam) - geometry().x() - mShadowBg->x();
                const int yPos = GET_Y_LPARAM(msg->lParam) - geometry().y() - mShadowBg->y();

                if (mTitleBar->rect().contains(xPos, yPos) && !mTitleBar->childAt(xPos, yPos)) {
                    *result = HTCAPTION;
                    return true;
                }
            }
        }
        break;
        case WM_NCCALCSIZE:
            return true;
        case WM_GETMINMAXINFO: {
            if (::IsZoomed(msg->hwnd)) {
                // 最大化时会超出屏幕，所以填充边框间距
                RECT frame = { 0, 0, 0, 0 };
                ::AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);
                frame.left = qAbs(frame.left);
                frame.top = qAbs(frame.bottom);
                mShadowBg->setContentsMargins(frame.left, frame.top, frame.right, frame.bottom);
            } else {
                mShadowBg->setContentsMargins(0, 0, 0, 0);
            }

            *result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
            return true;
        }
        break;
        default:
            break;
        }

        return QWidget::nativeEvent(eventType, message, result);
    }

    void MStyleWindow::_updateResizeType(int ex, int ey)
    {
        if (!mIsResizeReady && !isMaximized() && mIsResizable && !isFullScreen()) {
            const auto& g = mShadowBg->geometry();
            const int& bw = M_RESIZE_BORDER;

            //if (!g.adjusted(bw, bw, -bw, -bw).contains(event->pos())) {
            if (g.right() <= ex && ex <= g.right() + bw && g.bottom() <= ey && ey <= g.bottom() + bw) {
                //右下角
                mResizeType = RT_BottomRight;
            } else if (g.right() <= ex && ex <= g.right() + bw && g.top() >= ey && ey + bw >= g.top()) {
                //右上角
                mResizeType = RT_TopRight;
            } else if (g.left() <= ex + bw && ex <= g.left() && g.top() >= ey && ey + bw >= g.top()) {
                //左上角
                mResizeType = RT_TopLeft;
            } else if (g.left() <= ex + bw && ex <= g.left() && g.bottom() <= ey && ey <= g.bottom() + bw) {
                //左下角
                mResizeType = RT_BottomLeft;
            } else if (g.right() <= ex && ex <= g.right() + bw) {
                //右边
                mResizeType = RT_Right;
            } else if (g.top() >= ey && ey + bw >= g.top()) {
                //上边
                mResizeType = RT_Top;
            } else if (g.bottom() <= ey && ey <= g.bottom() + bw) {
                //下边
                mResizeType = RT_Bottom;
            } else if (g.left() <= ex + bw && ex <= g.left()) {
                //左边
                mResizeType = RT_Left;
            } else {
                mResizeType = RT_NoResize;
            }
            //} else {
            //    mResizeType = RT_NoResize;
            //}

            switch (mResizeType) {
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
        } else {
            mResizeType = RT_NoResize;
            setCursor(Qt::ArrowCursor);
        }
    }
} // namespace mui
