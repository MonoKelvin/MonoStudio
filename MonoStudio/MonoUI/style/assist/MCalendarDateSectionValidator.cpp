#include "MCalendarDateSectionValidator.h"

#include <qdatetime.h>

namespace mui
{
    static QString formatNumber(int number, int fieldWidth)
    {
        return QString::number(number).rightJustified(fieldWidth, QLatin1Char('0'));
    }

    QString MCalendarDateSectionValidator::highlightString(const QString& str, int pos)
    {
        if (pos == 0) {
            return QLatin1String("<b>") + str + QLatin1String("</b>");
        }

        const int startPos = str.length() - pos;
        return str.midRef(0, startPos) + QLatin1String("<b>") + str.midRef(startPos, pos) + QLatin1String("</b>");
    }

    //////////////////////////////////

    MCalendarDayValidator::MCalendarDayValidator()
        : MCalendarDateSectionValidator()
        , m_pos(0), m_day(1), m_oldDay(1)
    {
    }

    MCalendarDateSectionValidator::Section MCalendarDayValidator::handleKey(int key)
    {
        if (key == Qt::Key_Right || key == Qt::Key_Left) {
            m_pos = 0;
            return MCalendarDateSectionValidator::ThisSection;
        } else if (key == Qt::Key_Up) {
            m_pos = 0;
            ++m_day;
            if (m_day > 31) {
                m_day = 1;
            }
            return MCalendarDateSectionValidator::ThisSection;
        } else if (key == Qt::Key_Down) {
            m_pos = 0;
            --m_day;
            if (m_day < 1) {
                m_day = 31;
            }
            return MCalendarDateSectionValidator::ThisSection;
        } else if (key == Qt::Key_Back || key == Qt::Key_Backspace) {
            --m_pos;
            if (m_pos < 0) {
                m_pos = 1;
            }

            if (m_pos == 0) {
                m_day = m_oldDay;
            } else {
                m_day = m_day / 10;
            }
            //m_day = m_oldDay / 10 * 10 + m_day / 10;

            if (m_pos == 0) {
                return MCalendarDateSectionValidator::PrevSection;
            }
            return MCalendarDateSectionValidator::ThisSection;
        }
        if (key < Qt::Key_0 || key > Qt::Key_9) {
            return MCalendarDateSectionValidator::ThisSection;
        }
        int pressedKey = key - Qt::Key_0;
        if (m_pos == 0) {
            m_day = pressedKey;
        } else {
            m_day = m_day % 10 * 10 + pressedKey;
        }
        if (m_day > 31) {
            m_day = 31;
        }
        ++m_pos;
        if (m_pos > 1) {
            m_pos = 0;
            return MCalendarDateSectionValidator::NextSection;
        }
        return MCalendarDateSectionValidator::ThisSection;
    }

    QDate MCalendarDayValidator::applyToDate(QDate date, QCalendar cal) const
    {
        auto parts = cal.partsFromDate(date);
        if (!parts.isValid()) {
            return QDate();
        }
        parts.day = qMin(qMax(1, m_day), cal.daysInMonth(parts.year, parts.month));
        return cal.dateFromParts(parts);
    }

    void MCalendarDayValidator::setDate(QDate date, QCalendar cal)
    {
        m_day = m_oldDay = date.day(cal);
        m_pos = 0;
    }

    QString MCalendarDayValidator::text() const
    {
        return highlightString(formatNumber(m_day, 2), m_pos);
    }

    QString MCalendarDayValidator::text(QDate date, QCalendar cal, int repeat) const
    {
        if (repeat <= 1) {
            return QString::number(date.day(cal));
        } else if (repeat == 2) {
            return formatNumber(date.day(cal), 2);
        } else if (repeat == 3) {
            return m_locale.dayName(date.dayOfWeek(cal), QLocale::ShortFormat);
        } else { /* repeat >= 4 */
            return m_locale.dayName(date.dayOfWeek(cal), QLocale::LongFormat);
        }
    }

    //////////////////////////////////

    MCalendarMonthValidator::MCalendarMonthValidator()
        : MCalendarDateSectionValidator(), m_pos(0), m_month(1), m_oldMonth(1)
    {
    }

    MCalendarDateSectionValidator::Section MCalendarMonthValidator::handleKey(int key)
    {
        if (key == Qt::Key_Right || key == Qt::Key_Left) {
            m_pos = 0;
            return MCalendarDateSectionValidator::ThisSection;
        } else if (key == Qt::Key_Up) {
            m_pos = 0;
            ++m_month;
            if (m_month > 12) {
                m_month = 1;
            }
            return MCalendarDateSectionValidator::ThisSection;
        } else if (key == Qt::Key_Down) {
            m_pos = 0;
            --m_month;
            if (m_month < 1) {
                m_month = 12;
            }
            return MCalendarDateSectionValidator::ThisSection;
        } else if (key == Qt::Key_Back || key == Qt::Key_Backspace) {
            --m_pos;
            if (m_pos < 0) {
                m_pos = 1;
            }

            if (m_pos == 0) {
                m_month = m_oldMonth;
            } else {
                m_month = m_month / 10;
            }
            //m_month = m_oldMonth / 10 * 10 + m_month / 10;

            if (m_pos == 0) {
                return MCalendarDateSectionValidator::PrevSection;
            }
            return MCalendarDateSectionValidator::ThisSection;
        }
        if (key < Qt::Key_0 || key > Qt::Key_9) {
            return MCalendarDateSectionValidator::ThisSection;
        }
        int pressedKey = key - Qt::Key_0;
        if (m_pos == 0) {
            m_month = pressedKey;
        } else {
            m_month = m_month % 10 * 10 + pressedKey;
        }
        if (m_month > 12) {
            m_month = 12;
        }
        ++m_pos;
        if (m_pos > 1) {
            m_pos = 0;
            return MCalendarDateSectionValidator::NextSection;
        }
        return MCalendarDateSectionValidator::ThisSection;
    }

