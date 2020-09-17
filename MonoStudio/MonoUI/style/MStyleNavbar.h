
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
class QAbstractButton;
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
         * @brief 最小保持偏移。用于确保追踪条始终处于可视位置
         * @note 几个特殊的值
         *  - >=0: 表示最小保持偏移，即向哪一边移动就和导航栏的那边始终保持该值的间距，如：
         *  向左移时，追踪条就会确保和导航栏左边始终保持该值的间距，此时移动的将不是追踪条而是
         *  导航项，直到导航项不可再移动时，这时移动的才是追踪条
         *  - =-1: 表示自动确定该偏移值，一般为当前选中项的上一个或下一个导航项的宽度或者高度
         *  - <-1: 表示MStyleNavbar将不使用边界逆向追踪
         * @note 逆向追踪时动画是否开启、动画持续时间以及动画类型都是通过该结构中的线管参数同时
         * 控制的
         */
        int minHoldingOffset;

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
            : offset(0)
            , minHoldingOffset(-1)
            , animationType(1)      // InQuad
            , duration(200)
            , isFitItem(true)
            , isAnimation(true)
            , isStretchingAnimation(false)
            , isEnabled(true)
        {}
    };

    /**
     * @brief Mono风格化导航栏
     *
     * 导航栏中每一个可导航的按钮称为导航项(item)，每个item都有一个索引表示它的位置，对于垂直导航栏
     * 其索引（如果存在至少一个item）顺序为从上到下，从0开始；水平导航栏的索引顺寻为从左到右，从0开始
     *
     * 导航栏中附带一个“追踪条”控件，可以通过 SNavTrackBarStyle 参数进行风格化设置，其作用时为了跟踪
     * 当前选中的item，一般常见的是在垂直导航栏中item位置的左侧，当选中某个item时，其位置就会竖直移动到
     * 该item位置；类似的，在水平导航栏中常见的是在item位置底部，当选中某个item时，其位置就会水平移动到
     * 该item位置。
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
         * @param index 要添加到导航栏的位置。0是开始位置，小于零则添加到最后
         */
        void addItem(const QString& text, const QIcon& icon = QIcon(), int index = -1);

        /**
         * @brief 设置导航项大小。垂直导航栏会自动忽略w值，水平导航栏会自动忽略h值
         * @param w 宽度。垂直导航栏会自动该值
         * @param h 高度。水平导航栏会自动该值
         * @param index 要设置的导航项的索引，如果小于0就全部同时设置
         */
        void setItemSize(int w, int h, int index = -1);

        /**
         * @brief 设置导航项是否激活
         * @param index 导航项索引。如果索引不存在则什么也不做
         * @param isEnabled 是否激活导航项
         * - true: 激活导航项，则该导航项可以选中并触发导航信号等
         * - false: 取消激活导航项，则该导航项不可被选中，不在接收点击事件等，即使该导航项已经被选中了
         */
        void setItemEnabled(int index, bool isEnabled);

        /**
         * @brief 设置导航项大小。垂直导航栏会自动忽略宽度，水平导航栏会自动忽略高度
         * @param size 导航项大小
         * @param index 要设置的导航项的索引，如果小于0就全部同时设置
         */
        inline void setItemSize(const QSize& size, int index = -1)
        {
            setItemSize(size.width(), size.height(), index);
        }

        /**
         * @brief 移除指定位置的导航项
         * @param index 导航项的索引，如果索引不存在则什么也不做
         */
        void removeItem(int index);

        /**
         * @brief 根据索引获取导航项按钮
         * @param index 导航项索引
         * @return 返回指定索引的导航项，如果索引不存在则什么也不做
         */
        QPushButton* getItem(int index);

        /**
         * @brief 获得当前选中的导航项按钮
         * @return 返回当前选中的导航项按钮，如果未选中则什么也不做
         */
        QPushButton* getCurrentItem(void) const;

        /**
         * @brief 获取当前导航栏的方向
         * @return 返回导航栏的方向
         *  - Vertical 垂直方向
         *  - Horizontal 水平方向
         */
        Qt::Orientation getOrientation(void) const;

        /**
         * @brief 获取导航栏中包含导航项的内容控件
         * @note 尽量不要使用该函数去修改内容控件，否则会造成难以预料的结果，请使用const版本
         * @return 内容控件
         */
        inline QWidget* getContent(void) noexcept
        {
            return mContent;
        }

        /**
         * @brief 获取导航栏中包含导航项的内容控件
         * @return 内容控件
         */
        inline const QWidget* getContent(void) const noexcept
        {
            return mContent;
        }

        /**
         * @brief 获取追踪条控件
         * @return 追踪条控件
         */
        inline QWidget* getTrackBar(void) const noexcept
        {
            return mTrackBar;
        }

        /**
         * @brief 获取追踪条控件样式
         * @return
         */
        inline SNavTrackBarStyle getTrackBarStyle() const noexcept
        {
            return mTrackBarStyle;
        }

        /**
         * @brief 设置追踪条样式
         * @param trackBarStyle 追踪条样式
         */
        void setTrackBarStyle(const SNavTrackBarStyle& trackBarStyle);

        /**
         * @brief 设置每个导航项的间距
         * @param spacing 间距
         */
        void setSpacing(int spacing);

        /**
         * @brief 获得导航项间距
         * @return 导航项间距
         */
        int getSpacing(void) const;

        /**
         * @brief 设置导航栏的四个方向的内边距
         * @param margins 导航栏内边距
         */
        void setMargins(int margins);

        /**
         * @brief 设置导航栏内边距
         * @param left 左边距
         * @param top 上边距
         * @param right 右边距
         * @param bottom 下边距
         */
        void setMargins(int left, int top, int right, int bottom);

        /**
         * @brief 获得导航栏内边距
         * @return 导航栏内边距
         */
        QMargins getMargins(void) const;

        /**
         * @brief 获得导航项到`滚动区域内容的最左/上`部分的距离，不是到导航栏最左/上侧
         * @param index 导航项索引，小于0表示当前选中的导航项，默认-1
         * @param isCenter 是否计算的是导航项中心到滚动区域内容的最左/上部分的距离
         * @return 距离，返回-1说明获取导航项失败
         * @see getItemOffset
         */
        int getItemDistance(int index = -1, bool isCenter = true);

        /**
         * @brief 获得导航项到`导航栏的最左/上`部分的偏移距离，不是到滚动区域内容的最左/上侧
         * @param index 导航项索引，小于0表示当前选中的导航项，默认-1
         * @param isCenter 是否计算的是导航项中心到导航栏的最左/上部分的偏移距离
         * @return 偏移距离
         * @note 该函数不会对空的导航项进行计算，所以当索引未找到导航项时，返回0
         * @see getItemDistance
         */
        int getItemOffset(int index = -1, bool isCenter = true);

    public Q_SLOTS:
        /**
         * @brief 槽：导航
         * @param index 要导航到的索引。调用该函数后相对应的导航项会被选中（如果激活）
         */
        void navigate(int index);

    Q_SIGNALS:
        /**
         * @brief 信号：当导航时触发
         * @param item 当前是哪一个导航项被触发（点击）
         */
        void navigated(QPushButton* item);

    protected:
        bool event(QEvent* e) override;
        bool eventFilter(QObject* o, QEvent* e) override;
        inline void resizeEvent(QResizeEvent*) override
        {
            _updateScrollBars();
            _updateTrackBarPosition(true);
        }

        QSize viewportSizeHint() const override;
        void scrollContentsBy(int, int) override;

    private:
        void _init(Qt::Orientation ori);
        void _updateItemId(void);
        void _updateScrollBars(void);
        void _updateContentPosition(void);
        void _updateTrackBarPosition(bool isStopAnimation = false);
        int _inverseTrack(QAbstractButton* btn);

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
