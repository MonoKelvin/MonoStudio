
/****************************************************************************
 * @file    MStyleNavbar.h
 * @date    2020-09-13
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
#ifndef MSTYLENAVBAR_H
#define MSTYLENAVBAR_H

#include "defines.h"

#include <QAbstractScrollArea>

class QScrollArea;
class QButtonGroup;
class QPushButton;
class QBoxLayout;

#define MUI_NAVBAR_OBJNAME  "mui_navbar"
#define MUI_NAVITEM_OBJNAME "mui_navbtn"
#define MUI_NAV_TRACKBAR    "mui_nav_trackbar"

namespace mui
{
    struct SNavItemStyle
    {
        QSize size;
        Qt::CursorShape cursor;
        bool hasTooltip;
    };

    /**
     * @brief 导航栏的追踪条样式选项
     * @note 更多和显示相关的样式通过样式表来完成，如可以通过样式表设置背景色和圆角等，否则即使开启追踪条，
     * 导航栏默认也不会显示出来，因为没有背景色
     */
    struct SNavTrackBarStyle
    {
        /**
         * @brief 偏移位置
         * @note 如果开启（isEnabled = true）追踪条，则：
         * - 水平导航栏中其位置在导航按钮下方，即 TrackBar 底部和导航栏底部平齐，此时 offset == 0，
         *   如果 offset == -1，则TrackBar顶部和导航栏顶部平齐；否则随着offset增加其位置从底部开始上移，
         *   随着offset减少其位置从底部开始下移
         * - 垂直导航栏中其位置在导航按钮左方，即 TrackBar 左部和导航栏左部平齐，此时 offset == 0，
         *   如果 offset == -1，则TrackBar右部和导航栏右部平齐；否则随着offset增加其位置从左部开始右移，
         *   随着offset减少其位置从右部开始左移
         * @note 该偏移值不受导航栏的 spacing 和 margin 影响
         */
        int offset;

        /**
         * @brief 动画类型，isAnimation为true时该值有效
         * @see QEasingCurve::Type
         */
        int animationType;

        /**
         * @brief 动画时长，单位为毫秒(ms)，isAnimation为true时该值有效
         */
        int duration;

        /** @brief 是否长度/宽度适应导航按钮的长度/宽度 */
        bool isFitItem;

        /** @brief 是否使用动画 */
        bool isAnimation;

        /** @brief 是否使用拉伸动画，isAnimation为true时该值有效 */
        bool isStretchingAnimation;

        /** @brief 是否使用 TrackBar */
        bool isEnabled;

        SNavTrackBarStyle(void)
            : offset(15)
            , animationType(1)      // InQuad
            , duration(200)
            , isFitItem(false)
            , isAnimation(true)
            , isStretchingAnimation(false)
            , isEnabled(true)
        {}
    };

    /**
     * @brief Mono风格化导航栏
     */
    class MONOUI_API MStyleNavbar : public QAbstractScrollArea
    {
        Q_OBJECT
    public:
        explicit MStyleNavbar(QWidget* parent = nullptr);
        explicit MStyleNavbar(Qt::Orientation ori, QWidget* parent = nullptr);
        ~MStyleNavbar();

        /**
         * @brief 添加一个导航项
         * @param icon 导航项显示的图标
         * @param text 导航项显示的文字
         */
        void addItem(const QString& text, const QIcon& icon = QIcon(), int index = -1);

        void setItemSize(int w, int h, int index = -1);

        inline void setItemSize(const QSize& size, int index = -1)
        {
            setItemSize(size.width(), size.height(), index);
        }

        void removeItem(int pos);

        QPushButton* getItem(uint index);

        QPushButton* getCurrentItem(void) const;

        Qt::Orientation getOrientation(void) const;

        inline QWidget* getContent(void) const noexcept
        {
            return mContent;
        }

        inline QWidget* getTrackBar(void) const noexcept
        {
            return mTrackBar;
        }

        inline SNavTrackBarStyle getTrackBarStyle() const noexcept
        {
            return mTrackBarStyle;
        }
        void setTrackBarStyle(const SNavTrackBarStyle& trackBarStyle);

        void setSpacing(int spacing);
        int getSpacing(void) const;

        void setMargins(int margins);
        void setMargins(int left, int top, int right, int bottom);
        QMargins getMargins(void) const;

        /**
         * @brief 获得当前导航项到内容的最左部分的距离
         * @param isCenter 是否计算的是当前导航项中心到滚动区域内容的最左部分的距离
         * @return 距离，返回-1说明当前未选中任何导航项
         */
        int getCurrentItemDistance(bool isCenter = true);

    public Q_SLOTS:
        void navigate(int index);

    Q_SIGNALS:
        void navigated(QPushButton* item);

    protected:
        //bool eventFilter(QObject*, QEvent*) override;
        inline void resizeEvent(QResizeEvent*) override
        {
            _updateScrollBars();
        }

        QSize viewportSizeHint() const override;

    private:
        void _updateItemId(void);
        void _init(Qt::Orientation ori);
        void _updateScrollBars(void);
        void _updateWidgetPosition(void);
        void _adjustTrackBarPosition(bool isStopAnimation = false);
        void _resizeContent(int newWidth, int newHeight);

    private:
        /** @brief 导航追踪条样式 */
        SNavTrackBarStyle mTrackBarStyle;

        /** @brief 导航按钮组 */
        QButtonGroup* mNavGroup;

        /** @brief 内容容器 */
        QWidget* mContent;

        /** @brief 内容布局 */
        QBoxLayout* mLayout;

        /** @brief 追踪条控件 */
        QWidget* mTrackBar;

    private:
        Q_DISABLE_MOVE(MStyleNavbar)
        Q_DISABLE_COPY(MStyleNavbar)
    };
}

#endif // MSTYLENAVBAR_H
