/****************************************************************************
 * @file    MonoToolkit.h
 * @date    2020-07-25
 * @author  MonoKelvin
 * @email   15007083506@qq.com
 * @github  https://github.com/MonoKelvin
 * @brief
 *
 * This source file is part of MonoStudioProject.
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
#ifndef MONOTOOLKIT_H
#define MONOTOOLKIT_H

#include <QCryptographicHash>
#include <QDebug>

#if defined(MONOTOOLKIT_LIBRARY)
    #define MONOTOOLKIT_API Q_DECL_EXPORT
#else
    #define MONOTOOLKIT_API Q_DECL_IMPORT
#endif

/** @brief MonoToolkit 命名空间起始标识 */
#define MTK_NAMESPACE_BEGIN namespace MTK {

/** @brief MonoToolkit 命名空间结束标识 */
#define MTK_NAMESPACE_END }

MTK_NAMESPACE_BEGIN

/** @brief 安全删除指针 */
#define MONO_DELETE_PTR(_Pointer_)   \
do {                                 \
    if (_Pointer_) {                 \
        delete _Pointer_;            \
        _Pointer_ = nullptr;         \
    }                                \
} while (0)

/**
 * @brief 常量字符串转换为编译器构造的QString，用以降低运行时构造QString的代价
 * @param _ConstString_ 常量字符串，如: "String"
 * @note 等价于使用 QStringLiteral(_ConstString_)
 * @see QStringLiteral
 */
#define MCStr(_ConstString_) QStringLiteral(_ConstString_)

/**
 * @brief 常量字符串转换为单字节字符串QLatin1String，用在所有可以支持QLatin1String参数的地方
 * @param _ConstString_ 常量字符串，如: "String"
 * @note 等价于使用 QLatin1String(_ConstString_)
 * @see QLatin1String
 */
#define MCStrA(_ConstString_) QLatin1String(_ConstString_)

/**
 * @brief 常量字符串转换为字节数组QByteArray，用以降低运行时构造QByteArray的代价
 * @param _ConstString_ 常量字符串，如: "Byte Array"
 * @note 等价于使用 QByteArrayLiteral(_ConstString_)
 * @see QByteArrayLiteral
 */
#define MCByteArr(_ConstString_) QByteArrayLiteral(_ConstString_)

/** @brief 错误类枚举开始映射 */
#define M_ERROR_ENUM_BEGIN(_Enum_, _EnumVariable_)          \
inline void setError(_Enum_ err) {                          \
    _EnumVariable_ = err;                                   \
}                                                           \
inline _Enum_ getError(void) const noexcept {               \
    return _EnumVariable_;                                  \
}                                                           \
inline QString getLastError(void) const {                   \
    switch (_EnumVariable_) {

/** @brief 错误类枚举对应返回字符消息 */
#define M_ERROR_ENUM_STRING(_EnumValue_, _ReturnString_)    \
case _EnumValue_:                                           \
return QStringLiteral(_ReturnString_);

/** @brief 错误类枚举结束映射 */
#define M_ERROR_ENUM_END() default: return QStringLiteral(""); }}

/**
 * @brief 创建MD5
 * @param source 源字符串
 * @param isHalf 是否取中间16位
 * @return 返回MD5字符串
 * @example
 *  - 32位小写: 38d7355701b6f3760ee49852904319c1   // isHalf == false (default)
 *  - 16位小写: 01b6f3760ee49852                   // isHalf == true
 */
inline QString CreateMD5(const QString& source, bool isHalf = false)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(source.toLatin1());
    const QString result = hash.result().toHex();
    return isHalf ? result.mid(8, 16) : result;
}

/**
 * @brief 获得人类可读的字节数字符串
 * @param bytes 字节数，单位为B
 * @return 返回可读的字节数字符串，如200B、346K、34.1M、2.15G
 */
inline const QString GetReadableBytes(qint64 bytes) noexcept
{
    if (bytes < 1024) {
        return QString::number(bytes) + "B";
    }

    double b = bytes / 1024.0;
    if (b < 1024.0) {
        return QStringLiteral("%1K").arg(b, 0, 'f', 0);
    }

    b /= 1024.0;
    if (b < 1024.0) {
        return QStringLiteral("%1M").arg(b, 0, 'f', 1);
    }

    return QStringLiteral("%1G").arg(b / 1024.0, 0, 'f', 2);
}

/**
 * @brief 获取鼠标移动速度
 * @param [out] iMouseSpeed 鼠标移动速度，该值在1-20之间，默认为10
 * @return 是否出错
 */
bool MONOTOOLKIT_API GetMouseSpeed(int& iMouseSpeed);

/**
 * @brief 获取鼠标移动速度
 * @return int 鼠标移动速度，该值在1-20之间，默认为10
 */
int MONOTOOLKIT_API GetMouseSpeed(void);

MTK_NAMESPACE_END

#endif // MONOTOOLKIT_H
