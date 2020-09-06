/****************************************************************************
 * @file    MssDefines.h
 * @date    2020-08-11
 * @author  MonoKelvin
 * @email   15007083506@qq.com
 * @github  https://github.com/MonoKelvin
 * @brief
 *
 * This source file is part of MonoStyleSheet.
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
#ifndef MSSDEFINES_H
#define MSSDEFINES_H

#include "MonoStyleSheet.h"

#include <QColor>

MSS_NAMESPACE_BEGIN

/**
 * @brief 像素
 */
#define MPx "px"

/**
 * @brief 一个点的大小（即1/72英寸）
 */
#define MPt "pt"

/**
 * @brief 字体宽度（即'M'的宽度）
 */
#define MEm "em"

/**
 * @brief 字体宽度（即'x'的高度）
 */
#define MEx "ex"

/**
 * @brief 变量表
 */
typedef QMap<QString, QString> TVarTable;

/**
 * @brief 属性表
 * @keyword 属性名
 * @value 属性值，所有写入到文件中是都以字符串的形式
 */
typedef QMap<QString, QVariant> TAttrTable;

// 0-255
typedef unsigned char tinyint;

/**
 * @brief 选择器类型
 */
enum ESelector
{
    /**
     * @brief 通配符。匹配所有，一般为`*`
     * @example
     * * {
     *   // ...
     * }
     */
    MS_Universal,

    /**
     * @brief 类型选择器。匹配所有给定的类型和其子类的对象
     * @example
     * QPushButton {
     *   // ...
     * }
     */
    MS_Type,

    /**
     * @brief 属性选择器。匹配所有符合给定属性的值的类型对象
     * @example
     * QPushButton[flat="true"] {
     *   // ...
     * }
     */
    MS_Property,

    /**
     * @brief 类选择器。匹配所有给定类型的对象，但是不匹配其子类的对象
     * @example
     * .QPushButton {
     *   // ...
     * }
     */
    MS_Class,

    /**
     * @brief ID选择器。匹配所有给定ID的对象
     * @example
     * #btn1 {
     *   // ...
     * }
     */
    MS_Id,

    /**
     * @brief 子选择器。所有给定父对象下所有指定的对象，必须是直接包含关系
     * @example
     * QWidget > QPushButton {
     *   // ...
     * }
     */
    MS_Child,
};

/**
 * @brief 对齐
 */
enum EAlignment
{
    MA_Top,
    MA_Bottom,
    MA_Left,
    MA_Right,
    MA_Center,
};

/**
 * @brief 重复样式
 */
enum ERepeat
{
    MR_RepeatX,
    MR_RepeatY,
    MR_Repeat,
    MR_NoRepeat,
};

/**
 * @brief These values correspond the Color roles in the widget's QPalette.
 * @link https://doc.qt.io/qt-5/stylesheet-reference.html#paletterole
 */
enum EPaletteRole
{
    MPR_AlternateBase,
    MPR_Base,
    MPR_BrightText,
    MPR_Button,
    MPR_ButtonText,
    MPR_Dark,
    MPR_Highlight,
    MPR_HighlightedText,
    MPR_Light,
    MPR_Link,
    MPR_LinkVisited,
    MPR_Mid,
    MPR_Midlight,
    MPR_Shadow,
    MPR_Text,
    MPR_Window,
    MPR_WindowText
};

/**
 * @brief 图标状态
 */
enum EIconState
{
    MIS_Disabled,
    MIS_Active,
    MIS_Normal,
    MIS_Selected
};

/**
 * @brief 渐变
 */
enum EGradient
{
    MG_LinearGradient,
    MG_RadialGradient,
    MG_ConicalGradient
};

/**
 * @brief 字体样式
 */
enum EFontStyle
{
    MFS_Normal,
    MFS_Italic,
    MFS_Oblique
};

/**
 * @brief 字体权重
 */
enum EFontWeiget
{
    MFW_Normal,
    MFW_Bold,
    MFW_100 = 100,
    MFW_200 = 200,
    MFW_300 = 300,
    MFW_400 = 400,
    MFW_500 = 500,
    MFW_600 = 600,
    MFW_700 = 700,
    MFW_800 = 800,
    MFW_900 = 900,
};

/**
 * @brief 边框样式
 */
