/****************************************************************************
 * @file    MCalendarDateSectionValidator.h
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
#ifndef MCALENDARDATESECTIONVALIDATOR_H
#define MCALENDARDATESECTIONVALIDATOR_H

#include "defines.h"

#include <qcalendar.h>

namespace mui
{
    class MONOUI_API MCalendarDateSectionValidator
    {
    public:
        enum Section {
            NextSection,
            ThisSection,
            PrevSection
        };

        MCalendarDateSectionValidator() {}
        virtual ~MCalendarDateSectionValidator() {}

        virtual Section handleKey(int key) = 0;
        virtual QDate applyToDate(QDate date, QCalendar cal = QCalendar()) const = 0;
        virtual void setDate(QDate date, QCalendar cal = QCalendar()) = 0;
        virtual QString text() const = 0;
        virtual QString text(QDate date, QCalendar cal, int repeat) const = 0;

        QLocale m_locale;

    protected:
        static QString highlightString(const QString& str, int pos);
    };

    class MONOUI_API MCalendarDayValidator : public MCalendarDateSectionValidator
    {
    public:
        MCalendarDayValidator();

        virtual Section handleKey(int key) override;
        virtual QDate applyToDate(QDate date, QCalendar cal) const override;
        virtual void setDate(QDate date, QCalendar cal) override;
        virtual QString text() const override;
        virtual QString text(QDate date, QCalendar cal, int repeat) const override;

    private:
        int m_pos;
        int m_day;
        int m_oldDay;
    };

    class MCalendarMonthValidator : public MCalendarDateSectionValidator
    {

    public:
        MCalendarMonthValidator();

        virtual Section handleKey(int key) override;
        virtual QDate applyToDate(QDate date, QCalendar cal) const override;
        virtual void setDate(QDate date, QCalendar cal) override;
        virtual QString text() const override;
        virtual QString text(QDate date, QCalendar cal, int repeat) const override;

    private:
        int m_pos;
        int m_month;
        int m_oldMonth;
    };

    class MCalendarYearValidator : public MCalendarDateSectionValidator
    {

    public:
        MCalendarYearValidator();

        virtual Section handleKey(int key) override;
        virtual QDate applyToDate(QDate date, QCalendar cal) const override;
        virtual void setDate(QDate date, QCalendar cal) override;
        virtual QString text() const override;
        virtual QString text(QDate date, QCalendar cal, int repeat) const override;

    private:
        int pow10(int n);

    private:
        int m_pos;
        int m_year;
        int m_oldYear;
    };

    struct SectionToken {
        Q_DECL_CONSTEXPR SectionToken(MCalendarDateSectionValidator* v, int rep)
            : validator(v), repeat(rep)
        {}

        MCalendarDateSectionValidator* validator;
        int repeat;
    };
}

#endif // MCALENDARDATESECTIONVALIDATOR_H
