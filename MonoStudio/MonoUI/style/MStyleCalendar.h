/****************************************************************************
 * @file    MStyleCalendar.h
 * @date    2020-09-19
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
#ifndef MSTYLECALENDAR_H
#define MSTYLECALENDAR_H

#include "MStyleWindow.h"

#include <qtoolbutton.h>
#include <qdatetime.h>

class QDate;
class QTextCharFormat;
class QLabel;
class QPushButton;
class QSpacerItem;
class QItemSelectionModel;
class QMenu;
class QSpinBox;

namespace mui
{
    class MStyleCalendarModel;
    class MStyleCalendarView;
    class MStyleCalendarDelegate;

    struct SCalendarStyle {
    };

    //Private tool button class
    class MONOUI_API MCalToolButton: public QToolButton
    {
    public:
        explicit MCalToolButton(QWidget* parent) : QToolButton(parent) {}

    protected:
        void paintEvent(QPaintEvent* e) override;
    };

    class MONOUI_API QPrevNextCalButton : public QToolButton
    {
        Q_OBJECT
    public:
        explicit QPrevNextCalButton(QWidget* parent) : QToolButton(parent) {}
    protected:
        void paintEvent(QPaintEvent*) override;
    };

    class MONOUI_API MStyleCalendar : public QWidget
    {
        Q_OBJECT
        Q_ENUMS(Qt::DayOfWeek)
        Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate)
        Q_PROPERTY(QDate minimumDate READ minimumDate WRITE setMinimumDate)
        Q_PROPERTY(QDate maximumDate READ maximumDate WRITE setMaximumDate)
        Q_PROPERTY(Qt::DayOfWeek firstDayOfWeek READ firstDayOfWeek WRITE setFirstDayOfWeek)
        Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
        Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
        Q_PROPERTY(HorizontalHeaderFormat horizontalHeaderFormat READ horizontalHeaderFormat WRITE setHorizontalHeaderFormat)
        Q_PROPERTY(VerticalHeaderFormat verticalHeaderFormat READ verticalHeaderFormat WRITE setVerticalHeaderFormat)
        Q_PROPERTY(bool navigationBarVisible READ isNavigationBarVisible WRITE setNavigationBarVisible)
        Q_PROPERTY(bool dateEditEnabled READ isDateEditEnabled WRITE setDateEditEnabled)
        Q_PROPERTY(int dateEditAcceptDelay READ dateEditAcceptDelay WRITE setDateEditAcceptDelay)

    public:
        enum HorizontalHeaderFormat {
            NoHorizontalHeader,         // 没有星期几的标题头部
            SingleLetterDayNames,       // 单个字，如一、二、三 ...
            ShortDayNames,              // 简短名，如周一、周二 ...
            LongDayNames                // 长名字，如星期一、星期二 ...
        };
        Q_ENUM(HorizontalHeaderFormat)

        enum VerticalHeaderFormat {
            NoVerticalHeader,
            ISOWeekNumbers
        };
        Q_ENUM(VerticalHeaderFormat)

        enum SelectionMode {
            NoSelection,
            SingleSelection
        };
        Q_ENUM(SelectionMode)

        explicit MStyleCalendar(QWidget* parent = nullptr);
        ~MStyleCalendar();

        QSize sizeHint() const override;
        QSize minimumSizeHint() const override;

        QDate selectedDate() const;

        int yearShown() const;
        int monthShown() const;

        QDate minimumDate() const;
        void setMinimumDate(const QDate& date);

        QDate maximumDate() const;
        void setMaximumDate(const QDate& date);

        Qt::DayOfWeek firstDayOfWeek() const;
        void setFirstDayOfWeek(Qt::DayOfWeek dayOfWeek);

        bool isNavigationBarVisible() const;
        bool isGridVisible() const;

        QCalendar calendar() const;
        void setCalendar(QCalendar calendar);

        SelectionMode selectionMode() const;
        void setSelectionMode(SelectionMode mode);

        HorizontalHeaderFormat horizontalHeaderFormat() const;
        void setHorizontalHeaderFormat(HorizontalHeaderFormat format);

        VerticalHeaderFormat verticalHeaderFormat() const;
        void setVerticalHeaderFormat(VerticalHeaderFormat format);

        QTextCharFormat headerTextFormat() const;
        void setHeaderTextFormat(const QTextCharFormat& format);

        QTextCharFormat weekdayTextFormat(Qt::DayOfWeek dayOfWeek) const;
        void setWeekdayTextFormat(Qt::DayOfWeek dayOfWeek, const QTextCharFormat& format);

        QMap<QDate, QTextCharFormat> dateTextFormat() const;
        QTextCharFormat dateTextFormat(const QDate& date) const;
        void setDateTextFormat(const QDate& date, const QTextCharFormat& format);

        bool isDateEditEnabled() const;
        void setDateEditEnabled(bool enable);

        int dateEditAcceptDelay() const;
        void setDateEditAcceptDelay(int delay);

        MStyleCalendarModel* getModel(void) const noexcept
        {
            return m_model;
        }

    protected:
        bool event(QEvent* event) override;
        bool eventFilter(QObject* watched, QEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        void keyPressEvent(QKeyEvent* event) override;

        void updateCell(const QDate& date);
        void updateCells();

    Q_SIGNALS:
        void selectionChanged();
        void clicked(const QDate& date);
        void activated(const QDate& date);
        void currentPageChanged(int year, int month);

    public Q_SLOTS:
        void setSelectedDate(const QDate& date);
        void setDateRange(const QDate& min, const QDate& max);
        void setCurrentPage(int year, int month);
        void setGridVisible(bool show);
        void setNavigationBarVisible(bool visible);
        void showNextMonth();
        void showPreviousMonth();
        void showNextYear();
        void showPreviousYear();
        void showSelectedDate();
        void showToday();

    private Q_SLOTS:
        void _slotShowDate(const QDate& date);
        void _slotChangeDate(const QDate& date);
        void _slotChangeDateWithMonth(const QDate& date, bool changeMonth);
        void _editingFinished();
        void _prevMonthClicked();
        void _nextMonthClicked();
        void _yearEditingFinished();
        void _yearClicked();
        void _monthChanged(QAction* act);

    private:
        void showMonth(int year, int month);
        void update();

        void createNavigationBar(QWidget* widget);
        void updateButtonIcons();
        void updateMonthMenu();
        void updateMonthMenuNames();
        void updateNavigationBar();
        void updateCurrentPage(const QDate& newDate);
        inline QDate getCurrentDate();
        void setNavigatorEnabled(bool enable);

    private:
        MStyleCalendarModel* m_model;
        MStyleCalendarView* m_view;
        MStyleCalendarDelegate* m_delegate;
        QItemSelectionModel* m_selection;
        bool m_dateEditEnabled;

        QToolButton* nextMonth;
        QToolButton* prevMonth;
        MCalToolButton* monthButton;
        QMenu* monthMenu;
        QMap<int, QAction*> monthToAction;
        MCalToolButton* yearButton;
        QSpinBox* yearEdit;
        QWidget* navBarBackground;
        QSpacerItem* spaceHolder;

        bool navBarVisible;
        mutable QSize cachedSizeHint;
        Qt::FocusPolicy oldFocusPolicy;

    private:
        Q_DISABLE_COPY(MStyleCalendar)

    };

}

#endif // MSTYLECALENDAR_H