enum EBorderStyle
{
    MBS_Dashed,
    MBS_DotDash,
    MBS_DotDotDash,
    MBS_Dotted,
    MBS_Double,
    MBS_Groove,
    MBS_Inset,
    MBS_Outset,
    MBS_Ridge,
    MBS_Solid,
    MBS_None
};

/**
 * @brief 边框图片类型
 */
enum EBorderImageType
{
    MBI_Stretch,
    MBI_Repeat
};

/**
 * @brief 附着
 */
enum EAttachment
{
    MAt_Scroll,
    MAt_Fixed
};

/**
 * @brief 背景样式
 * @link https://doc.qt.io/qt-5/stylesheet-reference.html#background
 */
struct SBackground
{
    /* data */
};

/**
 * @brief 变量声明符号
 */
struct SVarDeclarator
{
    /**
     * @brief 前缀
     * @example
     *
     * @myVar: "hello";     // 其中`@`和`$`符号都是变量前缀
     * $count = 20;
     */
    char prefix;

    /**
     * @brief 赋值符号
     * @example
     *
     * @myVar: "hello";     // 其中`:`和`=`都是赋值符号
     * $count = 20;
     */
    char assignment;

    /**
     * @brief 语句终止符号
     * @example
     *
     * @myVar: "hello";     // 其中`;是赋值符号
     */
    char terminator;
};

/**
 * @brief 变量定义
 */
struct SVarDefintion
{
    /**
     * @brief 变量声明符号
     */
    SVarDeclarator declarator;

    /**
     * @brief 变量正则表达式
     * @example [\\w_][\\w\\d_]*
     */
    QString varPattern;

    /**
     * @brief 变量对应的值正则表达式
     * @example .+
     */
    QString valuePattern;
};

/**
 * @brief mss(Mono Style Sheet) 样式表文件配置选项。mss文件的变量命名规则是按照less的，编译生成后的
 * 文件为qss，可以作为Qt的样式表文件使用
 */
struct SMssConfigOption
{
    /**
     * @brief 是否清除注释
     * @note 默认清除所有`//`和"/ ** /标识的注释内容
     * @default true
     */
    bool isClearComments = true;

    /**
     * @brief 是否支持表达式
     * @note MMS中表达式计算在声明变量时才可使用
     * @default true
     * @todo 在变量使用时进行表达式计算
     */
    bool isSupportExpression = true;

    /**
     * @brief 是否解析出错时生成错误日志。
     * @note 默认错误日志在文件同一目录下，命名规则为mss-${yyy-MM-dd}.log
     */
    bool isErrorLog = false;

    /**
     * @brief 生成时使用的线程数量
     * @param 1-8
     * @default 2
     */
    tinyint threadCount = 4;

    /**
     * @brief 缩进时使用的空格数量
     * @param 0-8 空格个数
     * @param >8  所有缩进替换为制表符`\t`
     * @default 4
     */
    tinyint indentationNumber = 4;

    /**
     * @brief 生成时格式化文档选项
     * @param -n [none] 无，不对源格式进行调整
     * @param -l [loose] 宽松模式，保持基本的换行、空格和缩进
     * @param -c [Compact] 紧凑格式，尽可能删除空行、空格和缩进
     * @param -t [tiny] 最小模式，删除所有换行、空格和注释失效
     * @note -n和-t会使得`IsClearComments`失效，`IntendNumber`只在-l模式下起作用
     * @default -l
     */
    char formatOption = 'l';

    /**
     * @brief 目标文件名
     */
    QString targetFile;

    /**
     * @brief 源文件名
     */
    QString sourceFile;

    /**
     * @brief 默认使用的变量声明规则样式
     * @note 该命名规则符合less变量格式
     */
    const SVarDefintion varDefiniton{{'@', ':', ';'}, "[\\w_][\\w\\d_]*", ".+"};
};

/**
 * @brief qt颜色转css标准rgb字符串
 * @param color 颜色
 * @param isCompact 是否紧凑以格式返回，即去掉所用空格
 * @return 返回css标准rgb字符串
 * @example
 *  ColorToRgb(red, true);  // rgb(255,0,0)
 *  ColorToRgb("#ff0000");  // rgb(255, 0, 0)
 */
inline const QString ColorToRgb(const QColor &color, bool isCompact = false)
{
    if (isCompact)
    {
        return QStringLiteral("rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue());
    }
    else
    {
        return QStringLiteral("rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
    }
}

MSS_NAMESPACE_END

#endif // MSSDEFINES_H
