/****************************************************************************
 * @file    MFileToolkit.cpp
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
#include "MFileToolkit.h"

#include <regex>

#include <QDir>

MTK_NAMESPACE_BEGIN

MFileToolkit::MFileToolkit()
{

}

bool MFileToolkit::CreateDiretory(const QString& path, bool recursion/* = true*/)
{
    const QDir dir(path);
    if (dir.exists()) {
        return true;
    }

    if (recursion) {
        return dir.mkpath(path); // 创建多级目录
    }

    return dir.mkdir(path); // 只创建一级子目录，即必须保证上级目录存在
}

bool MFileToolkit::CreateFile(const QString& fileName, bool recursion)
{
    const QFileInfo fileInfo(fileName);
    CreateDiretory(fileInfo.path(), recursion);

    QFile file(fileName);
    const bool ok = file.open(QFile::WriteOnly);
    file.close();

    return ok;
}

bool MFileToolkit::CopyFile(const QString& sourceFile, const QString& newFile, bool isOverWrite)
{
    QFile file(sourceFile);
    if (!file.exists()) {
        return false;
    }

    const QFileInfo fileInfo(newFile);
    QDir().mkpath(fileInfo.path());

    if (isOverWrite) {
        QFile(newFile).remove(newFile);
    }

    return file.copy(newFile);
}

bool MFileToolkit::HasIllegalChar(const QString& str, bool checkEmpty)
{
    if (checkEmpty && str.isEmpty()) {
        return false;
    }

    const std::regex reg("[\\s\\\\/:\\*\\?\\\"<>\\|]");
    return std::regex_search(str.toStdString(), reg);
}

MTK_NAMESPACE_END
