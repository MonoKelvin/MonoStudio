/****************************************************************************
 * @file    MStyleCalendarMVC.h
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
#ifndef MSTYLECALENDARMVC_H
#define MSTYLECALENDARMVC_H

#include "defines.h"

#include <qabstractitemmodel.h>
#include <qtextformat.h>
#include <qcalendar.h>
#include <qtableview.h>
#include <qitemdelegate.h>

#include "../MStyleCalendar.h"

namespace mui
{
    enum {
        RowCount = 6,
        ColumnCount = 7,
        HeaderColumn = 0,
        HeaderRow = 0,
        MinimumDayOffset = 1
    };

    template <typename T>
    class StaticDayOfWeekAssociativeArray
    {
        bool contained[7];
        T data[7];

        static Q_DECL_CONSTEXPR int day2idx(Qt::DayOfWeek day) noexcept
        {
            return int(day) - 1;    // alt: day % 7
        }
    public:
        Q_DECL_CONSTEXPR StaticDayOfWeekAssociativeArray() noexcept(noexcept(T()))
#ifdef Q_COMPILER_CONSTEXPR
            : contained {}, data {} // arrays require uniform initialization
#else
            :
            contained(), data()
#endif
        {}

        Q_DECL_CONSTEXPR bool contains(Qt::DayOfWeek day) const noexcept
        {
            return contained[day2idx(day)];
        }
        Q_DECL_CONSTEXPR const T& value(Qt::DayOfWeek day) const noexcept
        {
            return data[day2idx(day)];
        }

        Q_DECL_RELAXED_CONSTEXPR T& operator[](Qt::DayOfWeek day) noexcept
        {
            const int idx = day2idx(day);
            contained[idx] = true;
            return data[idx];
        }

        Q_DECL_RELAXED_CONSTEXPR void insert(Qt::DayOfWeek day, T v) noexcept
        {
            operator[](day).swap(v);
        }
    };

    class MONOUI_API MStyleCalendarView : public QTableView
    {
        Q_OBJECT
    public:
        MStyleCalendarView(QWidget* parent = nullptr);

        void internalUpdate()
        {
            updateGeometries();
        }

        void setReadOnly(bool enable);

        virtual void keyboardSearch(const QString& search) override
        {
            Q_UNUSED(search)
        }

    Q_SIGNALS:
        void showDate(const QDate& date);
        void changeDate(const QDate&, bool changeMonth);
        void clicked(const QDate&);
        void editingFinished();

    protected:
        QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
        void mouseDoubleClickEvent(QMouseEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
#if QT_CONFIG(wheelevent)
        void wheelEvent(QWheelEvent* event) override;
#endif
        void keyPressEvent(QKeyEvent* event) override;
        bool event(QEvent* event) override;

        QDate handleMouseEvent(QMouseEvent* event);
    public:
        bool readOnly;
    private:
        bool validDateClicked;
#ifdef QT_KEYPAD_NAVIGATION
        QDate origDate;
#endif
    };

    class MONOUI_API MStyleCalendarModel : public QAbstractTableModel
    {
        Q_OBJECT
        friend MStyleCalendarView;
        friend MStyleCalendar;
    public:
        MStyleCalendarModel(QObject* parent = nullptr);

        int rowCount(const QModelIndex&) const override
        {
            return RowCount + m_firstRow;
        }
        int columnCount(const QModelIndex&) const override
        {
            return ColumnCount + m_firstColumn;
        }
        QVariant data(const QModelIndex& index, int role) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override
        {
            beginInsertRows(parent, row, row + count - 1);
            endInsertRows();
            return true;
        }
        bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override
        {
            beginInsertColumns(parent, column, column + count - 1);
            endInsertColumns();
            return true;
        }
        bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override
        {
            beginRemoveRows(parent, row, row + count - 1);
            endRemoveRows();
            return true;
        }
        bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override
        {
            beginRemoveColumns(parent, column, column + count - 1);
            endRemoveColumns();
            return true;
        }

        void showMonth(int year, int month);
        void setDate(QDate d);

        void setCalendar(QCalendar c);
        QCalendar calendar() const;

        void setMinimumDate(QDate date);
        void setMaximumDate(QDate date);

        void setRange(QDate min, QDate max);

        void setHorizontalHeaderFormat(MStyleCalendar::HorizontalHeaderFormat format);

        void setFirstColumnDay(Qt::DayOfWeek dayOfWeek);
        Qt::DayOfWeek firstColumnDay() const;

        bool weekNumbersShown() const;
        void setWeekNumbersShown(bool show);

        QTextCharFormat formatForCell(int row, int col) const;
        Qt::DayOfWeek dayOfWeekForColumn(int section) const;
        int columnForDayOfWeek(Qt::DayOfWeek day) const;
        QDate dateForCell(int row, int column) const;
        void cellForDate(QDate date, int* row, int* column) const;
        QString dayName(Qt::DayOfWeek day) const;

        void setView(MStyleCalendarView* view)
        {
            m_view = view;
        }

        void internalUpdate();
        QDate referenceDate() const;
        int columnForFirstOfMonth(QDate date) const;

        QString monthName(const QLocale& locale, int month)
        {
            return m_calendar.standaloneMonthName(locale, month, m_shownYear, QLocale::LongFormat);
        }

    private:
        int m_firstColumn;
        int m_firstRow;
        QCalendar m_calendar;
        QDate m_date;
        QDate m_minimumDate;
        QDate m_maximumDate;
        int m_shownYear;
        int m_shownMonth;
        Qt::DayOfWeek m_firstDay;
        MStyleCalendar::HorizontalHeaderFormat m_horizontalHeaderFormat;
        bool m_weekNumbersShown;
        StaticDayOfWeekAssociativeArray<QTextCharFormat> m_dayFormats;
        QMap<QDate, QTextCharFormat> m_dateFormats;
        QTextCharFormat m_headerFormat;
        MStyleCalendarView* m_view;
    };

    // ### Qt6: QStyledItemDelegate
    class MONOUI_API MStyleCalendarDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        explicit MStyleCalendarDelegate(MStyleCalendar* w)
            : QItemDelegate(w), calendarWidgetPrivate(w)
        {}
        ~MStyleCalendarDelegate();

        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const override;

        virtual void paintCell(QPainter* painter, const QRect& rect, const QDate& date) const;

    protected:
        MStyleCalendar* calendarWidgetPrivate;
        mutable QStyleOptionViewItem storedOption;
    };
}

#endif // MSTYLECALENDARMVC_H
