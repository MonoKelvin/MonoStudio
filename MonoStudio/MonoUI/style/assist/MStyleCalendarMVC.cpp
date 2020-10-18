/****************************************************************************
 * @file    MStyleCalendarMVC.cpp
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
#include "MStyleCalendarMVC.h"

#include <qevent.h>
#include <qheaderview.h>

namespace mui
{
    MStyleCalendarModel::MStyleCalendarModel(QObject* parent)
        : QAbstractTableModel(parent),
          m_firstColumn(1),
          m_firstRow(1),
          m_date(QDate::currentDate()),
          m_minimumDate(QDate::fromJulianDay(1)),
          m_maximumDate(9999, 12, 31),
          m_shownYear(m_date.year(m_calendar)),
          m_shownMonth(m_date.month(m_calendar)),
          m_firstDay(QLocale().firstDayOfWeek()),
          m_horizontalHeaderFormat(MStyleCalendar::ShortDayNames),
          m_weekNumbersShown(true),
          m_view(nullptr)
    {
    }

    Qt::DayOfWeek MStyleCalendarModel::dayOfWeekForColumn(int column) const
    {
        int col = column - m_firstColumn;
        if (col < 0 || col > 6) {
            return Qt::Sunday;
        }
        int day = m_firstDay + col;
        if (day > 7) {
            day -= 7;
        }
        return Qt::DayOfWeek(day);
    }

    int MStyleCalendarModel::columnForDayOfWeek(Qt::DayOfWeek day) const
    {
        if (day < 1 || unsigned(day) > unsigned(7)) {
            return -1;
        }
        int column = (int)day - (int)m_firstDay;
        if (column < 0) {
            column += 7;
        }
        return column + m_firstColumn;
    }

    /*
    This simple algorithm tries to generate a valid date from the month shown.
    Some months don't contain a first day (e.g. Jan of -4713 year,
    so QDate (-4713, 1, 1) would be invalid). In that case we try to generate
    another valid date for that month. Later, returned date's day is the number of cells
    calendar widget will reserve for days before referenceDate. (E.g. if returned date's
    day is 16, that day will be placed in 3rd or 4th row, not in the 1st or 2nd row).
    Depending on referenceData we can change behaviour of Oct 1582. If referenceDate is 1st
    of Oct we render 1 Oct in 1st or 2nd row. If referenceDate is 17 of Oct we show always 16
    dates before 17 of Oct, and since this month contains the hole 5-14 Oct, the first of Oct
    will be rendered in 2nd or 3rd row, showing more dates from previous month.
    */
    QDate MStyleCalendarModel::referenceDate() const
    {
        // TODO: Check this
        int refDay = 1;
        while (refDay <= 31) {
            QDate refDate(m_shownYear, m_shownMonth, refDay, m_calendar);
            if (refDate.isValid()) {
                return refDate;
            }
            refDay += 1;
        }
        return QDate();
    }

    int MStyleCalendarModel::columnForFirstOfMonth(QDate date) const
    {
        return (columnForDayOfWeek(static_cast<Qt::DayOfWeek>(m_calendar.dayOfWeek(date)))
                - (date.day(m_calendar) % 7) + 8) % 7;
    }

    QDate MStyleCalendarModel::dateForCell(int row, int column) const
    {
        if (row < m_firstRow || row > m_firstRow + RowCount - 1 ||
            column < m_firstColumn || column > m_firstColumn + ColumnCount - 1) {
            return QDate();
        }
        const QDate refDate = referenceDate();
        if (!refDate.isValid()) {
            return QDate();
        }

        const int columnForFirstOfShownMonth = columnForFirstOfMonth(refDate);
        if (columnForFirstOfShownMonth - m_firstColumn < MinimumDayOffset) {
            row -= 1;
        }

        const int requestedDay =
            7 * (row - m_firstRow) + column - columnForFirstOfShownMonth - refDate.day(m_calendar) + 1;
        return refDate.addDays(requestedDay);
    }

    void MStyleCalendarModel::cellForDate(QDate date, int* row, int* column) const
    {
        if (!row && !column) {
            return;
        }

        if (row) {
            *row = -1;
        }
        if (column) {
            *column = -1;
        }

        const QDate refDate = referenceDate();
        if (!refDate.isValid()) {
            return;
        }

        const int columnForFirstOfShownMonth = columnForFirstOfMonth(refDate);
        const int requestedPosition = (refDate.daysTo(date) - m_firstColumn +
                                       columnForFirstOfShownMonth + refDate.day(m_calendar) - 1);

        int c = requestedPosition % 7;
        int r = requestedPosition / 7;
        if (c < 0) {
            c += 7;
            r -= 1;
        }

        if (columnForFirstOfShownMonth - m_firstColumn < MinimumDayOffset) {
            r += 1;
        }

        if (r < 0 || r > RowCount - 1 || c < 0 || c > ColumnCount - 1) {
            return;
        }

        if (row) {
            *row = r + m_firstRow;
        }
        if (column) {
            *column = c + m_firstColumn;
        }
    }

    QString MStyleCalendarModel::dayName(Qt::DayOfWeek day) const
    {
        switch (m_horizontalHeaderFormat) {
        case MStyleCalendar::SingleLetterDayNames: {
            QString standaloneDayName = m_view->locale().standaloneDayName(day, QLocale::NarrowFormat);
            if (standaloneDayName == m_view->locale().dayName(day, QLocale::NarrowFormat)) {
                return standaloneDayName.left(1);
            }
            return standaloneDayName;
        }
        case MStyleCalendar::ShortDayNames:
            return m_view->locale().dayName(day, QLocale::ShortFormat);
        case MStyleCalendar::LongDayNames:
            return m_view->locale().dayName(day, QLocale::LongFormat);
        default:
            break;
        }
        return QString();
    }

    QTextCharFormat MStyleCalendarModel::formatForCell(int row, int col) const
    {
        QPalette pal;
        QPalette::ColorGroup cg = QPalette::Active;
        if (m_view) {
            pal = m_view->palette();
            if (!m_view->isEnabled()) {
                cg = QPalette::Disabled;
            } else if (!m_view->isActiveWindow()) {
                cg = QPalette::Inactive;
            }
        }

        QTextCharFormat format;
        format.setFont(m_view->font());
        bool header = (m_weekNumbersShown && col == HeaderColumn)
                      || (m_horizontalHeaderFormat != MStyleCalendar::NoHorizontalHeader && row == HeaderRow);
        format.setBackground(pal.brush(cg, header ? QPalette::AlternateBase : QPalette::Base));
        format.setForeground(pal.brush(cg, QPalette::Text));
        if (header) {
            format.merge(m_headerFormat);
        }

        if (col >= m_firstColumn && col < m_firstColumn + ColumnCount) {
            Qt::DayOfWeek dayOfWeek = dayOfWeekForColumn(col);
            if (m_dayFormats.contains(dayOfWeek)) {
                format.merge(m_dayFormats.value(dayOfWeek));
            }
        }

        if (!header) {
            QDate date = dateForCell(row, col);
            format.merge(m_dateFormats.value(date));
            if (date < m_minimumDate || date > m_maximumDate) {
                format.setBackground(pal.brush(cg, QPalette::Window));
            }
            if (m_shownMonth != date.month(m_calendar)) {
                format.setForeground(pal.brush(QPalette::Disabled, QPalette::Text));
            }
        }
        return format;
    }

    QVariant MStyleCalendarModel::data(const QModelIndex& index, int role) const
    {
        if (role == Qt::TextAlignmentRole) {
            return (int) Qt::AlignCenter;
        }

        int row = index.row();
        int column = index.column();

        if (role == Qt::DisplayRole) {
            if (m_weekNumbersShown && column == HeaderColumn
                && row >= m_firstRow && row < m_firstRow + RowCount) {
                QDate date = dateForCell(row, columnForDayOfWeek(Qt::Monday));
                if (date.isValid()) {
                    return date.weekNumber();
                }
            }
            if (m_horizontalHeaderFormat != MStyleCalendar::NoHorizontalHeader && row == HeaderRow
                && column >= m_firstColumn && column < m_firstColumn + ColumnCount) {
                return dayName(dayOfWeekForColumn(column));
            }
            QDate date = dateForCell(row, column);
            if (date.isValid()) {
                return date.day(m_calendar);
            }
            return QString();
        }

        QTextCharFormat fmt = formatForCell(row, column);
        if (role == Qt::BackgroundRole) {
            return fmt.background().color();
        }
        if (role == Qt::ForegroundRole) {
            return fmt.foreground().color();
        }
        if (role == Qt::FontRole) {
            return fmt.font();
        }
        if (role == Qt::ToolTipRole) {
            return fmt.toolTip();
        }
        return QVariant();
    }

    Qt::ItemFlags MStyleCalendarModel::flags(const QModelIndex& index) const
    {
        QDate date = dateForCell(index.row(), index.column());
        if (!date.isValid()) {
            return QAbstractTableModel::flags(index);
        }
        if (date < m_minimumDate)
            return { };
        if (date > m_maximumDate)
            return { };
        return QAbstractTableModel::flags(index);
    }

    void MStyleCalendarModel::setDate(QDate d)
    {
        m_date = d;
        if (m_date < m_minimumDate) {
            m_date = m_minimumDate;
        } else if (m_date > m_maximumDate) {
            m_date = m_maximumDate;
        }
    }

    void MStyleCalendarModel::setCalendar(QCalendar c)
    {
        m_calendar = c;
        m_shownYear = m_date.year(c);
        m_shownMonth = m_date.month(c);
        internalUpdate();
        m_view->internalUpdate();
    }

    QCalendar MStyleCalendarModel::calendar() const
    {
        return m_calendar;
    }

    void MStyleCalendarModel::showMonth(int year, int month)
    {
        if (m_shownYear == year && m_shownMonth == month) {
            return;
        }

        m_shownYear = year;
        m_shownMonth = month;

        internalUpdate();
    }

    void MStyleCalendarModel::setMinimumDate(QDate d)
    {
        if (!d.isValid() || d == m_minimumDate) {
            return;
        }

        m_minimumDate = d;
        if (m_maximumDate < m_minimumDate) {
            m_maximumDate = m_minimumDate;
        }
        if (m_date < m_minimumDate) {
            m_date = m_minimumDate;
        }
        internalUpdate();
    }

    void MStyleCalendarModel::setMaximumDate(QDate d)
    {
        if (!d.isValid() || d == m_maximumDate) {
            return;
        }

        m_maximumDate = d;
        if (m_minimumDate > m_maximumDate) {
            m_minimumDate = m_maximumDate;
        }
        if (m_date > m_maximumDate) {
            m_date = m_maximumDate;
        }
        internalUpdate();
    }

    void MStyleCalendarModel::setRange(QDate min, QDate max)
    {
        m_minimumDate = min;
        m_maximumDate = max;
        if (m_minimumDate > m_maximumDate) {
            qSwap(m_minimumDate, m_maximumDate);
        }
        if (m_date < m_minimumDate) {
            m_date = m_minimumDate;
        }
        if (m_date > m_maximumDate) {
            m_date = m_maximumDate;
        }
        internalUpdate();
    }

    void MStyleCalendarModel::internalUpdate()
    {
        QModelIndex begin = index(0, 0);
        QModelIndex end = index(m_firstRow + RowCount - 1, m_firstColumn + ColumnCount - 1);
        emit dataChanged(begin, end);
        emit headerDataChanged(Qt::Vertical, 0, m_firstRow + RowCount - 1);
        emit headerDataChanged(Qt::Horizontal, 0, m_firstColumn + ColumnCount - 1);
    }

    void MStyleCalendarModel::setHorizontalHeaderFormat(MStyleCalendar::HorizontalHeaderFormat format)
    {
        if (m_horizontalHeaderFormat == format) {
            return;
        }

        int oldFormat = m_horizontalHeaderFormat;
        m_horizontalHeaderFormat = format;
        if (oldFormat == MStyleCalendar::NoHorizontalHeader) {
            m_firstRow = 1;
            insertRow(0);
        } else if (m_horizontalHeaderFormat == MStyleCalendar::NoHorizontalHeader) {
            m_firstRow = 0;
            removeRow(0);
        }
        internalUpdate();
    }

    void MStyleCalendarModel::setFirstColumnDay(Qt::DayOfWeek dayOfWeek)
    {
        if (m_firstDay == dayOfWeek) {
            return;
        }

        m_firstDay = dayOfWeek;
        internalUpdate();
    }

    Qt::DayOfWeek MStyleCalendarModel::firstColumnDay() const
    {
        return m_firstDay;
    }

    bool MStyleCalendarModel::weekNumbersShown() const
    {
        return m_weekNumbersShown;
    }

    void MStyleCalendarModel::setWeekNumbersShown(bool show)
    {
        if (m_weekNumbersShown == show) {
            return;
        }

        m_weekNumbersShown = show;
        if (show) {
            m_firstColumn = 1;
            insertColumn(0);
        } else {
            m_firstColumn = 0;
            removeColumn(0);
        }
        internalUpdate();
    }

    MStyleCalendarView::MStyleCalendarView(QWidget* parent)
        : QTableView(parent),
          readOnly(false),
          validDateClicked(false)
    {
        setTabKeyNavigation(false);
        setShowGrid(false);
        verticalHeader()->setVisible(false);
        horizontalHeader()->setVisible(false);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    QModelIndex MStyleCalendarView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
    {
        MStyleCalendarModel* calendarModel = qobject_cast<MStyleCalendarModel*>(model());
        if (!calendarModel) {
            return QTableView::moveCursor(cursorAction, modifiers);
        }

        QCalendar cal = calendarModel->calendar();

        if (readOnly) {
            return currentIndex();
        }

        QModelIndex index = currentIndex();
        QDate currentDate = static_cast<MStyleCalendarModel*>(model())->dateForCell(index.row(), index.column());
        switch (cursorAction) {
        case QAbstractItemView::MoveUp:
            currentDate = currentDate.addDays(-7);
            break;
        case QAbstractItemView::MoveDown:
            currentDate = currentDate.addDays(7);
            break;
        case QAbstractItemView::MoveLeft:
            currentDate = currentDate.addDays(isRightToLeft() ? 1 : -1);
            break;
        case QAbstractItemView::MoveRight:
            currentDate = currentDate.addDays(isRightToLeft() ? -1 : 1);
            break;
        case QAbstractItemView::MoveHome: {
            auto parts = cal.partsFromDate(currentDate);
            if (parts.isValid()) {
                parts.day = 1;
                currentDate = cal.dateFromParts(parts);
            }
        }
        break;
        case QAbstractItemView::MoveEnd: {
            auto parts = cal.partsFromDate(currentDate);
            if (parts.isValid()) {
                parts.day = cal.daysInMonth(parts.year, parts.month);
                currentDate = cal.dateFromParts(parts);
            }
        }
        break;
        case QAbstractItemView::MovePageUp:
            currentDate = currentDate.addMonths(-1, cal);
            break;
        case QAbstractItemView::MovePageDown:
            currentDate = currentDate.addMonths(1, cal);
            break;
        case QAbstractItemView::MoveNext:
        case QAbstractItemView::MovePrevious:
            return currentIndex();
        default:
            break;
        }
        emit changeDate(currentDate, true);
        return currentIndex();
    }

    void MStyleCalendarView::keyPressEvent(QKeyEvent* event)
    {
#ifdef QT_KEYPAD_NAVIGATION
        if (event->key() == Qt::Key_Select) {
            if (QApplicationPrivate::keypadNavigationEnabled()) {
                if (!hasEditFocus()) {
                    setEditFocus(true);
                    return;
                }
            }
        } else if (event->key() == Qt::Key_Back) {
            if (QApplicationPrivate::keypadNavigationEnabled() && hasEditFocus()) {
                if (qobject_cast<MStyleCalendarModel*>(model())) {
                    emit changeDate(origDate, true); //changes selection back to origDate, but doesn't activate
                    setEditFocus(false);
                    return;
                }
            }
        }
#endif

        if (!readOnly) {
            switch (event->key()) {
            case Qt::Key_Return:
            case Qt::Key_Enter:
            case Qt::Key_Select:
                emit editingFinished();
                return;
            default:
                break;
            }
        }
        QTableView::keyPressEvent(event);
    }

#if QT_CONFIG(wheelevent)
    void MStyleCalendarView::wheelEvent(QWheelEvent* event)
    {
        const int numDegrees = event->angleDelta().y() / 8;
        const int numSteps = numDegrees / 15;
        const QModelIndex index = currentIndex();
        MStyleCalendarModel* calendarModel = static_cast<MStyleCalendarModel*>(model());
        QDate currentDate = calendarModel->dateForCell(index.row(), index.column());
        currentDate = currentDate.addMonths(-numSteps, calendarModel->calendar());
        emit showDate(currentDate);
    }
#endif

    bool MStyleCalendarView::event(QEvent* event)
    {
#ifdef QT_KEYPAD_NAVIGATION
        if (event->type() == QEvent::FocusIn) {
            if (MStyleCalendarModel* calendarModel = qobject_cast<MStyleCalendarModel*>(model())) {
                origDate = calendarModel->m_date;
            }
        }
#endif

        return QTableView::event(event);
    }

    QDate MStyleCalendarView::handleMouseEvent(QMouseEvent* event)
    {
        MStyleCalendarModel* calendarModel = qobject_cast<MStyleCalendarModel*>(model());
        if (!calendarModel) {
            return QDate();
        }

        QPoint pos = event->pos();
        QModelIndex index = indexAt(pos);
        QDate date = calendarModel->dateForCell(index.row(), index.column());
        if (date.isValid() && date >= calendarModel->m_minimumDate
            && date <= calendarModel->m_maximumDate) {
            return date;
        }
        return QDate();
    }

    void MStyleCalendarView::mouseDoubleClickEvent(QMouseEvent* event)
    {
        MStyleCalendarModel* calendarModel = qobject_cast<MStyleCalendarModel*>(model());
        if (!calendarModel) {
            QTableView::mouseDoubleClickEvent(event);
            return;
        }

        if (readOnly) {
            return;
        }

        QDate date = handleMouseEvent(event);
        validDateClicked = false;
        if (date == calendarModel->m_date && !style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick)) {
            emit editingFinished();
        }
    }

    void MStyleCalendarView::mousePressEvent(QMouseEvent* event)
    {
        MStyleCalendarModel* calendarModel = qobject_cast<MStyleCalendarModel*>(model());
        if (!calendarModel) {
            QTableView::mousePressEvent(event);
            return;
        }

        if (readOnly) {
            return;
        }

        if (event->button() != Qt::LeftButton) {
            return;
        }

        QDate date = handleMouseEvent(event);
        if (date.isValid()) {
            validDateClicked = true;
            int row = -1, col = -1;
            static_cast<MStyleCalendarModel*>(model())->cellForDate(date, &row, &col);
            if (row != -1 && col != -1) {
                selectionModel()->setCurrentIndex(model()->index(row, col), QItemSelectionModel::NoUpdate);
            }
        } else {
            validDateClicked = false;
            event->ignore();
        }
    }

    void MStyleCalendarView::mouseMoveEvent(QMouseEvent* event)
    {
        MStyleCalendarModel* calendarModel = qobject_cast<MStyleCalendarModel*>(model());
        if (!calendarModel) {
            QTableView::mouseMoveEvent(event);
            return;
        }

        if (readOnly) {
            return;
        }

        if (validDateClicked) {
            QDate date = handleMouseEvent(event);
            if (date.isValid()) {
                int row = -1, col = -1;
                static_cast<MStyleCalendarModel*>(model())->cellForDate(date, &row, &col);
                if (row != -1 && col != -1) {
                    selectionModel()->setCurrentIndex(model()->index(row, col), QItemSelectionModel::NoUpdate);
                }
            }
        } else {
            event->ignore();
        }
    }

    void MStyleCalendarView::mouseReleaseEvent(QMouseEvent* event)
    {
        MStyleCalendarModel* calendarModel = qobject_cast<MStyleCalendarModel*>(model());
        if (!calendarModel) {
            QTableView::mouseReleaseEvent(event);
            return;
        }

        if (event->button() != Qt::LeftButton) {
            return;
        }

        if (readOnly) {
            return;
        }

        if (validDateClicked) {
            QDate date = handleMouseEvent(event);
            if (date.isValid()) {
                emit changeDate(date, true);
                emit clicked(date);
                if (style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick)) {
                    emit editingFinished();
                }
            }
            validDateClicked = false;
        } else {
            event->ignore();
        }
    }

    MStyleCalendarDelegate::~MStyleCalendarDelegate()
    {
    }

    void MStyleCalendarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                       const QModelIndex& index) const
    {
        QDate date = calendarWidgetPrivate->getModel()->dateForCell(index.row(), index.column());
        if (date.isValid()) {
            storedOption = option;
            QRect rect = option.rect;
            paintCell(painter, rect, date);
        } else {
            QItemDelegate::paint(painter, option, index);
        }
    }

    void MStyleCalendarDelegate::paintCell(QPainter* painter, const QRect& rect, const QDate& date) const
    {
        storedOption.rect = rect;
        int row = -1;
        int col = -1;
        calendarWidgetPrivate->getModel()->cellForDate(date, &row, &col);
        QModelIndex idx = calendarWidgetPrivate->getModel()->index(row, col);
        QItemDelegate::paint(painter, storedOption, idx);
    }

}
