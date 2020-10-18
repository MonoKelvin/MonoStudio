/****************************************************************************
 * @file    MStyleCalendar.cpp
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
#include "MStyleCalendar.h"

#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qmenu.h>
#include <qlayout.h>
#include <qevent.h>
#include <qtextformat.h>
#include <qheaderview.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qmenu.h>
#include <qapplication.h>
#include <qbasictimer.h>
#include <qstylepainter.h>
#include <qcalendar.h>

#include "assist/MCalendarDateSectionValidator.h"
#include "assist/MCalendarDateValidator.h"
#include "assist/MStyleCalendarMVC.h"

namespace mui
{
    MStyleCalendar::MStyleCalendar(QWidget* parent)
        : QWidget(parent)
    {
        m_model = nullptr;
        m_view = nullptr;
        m_delegate = nullptr;
        m_selection = nullptr;
        m_dateEditEnabled = false;
        navBarVisible = true;
        //m_navigator = nullptr;
        oldFocusPolicy = Qt::StrongFocus;

        setAutoFillBackground(true);
        setBackgroundRole(QPalette::Window);

        QVBoxLayout* layoutV = new QVBoxLayout(this);
        layoutV->setContentsMargins(QMargins());
        m_model = new MStyleCalendarModel(this);
        QTextCharFormat fmt;
        fmt.setForeground(QBrush(Qt::red));
        m_model->m_dayFormats.insert(Qt::Saturday, fmt);
        m_model->m_dayFormats.insert(Qt::Sunday, fmt);
        m_view = new MStyleCalendarView(this);
        m_view->setObjectName(QLatin1String("qt_calendar_calendarview"));
        m_view->setModel(m_model);
        m_model->setView(m_view);
        m_view->setSelectionBehavior(QAbstractItemView::SelectItems);
        m_view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_view->horizontalHeader()->setSectionsClickable(false);
        m_view->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_view->verticalHeader()->setSectionsClickable(false);
        m_selection = m_view->selectionModel();
        createNavigationBar(this);
        m_view->setFrameStyle(QFrame::NoFrame);
        m_delegate = new MStyleCalendarDelegate(this);
        m_view->setItemDelegate(m_delegate);
        update();
        updateNavigationBar();
        setFocusPolicy(Qt::StrongFocus);
        setFocusProxy(m_view);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        connect(m_view, &MStyleCalendarView::showDate, this, &MStyleCalendar::_slotShowDate);
        connect(m_view, &MStyleCalendarView::changeDate, this, &MStyleCalendar::_slotChangeDateWithMonth);
        connect(m_view, &MStyleCalendarView::clicked, this, &MStyleCalendar::clicked);
        connect(m_view, &MStyleCalendarView::editingFinished, this, &MStyleCalendar::_editingFinished);

        connect(prevMonth, &QToolButton::clicked, this, &MStyleCalendar::_prevMonthClicked);
        connect(nextMonth, &QToolButton::clicked, this, &MStyleCalendar::_nextMonthClicked);
        connect(yearButton, &QToolButton::clicked, this, &MStyleCalendar::_yearClicked);
        connect(monthMenu, &QMenu::triggered, this, &MStyleCalendar::_monthChanged);
        connect(yearEdit, &QSpinBox::editingFinished, this, &MStyleCalendar::_yearEditingFinished);

        layoutV->setContentsMargins(QMargins());
        layoutV->setSpacing(0);
        layoutV->addWidget(navBarBackground);
        layoutV->addWidget(m_view);

        //m_navigator = new QCalendarTextNavigator(this);
        setDateEditEnabled(true);
    }

    MStyleCalendar::~MStyleCalendar()
    {
    }

    QSize MStyleCalendar::sizeHint() const
    {
        return minimumSizeHint();
    }

    QSize MStyleCalendar::minimumSizeHint() const
    {
        if (cachedSizeHint.isValid()) {
            return cachedSizeHint;
        }

        ensurePolished();

        int w = 0;
        int h = 0;

        int end = 53;
        int rows = 7;
        int cols = 8;

        QStyleOption option;
        option.initFrom(this);
        const int marginH = (style()->pixelMetric(QStyle::PM_FocusFrameHMargin, &option) + 1) * 2;

        if (horizontalHeaderFormat() == MStyleCalendar::NoHorizontalHeader) {
            rows = 6;
        } else {
            for (int i = 1; i <= 7; i++) {
                QFontMetrics fm(m_model->formatForCell(0, i).font());
                w = qMax(w, fm.horizontalAdvance(m_model->dayName(m_model->dayOfWeekForColumn(i))) + marginH);
                h = qMax(h, fm.height());
            }
        }

        if (verticalHeaderFormat() == MStyleCalendar::NoVerticalHeader) {
            cols = 7;
        } else {
            for (int i = 1; i <= 6; i++) {
                QFontMetrics fm(m_model->formatForCell(i, 0).font());
                for (int j = 1; j < end; j++) {
                    w = qMax(w, fm.horizontalAdvance(QString::number(j)) + marginH);
                }
                h = qMax(h, fm.height());
            }
        }

        QFontMetrics fm(m_model->formatForCell(1, 1).font());
        for (int i = 1; i <= end; i++) {
            w = qMax(w, fm.horizontalAdvance(QString::number(i)) + marginH);
            h = qMax(h, fm.height());
        }

        if (m_view->showGrid()) {
            // hardcoded in tableview
            w += 1;
            h += 1;
        }

        w += 1; // default column span

        h = qMax(h, m_view->verticalHeader()->minimumSectionSize());
        w = qMax(w, m_view->horizontalHeader()->minimumSectionSize());

        //add the size of the header.
        QSize headerSize(0, 0);
        if (navBarVisible) {
            int headerH = navBarBackground->sizeHint().height();
            int headerW = 0;

            headerW += prevMonth->sizeHint().width();
            headerW += nextMonth->sizeHint().width();

            QFontMetrics fm = monthButton->fontMetrics();
            int monthW = 0;
            for (int i = 1; i < 12; i++) {
                QString monthName = m_model->monthName(locale(), i);
                monthW = qMax(monthW, fm.boundingRect(monthName).width());
            }
            const int buttonDecoMargin = monthButton->sizeHint().width() - fm.boundingRect(monthButton->text()).width();
            headerW += monthW + buttonDecoMargin;

            fm = yearButton->fontMetrics();
            headerW += fm.boundingRect(QLatin1String("5555")).width() + buttonDecoMargin;

            headerSize = QSize(headerW, headerH);
        }
        w *= cols;
        w = qMax(headerSize.width(), w);
        h = (h * rows) + headerSize.height();
        QMargins cm = contentsMargins();
        w += cm.left() + cm.right();
        h += cm.top() + cm.bottom();
        cachedSizeHint = QSize(w, h);
        return cachedSizeHint;
    }

    void MStyleCalendar::setNavigatorEnabled(bool enable)
    {
        Q_UNUSED(enable)
        /*bool navigatorEnabled = (m_navigator->widget() != nullptr);
        if (enable == navigatorEnabled) {
            return;
        }

        if (enable) {
            m_navigator->setWidget(q);
            connect(m_navigator, SIGNAL(dateChanged(QDate)),
                       q, SLOT(_slotChangeDate(QDate)));
            connect(m_navigator, SIGNAL(editingFinished()),
                       q, SLOT(_editingFinished()));
            m_view->installEventFilter(m_navigator);
        } else {
            m_navigator->setWidget(nullptr);
            disconnect(m_navigator, SIGNAL(dateChanged(QDate)),
                          q, SLOT(_slotChangeDate(QDate)));
            disconnect(m_navigator, SIGNAL(editingFinished()),
                          q, SLOT(_editingFinished()));
            m_view->removeEventFilter(m_navigator);
        }*/
    }

    QDate MStyleCalendar::selectedDate() const
    {
        return m_model->m_date;
    }

    void MStyleCalendar::setSelectedDate(const QDate& date)
    {

        if (m_model->m_date == date && date == getCurrentDate()) {
            return;
        }

        if (!date.isValid()) {
            return;
        }

        m_model->setDate(date);
        update();
        QDate newDate = m_model->m_date;
        QCalendar cal = m_model->m_calendar;
        showMonth(newDate.year(cal), newDate.month(cal));
        emit selectionChanged();
    }

    int MStyleCalendar::yearShown() const
    {
        return m_model->m_shownYear;
    }

    int MStyleCalendar::monthShown() const
    {
        return m_model->m_shownMonth;
    }

    void MStyleCalendar::setCurrentPage(int year, int month)
    {

        QDate currentDate = getCurrentDate();
        QCalendar cal = m_model->m_calendar;
        int day = currentDate.day(cal);
        int daysInMonths = cal.daysInMonth(year, month);
        if (day > daysInMonths) {
            day = daysInMonths;
        }

        showMonth(year, month);

        QDate newDate(year, month, day, m_model->m_calendar);
        int row = -1, col = -1;
        m_model->cellForDate(newDate, &row, &col);
        if (row != -1 && col != -1) {
            m_view->selectionModel()->setCurrentIndex(m_model->index(row, col),
                    QItemSelectionModel::NoUpdate);
        }
    }

    void MStyleCalendar::showNextMonth()
    {
        int year = yearShown();
        int month = monthShown();
        if (month == m_model->m_calendar.maximumMonthsInYear()) {
            ++year;
            month = 1;
        } else {
            ++month;
        }

        setCurrentPage(year, month);
    }

    /*!
        Shows the previous month relative to the currently displayed
        month. Note that the selected date is not changed.

        \sa showNextMonth(), setCurrentPage(), setSelectedDate()
    */

    void MStyleCalendar::showPreviousMonth()
    {

        int year = yearShown();
        int month = monthShown();
        if (month == 1) {
            --year;
            month = m_model->m_calendar.maximumMonthsInYear();
        } else {
            --month;
        }
        setCurrentPage(year, month);
    }

    void MStyleCalendar::showNextYear()
    {
        int year = yearShown();
        int month = monthShown();
        ++year;
        setCurrentPage(year, month);
    }

    void MStyleCalendar::showPreviousYear()
    {
        int year = yearShown();
        int month = monthShown();
        --year;
        setCurrentPage(year, month);
    }

    void MStyleCalendar::showSelectedDate()
    {
        QDate currentDate = selectedDate();
        setCurrentPage(currentDate.year(m_model->m_calendar), currentDate.month(m_model->m_calendar));
    }

    void MStyleCalendar::showToday()
    {
        QDate currentDate = QDate::currentDate();
        setCurrentPage(currentDate.year(m_model->m_calendar), currentDate.month(m_model->m_calendar));
    }

    QDate MStyleCalendar::minimumDate() const
    {
        return m_model->m_minimumDate;
    }

    void MStyleCalendar::setMinimumDate(const QDate& date)
    {
        if (!date.isValid() || m_model->m_minimumDate == date) {
            return;
        }

        QDate oldDate = m_model->m_date;
        m_model->setMinimumDate(date);
        yearEdit->setMinimum(m_model->m_minimumDate.year(m_model->m_calendar));
        updateMonthMenu();
        QDate newDate = m_model->m_date;
        if (oldDate != newDate) {
            update();
            showMonth(newDate.year(m_model->m_calendar), newDate.month(m_model->m_calendar));
            //m_navigator->setDate(newDate);
            emit selectionChanged();
        }
    }

    QDate MStyleCalendar::maximumDate() const
    {
        return m_model->m_maximumDate;
    }

    void MStyleCalendar::setMaximumDate(const QDate& date)
    {

        if (!date.isValid() || m_model->m_maximumDate == date) {
            return;
        }

        QDate oldDate = m_model->m_date;
        m_model->setMaximumDate(date);
        yearEdit->setMaximum(m_model->m_maximumDate.year(m_model->m_calendar));
        updateMonthMenu();
        QDate newDate = m_model->m_date;
        if (oldDate != newDate) {
            update();
            showMonth(newDate.year(m_model->m_calendar), newDate.month(m_model->m_calendar));
            //m_navigator->setDate(newDate);
            emit selectionChanged();
        }
    }

    void MStyleCalendar::setDateRange(const QDate& min, const QDate& max)
    {

        if (m_model->m_minimumDate == min && m_model->m_maximumDate == max) {
            return;
        }
        if (!min.isValid() || !max.isValid()) {
            return;
        }

        QDate oldDate = m_model->m_date;
        m_model->setRange(min, max);
        yearEdit->setMinimum(m_model->m_minimumDate.year(m_model->m_calendar));
        yearEdit->setMaximum(m_model->m_maximumDate.year(m_model->m_calendar));
        updateMonthMenu();
        QDate newDate = m_model->m_date;
        if (oldDate != newDate) {
            update();
            showMonth(newDate.year(m_model->m_calendar), newDate.month(m_model->m_calendar));
            //m_navigator->setDate(newDate);
            emit selectionChanged();
        }
    }

    void MStyleCalendar::setHorizontalHeaderFormat(MStyleCalendar::HorizontalHeaderFormat format)
    {

        if (m_model->m_horizontalHeaderFormat == format) {
            return;
        }

        m_model->setHorizontalHeaderFormat(format);
        cachedSizeHint = QSize();
        m_view->viewport()->update();
        m_view->updateGeometry();
    }

    MStyleCalendar::HorizontalHeaderFormat MStyleCalendar::horizontalHeaderFormat() const
    {
        return m_model->m_horizontalHeaderFormat;
    }

    MStyleCalendar::VerticalHeaderFormat MStyleCalendar::verticalHeaderFormat() const
    {
        if (m_model->weekNumbersShown()) {
            return MStyleCalendar::ISOWeekNumbers;
        }

        return MStyleCalendar::NoVerticalHeader;
    }

    void MStyleCalendar::setVerticalHeaderFormat(MStyleCalendar::VerticalHeaderFormat format)
    {
        bool show = false;
        if (format == MStyleCalendar::ISOWeekNumbers) {
            show = true;
        }
        if (m_model->weekNumbersShown() == show) {
            return;
        }
        m_model->setWeekNumbersShown(show);
        cachedSizeHint = QSize();
        m_view->viewport()->update();
        m_view->updateGeometry();
    }

    bool MStyleCalendar::isGridVisible() const
    {
        return m_view->showGrid();
    }

    QCalendar MStyleCalendar::calendar() const
    {
        return m_model->m_calendar;
    }

    void MStyleCalendar::setCalendar(QCalendar c)
    {
        m_model->setCalendar(c);
        updateMonthMenuNames();
        yearEdit->setMinimum(m_model->m_minimumDate.year(m_model->m_calendar));
        yearEdit->setMaximum(m_model->m_maximumDate.year(m_model->m_calendar));
        updateNavigationBar();
    }

    void MStyleCalendar::setGridVisible(bool show)
    {
        m_view->setShowGrid(show);
        cachedSizeHint = QSize();
        m_view->viewport()->update();
        m_view->updateGeometry();
    }

    MStyleCalendar::SelectionMode MStyleCalendar::selectionMode() const
    {
        return m_view->readOnly ? MStyleCalendar::NoSelection : MStyleCalendar::SingleSelection;
    }

    void MStyleCalendar::setSelectionMode(SelectionMode mode)
    {
        m_view->readOnly = (mode == MStyleCalendar::NoSelection);
        setNavigatorEnabled(isDateEditEnabled() && (selectionMode() != MStyleCalendar::NoSelection));
        update();
    }

    void MStyleCalendar::setFirstDayOfWeek(Qt::DayOfWeek dayOfWeek)
    {
        if ((Qt::DayOfWeek)m_model->firstColumnDay() == dayOfWeek) {
            return;
        }

        m_model->setFirstColumnDay(dayOfWeek);
        update();
    }

    Qt::DayOfWeek MStyleCalendar::firstDayOfWeek() const
    {
        return (Qt::DayOfWeek)m_model->firstColumnDay();
    }

    QTextCharFormat MStyleCalendar::headerTextFormat() const
    {
        return m_model->m_headerFormat;
    }

    void MStyleCalendar::setHeaderTextFormat(const QTextCharFormat& format)
    {
        m_model->m_headerFormat = format;
        cachedSizeHint = QSize();
        m_view->viewport()->update();
        m_view->updateGeometry();
    }

    QTextCharFormat MStyleCalendar::weekdayTextFormat(Qt::DayOfWeek dayOfWeek) const
    {
        return m_model->m_dayFormats.value(dayOfWeek);
    }

    void MStyleCalendar::setWeekdayTextFormat(Qt::DayOfWeek dayOfWeek, const QTextCharFormat& format)
    {
        m_model->m_dayFormats[dayOfWeek] = format;
        cachedSizeHint = QSize();
        m_view->viewport()->update();
        m_view->updateGeometry();
    }

    QMap<QDate, QTextCharFormat> MStyleCalendar::dateTextFormat() const
    {
        return m_model->m_dateFormats;
    }

    QTextCharFormat MStyleCalendar::dateTextFormat(const QDate& date) const
    {
        return m_model->m_dateFormats.value(date);
    }

    void MStyleCalendar::setDateTextFormat(const QDate& date, const QTextCharFormat& format)
    {
        if (date.isNull()) {
            m_model->m_dateFormats.clear();
        } else {
            m_model->m_dateFormats[date] = format;
        }
        m_view->viewport()->update();
        m_view->updateGeometry();
    }

    bool MStyleCalendar::isDateEditEnabled() const
    {
        return m_dateEditEnabled;
    }

    void MStyleCalendar::setDateEditEnabled(bool enable)
    {
        if (isDateEditEnabled() == enable) {
            return;
        }

        m_dateEditEnabled = enable;

        setNavigatorEnabled(enable && (selectionMode() != MStyleCalendar::NoSelection));
    }

    int MStyleCalendar::dateEditAcceptDelay() const
    {
        //return m_navigator->dateEditAcceptDelay();
        return 0;
    }

    void MStyleCalendar::setDateEditAcceptDelay(int delay)
    {
        //m_navigator->setDateEditAcceptDelay(delay);
        Q_UNUSED(delay)
    }

    void MStyleCalendar::updateCell(const QDate& date)
    {
        if (Q_UNLIKELY(!date.isValid())) {
            qWarning("MStyleCalendar::updateCell: Invalid date");
            return;
        }

        if (!isVisible()) {
            return;
        }

        int row, column;
        m_model->cellForDate(date, &row, &column);
        if (row == -1 || column == -1) {
            return;
        }

        QModelIndex modelIndex = m_model->index(row, column);
        if (!modelIndex.isValid()) {
            return;
        }

        m_view->viewport()->update(m_view->visualRect(modelIndex));
    }

    void MStyleCalendar::updateCells()
    {
        if (isVisible()) {
            m_view->viewport()->update();
        }
    }

    bool MStyleCalendar::isNavigationBarVisible() const
    {
        return navBarVisible;
    }

    void MStyleCalendar::setNavigationBarVisible(bool visible)
    {
        navBarVisible = visible;
        cachedSizeHint = QSize();
        navBarBackground->setVisible(visible);
        updateGeometry();
    }

    bool MStyleCalendar::event(QEvent* event)
    {
        switch (event->type()) {
        case QEvent::LayoutDirectionChange:
            updateButtonIcons();
            break;
        case QEvent::LocaleChange:
            m_model->setFirstColumnDay(locale().firstDayOfWeek());
            cachedSizeHint = QSize();
            updateMonthMenuNames();
            updateNavigationBar();
            m_view->updateGeometry();
            // TODO: fix this known bug of calendaring API:
            // Changing locale before calendar works, but reverse order causes
            // invalid month names (in C Locale apparently).
            break;
        case QEvent::FontChange:
        case QEvent::ApplicationFontChange:
            cachedSizeHint = QSize();
            m_view->updateGeometry();
            break;
        case QEvent::StyleChange:
            cachedSizeHint = QSize();
            m_view->updateGeometry();
        default:
            break;
        }
        return QWidget::event(event);
    }

    bool MStyleCalendar::eventFilter(QObject* watched, QEvent* event)
    {
        if (event->type() == QEvent::MouseButtonPress && yearEdit->hasFocus()) {
            // We can get filtered press events that were intended for Qt Virtual Keyboard's
            // input panel (QQuickView), so we have to make sure that the window is indeed a QWidget - no static_cast.
            // In addition, as we have a event filter on the whole application we first make sure that the top level widget
            // of both this and the watched widget are the same to decide if we should finish the year edition.
            QWidget* tlw = window();
            QWidget* widget = qobject_cast<QWidget*>(watched);
            if (!widget || widget->window() != tlw) {
                return QWidget::eventFilter(watched, event);
            }

            QPoint mousePos = widget->mapTo(tlw, static_cast<QMouseEvent*>(event)->pos());
            QRect geom = QRect(yearEdit->mapTo(tlw, QPoint(0, 0)), yearEdit->size());
            if (!geom.contains(mousePos)) {
                event->accept();
                _yearEditingFinished();
                setFocus();
                return true;
            }
        }
        return QWidget::eventFilter(watched, event);
    }

    void MStyleCalendar::mousePressEvent(QMouseEvent* event)
    {
        setAttribute(Qt::WA_NoMouseReplay);
        QWidget::mousePressEvent(event);
        setFocus();
    }

    void MStyleCalendar::resizeEvent(QResizeEvent* event)
    {
        // XXX Should really use a QWidgetStack for yearEdit and yearButton,
        // XXX here we hide the year edit when the layout is likely to break
        // XXX the manual positioning of the yearEdit over the yearButton.
        if (yearEdit->isVisible() && event->size().width() != event->oldSize().width()) {
            _yearEditingFinished();
        }

        QWidget::resizeEvent(event);
    }

    void MStyleCalendar::keyPressEvent(QKeyEvent* event)
    {
#if QT_CONFIG(shortcut)
        if (yearEdit->isVisible() && event->matches(QKeySequence::Cancel)) {
            yearEdit->setValue(yearShown());
            _yearEditingFinished();
            return;
        }
#endif
        QWidget::keyPressEvent(event);
    }

    void MStyleCalendar::createNavigationBar(QWidget* widget)
    {
        navBarBackground = new QWidget(widget);
        navBarBackground->setObjectName(QLatin1String("mui_calendar_navibar"));
        navBarBackground->setAutoFillBackground(true);
        navBarBackground->setBackgroundRole(QPalette::Highlight);

        prevMonth = new QPrevNextCalButton(navBarBackground);
        nextMonth = new QPrevNextCalButton(navBarBackground);
        prevMonth->setAutoRaise(true);
        nextMonth->setAutoRaise(true);
        prevMonth->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        nextMonth->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        nextMonth->setAutoRaise(true);
        updateButtonIcons();
        prevMonth->setAutoRepeat(true);
        nextMonth->setAutoRepeat(true);

        monthButton = new MCalToolButton(navBarBackground);
        monthButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        monthButton->setAutoRaise(true);
        monthButton->setPopupMode(QToolButton::InstantPopup);
        monthMenu = new QMenu(monthButton);
        for (int i = 1, e = m_model->m_calendar.maximumMonthsInYear(); i <= e; i++) {
            QString monthName(m_model->monthName(locale(), i));
            QAction* act = monthMenu->addAction(monthName);
            act->setData(i);
            monthToAction[i] = act;
        }
        monthButton->setMenu(monthMenu);
        yearButton = new MCalToolButton(navBarBackground);
        yearButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        yearButton->setAutoRaise(true);
        yearEdit = new QSpinBox(navBarBackground);

        QFont font = __super::font();
        font.setBold(true);
        monthButton->setFont(font);
        yearButton->setFont(font);
        yearEdit->setFrame(false);
        yearEdit->setMinimum(m_model->m_minimumDate.year(m_model->m_calendar));
        yearEdit->setMaximum(m_model->m_maximumDate.year(m_model->m_calendar));
        yearEdit->hide();
        spaceHolder = new QSpacerItem(0, 0);

        QHBoxLayout* headerLayout = new QHBoxLayout;
        headerLayout->setContentsMargins(QMargins());
        headerLayout->setSpacing(0);
        headerLayout->addWidget(prevMonth);
        headerLayout->insertStretch(headerLayout->count());
        headerLayout->addWidget(monthButton);
        headerLayout->addItem(spaceHolder);
        headerLayout->addWidget(yearButton);
        headerLayout->insertStretch(headerLayout->count());
        headerLayout->addWidget(nextMonth);
        navBarBackground->setLayout(headerLayout);

        yearEdit->setFocusPolicy(Qt::StrongFocus);
        prevMonth->setFocusPolicy(Qt::NoFocus);
        nextMonth->setFocusPolicy(Qt::NoFocus);
        yearButton->setFocusPolicy(Qt::NoFocus);
        monthButton->setFocusPolicy(Qt::NoFocus);

        //set names for the header controls.
        prevMonth->setObjectName(QLatin1String("mui_calendar_prevmonth"));
        nextMonth->setObjectName(QLatin1String("mui_calendar_nextmonth"));
        monthButton->setObjectName(QLatin1String("mui_calendar_btn_month"));
        yearButton->setObjectName(QLatin1String("mui_calendar_btn_year"));
        yearEdit->setObjectName(QLatin1String("mui_calendar_edit_year"));

        updateMonthMenu();
        showMonth(m_model->m_date.year(m_model->m_calendar), m_model->m_date.month(m_model->m_calendar));
    }

    void MStyleCalendar::updateButtonIcons()
    {
        prevMonth->setIcon(style()->standardIcon(isRightToLeft() ? QStyle::SP_ArrowRight : QStyle::SP_ArrowLeft, nullptr,
                           this));
        nextMonth->setIcon(style()->standardIcon(isRightToLeft() ? QStyle::SP_ArrowLeft : QStyle::SP_ArrowRight, nullptr,
                           this));
    }

    void MStyleCalendar::updateMonthMenu()
    {
        int maxMonths = m_model->m_calendar.monthsInYear(m_model->m_shownYear);
        int beg = 1, end = maxMonths;
        bool prevEnabled = true;
        bool nextEnabled = true;
        QCalendar cal = m_model->calendar();
        if (m_model->m_shownYear == m_model->m_minimumDate.year(cal)) {
            beg = m_model->m_minimumDate.month(cal);
            if (m_model->m_shownMonth == m_model->m_minimumDate.month(cal)) {
                prevEnabled = false;
            }
        }
        if (m_model->m_shownYear == m_model->m_maximumDate.year(cal)) {
            end = m_model->m_maximumDate.month(cal);
            if (m_model->m_shownMonth == m_model->m_maximumDate.month(cal)) {
                nextEnabled = false;
            }
        }
        prevMonth->setEnabled(prevEnabled);
        nextMonth->setEnabled(nextEnabled);
        for (int i = 1; i <= maxMonths; i++) {
            bool monthEnabled = true;
            if (i < beg || i > end) {
                monthEnabled = false;
            }
            monthToAction[i]->setEnabled(monthEnabled);
        }
    }

    void MStyleCalendar::updateMonthMenuNames()
    {
        for (int i = 1; i <= 12; i++) {
            QString monthName(m_model->monthName(locale(), i));
            monthToAction[i]->setText(monthName);
        }
    }

    void MStyleCalendar::updateCurrentPage(const QDate& date)
    {
        const QCalendar cal = m_model->calendar();

        QDate newDate = date;
        const QDate minDate = minimumDate();
        const QDate maxDate = maximumDate();

        if (minDate.isValid() && minDate.daysTo(newDate) < 0) {
            newDate = minDate;
        }
        if (maxDate.isValid() && maxDate.daysTo(newDate) > 0) {
            newDate = maxDate;
        }

        showMonth(newDate.year(cal), newDate.month(cal));
        int row = -1, col = -1;
        m_model->cellForDate(newDate, &row, &col);
        if (row != -1 && col != -1) {
            m_view->selectionModel()->setCurrentIndex(m_model->index(row, col), QItemSelectionModel::NoUpdate);
        }
    }

    void MStyleCalendar::_monthChanged(QAction* act)
    {
        monthButton->setText(act->text());
        const QDate currentDate = getCurrentDate();
        const QDate newDate = currentDate.addMonths(act->data().toInt() - currentDate.month(m_model->m_calendar),
                              m_model->m_calendar);
        updateCurrentPage(newDate);
    }

    QDate MStyleCalendar::getCurrentDate()
    {
        const QModelIndex index = m_view->currentIndex();
        return m_model->dateForCell(index.row(), index.column());
    }

    void MStyleCalendar::_prevMonthClicked()
    {
        const QDate currentDate = getCurrentDate().addMonths(-1, m_model->m_calendar);
        updateCurrentPage(currentDate);
    }

    void MStyleCalendar::_nextMonthClicked()
    {
        const QDate currentDate = getCurrentDate().addMonths(1, m_model->m_calendar);
        updateCurrentPage(currentDate);
    }

    void MStyleCalendar::_yearEditingFinished()
    {
        yearButton->setText(locale().toString(yearEdit->value()));
        yearEdit->hide();

        setFocusPolicy(oldFocusPolicy);

        qApp->removeEventFilter(this);

        spaceHolder->changeSize(0, 0);
        yearButton->show();

        QDate currentDate = getCurrentDate();
        const int newYear = locale().toInt(yearEdit->text());

        currentDate = currentDate.addYears(newYear - currentDate.year(m_model->m_calendar), m_model->m_calendar);
        updateCurrentPage(currentDate);
    }

    void MStyleCalendar::_yearClicked()
    {
        //show the spinbox on top of the button
        yearEdit->setGeometry(yearButton->x(), yearButton->y(),
                              yearEdit->sizeHint().width(), yearButton->height());
        spaceHolder->changeSize(yearButton->width(), 0);
        yearButton->hide();
        oldFocusPolicy = focusPolicy();
        setFocusPolicy(Qt::NoFocus);
        yearEdit->show();
        qApp->installEventFilter(this);
        yearEdit->raise();
        yearEdit->selectAll();
        yearEdit->setFocus(Qt::MouseFocusReason);
    }

    void MStyleCalendar::showMonth(int year, int month)
    {
        if (m_model->m_shownYear == year && m_model->m_shownMonth == month) {
            return;
        }

        m_model->showMonth(year, month);

        updateNavigationBar();
        emit currentPageChanged(year, month);

        m_view->internalUpdate();
        cachedSizeHint = QSize();

        update();
        updateMonthMenu();
    }

    void MStyleCalendar::updateNavigationBar()
    {
        const QString monthName = m_model->monthName(locale(), m_model->m_shownMonth);

        monthButton->setText(monthName);
        yearEdit->setValue(m_model->m_shownYear);
        yearButton->setText(yearEdit->text());
    }

    void MStyleCalendar::update()
    {
        QDate currentDate = m_model->m_date;
        int row, column;
        m_model->cellForDate(currentDate, &row, &column);
        QModelIndex idx;
        m_selection->clear();
        if (row != -1 && column != -1) {
            idx = m_model->index(row, column);
            m_selection->setCurrentIndex(idx, QItemSelectionModel::SelectCurrent);
        }
    }

    void MStyleCalendar::_slotShowDate(const QDate& date)
    {
        updateCurrentPage(date);
    }

    void MStyleCalendar::_slotChangeDate(const QDate& date)
    {
        _slotChangeDateWithMonth(date, true);
    }

    void MStyleCalendar::_slotChangeDateWithMonth(const QDate& date, bool changeMonth)
    {
        QDate oldDate = m_model->m_date;
        m_model->setDate(date);
        QDate newDate = m_model->m_date;
        if (changeMonth) {
            showMonth(newDate.year(m_model->m_calendar), newDate.month(m_model->m_calendar));
        }
        if (oldDate != newDate) {
            update();

            //m_navigator->setDate(newDate);
            emit selectionChanged();
        }
    }

    void MStyleCalendar::_editingFinished()
    {
        emit activated(m_model->m_date);
    }

    void MCalToolButton::paintEvent(QPaintEvent* e)
    {
        Q_UNUSED(e)

        QStyleOptionToolButton opt;
        initStyleOption(&opt);

        if (opt.state & QStyle::State_MouseOver || isDown()) {
            //act as normal button
            setPalette(QPalette());
        } else {
            //set the highlight color for button text
            QPalette toolPalette = palette();
            toolPalette.setColor(QPalette::ButtonText, toolPalette.color(QPalette::HighlightedText));
            setPalette(toolPalette);
        }

        QToolButton::paintEvent(e);
    }

    void QPrevNextCalButton::paintEvent(QPaintEvent*)
    {
        QStylePainter painter(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        opt.state &= ~QStyle::State_HasFocus;
        painter.drawComplexControl(QStyle::CC_ToolButton, opt);
    }


}
