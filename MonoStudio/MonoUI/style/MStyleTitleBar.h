/****************************************************************************
 * @file    MStyleTitleBar.h
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
#ifndef MSTYLETITLEBAR_H
#define MSTYLETITLEBAR_H

#include "defines.h"

class QBoxLayout;
class QPushButton;

#define MUI_TITLEBAR_BTN_MIN    "mui_tb_btn_min"
#define MUI_TITLEBAR_BTN_MAX    "mui_tb_btn_max"
#define MUI_TITLEBAR_BTN_CLOSE  "mui_tb_btn_close"
#define MUI_TITLEBAR_BTN_HELP   "mui_tb_btn_help"
#define MUI_TITLEBAR_LB_TITLE   "mui_tb_lb_title"

namespace mui
{
    class MElidedLabel;

    /**
     * @brief MonoUI 样式化标题栏，可以自定义窗口标题及其位置、右上角按钮以及标题栏其他控件
     *
     * 标题栏从左到右（如果提供）的控件布局顺序依次为：
     * 图标 -> 标题 -> 用户控件 -> 帮助(?) -> 最小化(_) -> 最大化([]) -> 关闭窗口(x)
     * 这种次序一般不可变更，如果需要可以获取标题栏的布局管理器，通过布局管理器来获取所有子控件，
     * 或者直接获取标题栏子控件，但不建议这么做。对于自带的控件（右上角按钮和标题文本等）可以通过
     * objectName 找到，其对应的对象名为 MUI_TITLEBAR_XXX 宏标识。
     *
     * @note MonoUI 样式化标题栏最好指定父控件，且其管理的窗口就是其父控件，即最小化控制的是其父
     * 控件的最小化，最大化控制的是其父控件的最大化，因此父控件要求没有它的父控件，否则无论指定
     * ETitleButtonsHint 中那个值，都最多只有HelpButton和CloseButton起作用，即最多只有
     * 帮助和关闭窗口的控件。
     */
    class MONOUI_API MStyleTitleBar : public QWidget
    {
        Q_OBJECT
    public:
        /** @brief 标题栏按钮策略 */
        enum ETitleButtonsHint {
            /** @brief 不创建默认的按钮 */
            NoButtons = 0x000,

            /** @brief 使用最小化按钮 */
            MinimizeButton = 0x001,

            /** @brief 使用最大化（还原）按钮 */
            MaximizeButton = 0x002,

            /** @brief 使用关闭窗口按钮 */
            CloseButton = 0x004,

            /** @brief 使用帮助按钮，帮助按钮默认在所有按钮的最左侧 */
            HelpButton = 0x008,

            /** @brief 标准窗口按钮，包括最小化、最大化和关闭窗口按钮 */
            StandardWindowButtons = MinimizeButton | MaximizeButton | CloseButton,
        };
        Q_DECLARE_FLAGS(FTitleButtonsHints, ETitleButtonsHint)

        /**
         * @brief 标题文本的对齐方式
         */
        enum ETitleTextAlignment {
            AlignLeft,
            AlignCenter,
        };
        Q_ENUM(ETitleTextAlignment)

        explicit MStyleTitleBar(QWidget* parent = nullptr, const FTitleButtonsHints& hints = StandardWindowButtons);
        ~MStyleTitleBar();

        void setTitle(const QString& title, ETitleTextAlignment alignment = AlignLeft);

        void setButtonEnable(const FTitleButtonsHints& hints, bool enabled);

        void setIcon(const QIcon& icon);

        inline FTitleButtonsHints getButtonsHints(void) const noexcept
        {
            return mBtnHints;
        }

    Q_SIGNALS:
        void minimized(bool checked = false);
        void maximized(bool restored);
        void closed(bool checked = false);
        void help(bool checked = false);

    public Q_SLOTS:
        void mximizedExclusion(void);

    protected:
        void paintEvent(QPaintEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        QSize sizeHint(void) const override;

    private:
        void _applyButtonsHint(const FTitleButtonsHints& hints);
        void _updateTitlePosition(void);
        int _getChildrenWidthExceptTitle(void) const;
        QPushButton* _createTitleButtons(const QString& objName);

    private:
        // 主（水平）布局
        QBoxLayout* mMainLayout;

        // 标题栏策略
        FTitleButtonsHints mBtnHints;

        // 标题控件
        MElidedLabel* mTitle;

        // 标题文本对齐方式
        ETitleTextAlignment mTextAlignment;
    };
}

#endif // MSTYLETITLEBAR_H
