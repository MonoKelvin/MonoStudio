/****************************************************************************
 * @file    MonoStyleSheet.h
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
#ifndef MONOSTYLESHEET_H
#define MONOSTYLESHEET_H

#include <QObject>

#if defined(MONOSTYLESHEET_LIBRARY)
    #define MONOSTYLESHEET_API Q_DECL_EXPORT
#else
    #define MONOSTYLESHEET_API Q_DECL_IMPORT
#endif

/** @brief MonoStyleSheet 命名空间起始标识 */
#define MSS_NAMESPACE_BEGIN namespace MSS {

/** @brief MonoStyleSheet 命名空间结束标识 */
#define MSS_NAMESPACE_END }

MSS_NAMESPACE_BEGIN

MSS_NAMESPACE_END

#endif // MONOSTYLESHEET_H