    QDate MCalendarMonthValidator::applyToDate(QDate date, QCalendar cal) const
    {
        auto parts = cal.partsFromDate(date);
        if (!parts.isValid()) {
            return QDate();
        }
        parts.month = qMin(qMax(1, m_month), cal.monthsInYear(parts.year));
        parts.day = qMin(parts.day, cal.daysInMonth(parts.year, m_month)); // m_month or parts.month ?
        return cal.dateFromParts(parts);
    }

    void MCalendarMonthValidator::setDate(QDate date, QCalendar cal)
    {
        m_month = m_oldMonth = date.month(cal);
        m_pos = 0;
    }

    QString MCalendarMonthValidator::text() const
    {
        return highlightString(formatNumber(m_month, 2), m_pos);
    }

    QString MCalendarMonthValidator::text(QDate date, QCalendar cal, int repeat) const
    {
        const auto parts = cal.partsFromDate(date);
        // Numeric forms:
        if (repeat <= 1) {
            return QString::number(parts.month);
        }
        if (repeat == 2) {
            return formatNumber(parts.month, 2);
        }
        // Text forms:
        if (repeat == 3) {
            return cal.standaloneMonthName(m_locale, parts.month, parts.year, QLocale::ShortFormat);
        }
        /* repeat >= 4 */
        return cal.standaloneMonthName(m_locale, parts.month, parts.year, QLocale::LongFormat);
    }

    //////////////////////////////////

    MCalendarYearValidator::MCalendarYearValidator()
        : MCalendarDateSectionValidator()
        , m_pos(0), m_year(2000), m_oldYear(2000)
    {
        // TODO: What to use (for non-Gregorian calendars) as default year?
        // Maybe 1360 for Jalali, 1420 for Islamic, etc.
    }

    int MCalendarYearValidator::pow10(int n)
    {
        int power = 1;
        for (int i = 0; i < n; i++) {
            power *= 10;
        }

        return power;
    }

    MCalendarDateSectionValidator::Section MCalendarYearValidator::handleKey(int key)
    {
        if (key == Qt::Key_Right || key == Qt::Key_Left) {
            m_pos = 0;
            return MCalendarDateSectionValidator::ThisSection;
        } else if (key == Qt::Key_Up) {
            m_pos = 0;
            ++m_year;
            return MCalendarDateSectionValidator::ThisSection;
        } else if (key == Qt::Key_Down) {
            m_pos = 0;
            --m_year;
            return MCalendarDateSectionValidator::ThisSection;
        } else if (key == Qt::Key_Back || key == Qt::Key_Backspace) {
            --m_pos;
            if (m_pos < 0) {
                m_pos = 3;
            }

            const int pow = pow10(m_pos);
            m_year = m_oldYear / pow * pow + m_year % (pow * 10) / 10;

            if (m_pos == 0) {
                return MCalendarDateSectionValidator::PrevSection;
            }
            return MCalendarDateSectionValidator::ThisSection;
        }

        if (key < Qt::Key_0 || key > Qt::Key_9) {
            return MCalendarDateSectionValidator::ThisSection;
        }

        int pressedKey = key - Qt::Key_0;
        int pow = pow10(m_pos);

        m_year = m_year / (pow * 10) * (pow * 10) + m_year % pow * 10 + pressedKey;
        ++m_pos;

        if (m_pos > 3) {
            m_pos = 0;
            return MCalendarDateSectionValidator::NextSection;
        }

        return MCalendarDateSectionValidator::ThisSection;
    }

    QDate MCalendarYearValidator::applyToDate(QDate date, QCalendar cal) const
    {
        auto parts = cal.partsFromDate(date);
        if (!parts.isValid()) {
            return QDate();
        }

        // This widget does not support negative years (some calendars may support)
        parts.year = qMax(1, m_year);
        parts.day = qMin(parts.day, cal.daysInMonth(parts.year, parts.month));
        return cal.dateFromParts(parts);
    }

    void MCalendarYearValidator::setDate(QDate date, QCalendar cal)
    {
        m_year = m_oldYear = date.year(cal);
        m_pos = 0;
    }

    QString MCalendarYearValidator::text() const
    {
        return highlightString(formatNumber(m_year, 4), m_pos);
    }

    QString MCalendarYearValidator::text(QDate date, QCalendar cal, int repeat) const
    {
        if (repeat < 4) {
            return formatNumber(date.year(cal) % 100, 2);
        }

        return QString::number(date.year(cal));
    }
}
