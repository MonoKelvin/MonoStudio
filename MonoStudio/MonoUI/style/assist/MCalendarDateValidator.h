/****************************************************************************
 * @file    MCalendarDateValidator.h
 * @date    2020-10-17
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
#ifndef MCALENDARDATEVALIDATOR_H
#define MCALENDARDATEVALIDATOR_H

#include <qdatetime.h>

#include "MCalendarDateSectionValidator.h"

namespace mui
{
    class MONOUI_API MCalendarDateValidator
    {
    public:
        MCalendarDateValidator();
        ~MCalendarDateValidator();

        void handleKeyEvent(QKeyEvent* keyEvent, QCalendar cal);
        QString currentText(QCalendar cal) const;
        QDate currentDate() const
        {
            return m_currentDate;
        }

        void setFormat(const QString& format);
        void setInitialDate(QDate date, QCalendar cal);

        void setLocale(const QLocale& locale);

    private:
        void toNextToken();
        void toPreviousToken();
        void applyToDate(QCalendar cal);

        int countRepeat(const QString& str, int index) const;
        void clear();

        QStringList m_separators;
        std::vector<SectionToken> m_tokens;
        MCalendarYearValidator m_yearValidator;
        MCalendarMonthValidator m_monthValidator;
        MCalendarDayValidator m_dayValidator;

        int m_currentToken;

        QDate m_initialDate;
        QDate m_currentDate;

        MCalendarDateSectionValidator::Section m_lastSectionMove;
    };
}

#endif // MCALENDARDATEVALIDATOR_H
