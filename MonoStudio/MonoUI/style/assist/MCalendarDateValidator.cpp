#include "MCalendarDateValidator.h"

#include <qevent.h>

namespace mui
{
    MCalendarDateValidator::MCalendarDateValidator()
        : m_currentToken(-1),
          m_initialDate(QDate::currentDate()),
          m_currentDate(m_initialDate),
          m_lastSectionMove(MCalendarDateSectionValidator::ThisSection)
    {
    }

    void MCalendarDateValidator::setLocale(const QLocale& locale)
    {
        m_yearValidator.m_locale = locale;
        m_monthValidator.m_locale = locale;
        m_dayValidator.m_locale = locale;
    }

    MCalendarDateValidator::~MCalendarDateValidator()
    {
        clear();
    }

    // from qdatetime.cpp
    int MCalendarDateValidator::countRepeat(const QString& str, int index) const
    {
        Q_ASSERT(index >= 0 && index < str.size());
        int count = 1;
        const QChar ch = str.at(index);
        while (index + count < str.size() && str.at(index + count) == ch) {
            ++count;
        }
        return count;
    }

    void MCalendarDateValidator::setInitialDate(QDate date, QCalendar cal)
    {
        m_yearValidator.setDate(date, cal);
        m_monthValidator.setDate(date, cal);
        m_dayValidator.setDate(date, cal);
        m_initialDate = date;
        m_currentDate = date;
        m_lastSectionMove = MCalendarDateSectionValidator::ThisSection;
    }

    QString MCalendarDateValidator::currentText(QCalendar cal) const
    {
        QString str;
        const int numSeps = m_separators.size();
        const int numTokens = int(m_tokens.size());
        for (int i = 0; i < numSeps; ++i) {
            str += m_separators.at(i);
            if (i < numTokens) {
                const SectionToken& token = m_tokens[i];
                if (i == m_currentToken) {
                    str += token.validator->text();
                } else {
                    str += token.validator->text(m_currentDate, cal, token.repeat);
                }
            }
        }
        return str;
    }

    void MCalendarDateValidator::clear()
    {
        m_tokens.clear();
        m_separators.clear();

        m_currentToken = -1;
    }

    void MCalendarDateValidator::setFormat(const QString& format)
    {
        clear();

        int pos = 0;
        const QLatin1Char quote('\'');
        bool quoting = false;
        QString separator;
        while (pos < format.size()) {
            const QStringRef mid = format.midRef(pos);
            int offset = 1;

            if (mid.startsWith(quote)) {
                quoting = !quoting;
            } else {
                const QChar nextChar = format.at(pos);
                if (quoting) {
                    separator += nextChar;
                    quoting = false;
                } else {
                    MCalendarDateSectionValidator* validator = nullptr;
                    if (nextChar == QLatin1Char('d')) {
                        offset = qMin(4, countRepeat(format, pos));
                        validator = &m_dayValidator;
                    } else if (nextChar == QLatin1Char('M')) {
                        offset = qMin(4, countRepeat(format, pos));
                        validator = &m_monthValidator;
                    } else if (nextChar == QLatin1Char('y')) {
                        offset = qMin(4, countRepeat(format, pos));
                        validator = &m_yearValidator;
                    } else {
                        separator += nextChar;
                    }
                    if (validator) {
                        m_tokens.push_back(SectionToken(validator, offset));
                        m_separators.append(separator);
                        separator = QString();
                        if (m_currentToken < 0) {
                            m_currentToken = int(m_tokens.size()) - 1;
                        }

                    }
                }
            }
            pos += offset;
        }
        m_separators += separator;
    }

    void MCalendarDateValidator::applyToDate(QCalendar cal)
    {
        m_currentDate = m_yearValidator.applyToDate(m_currentDate, cal);
        m_currentDate = m_monthValidator.applyToDate(m_currentDate, cal);
        m_currentDate = m_dayValidator.applyToDate(m_currentDate, cal);
    }

    void MCalendarDateValidator::toNextToken()
    {
        if (m_currentToken < 0) {
            return;
        }
        ++m_currentToken;
        m_currentToken %= m_tokens.size();
    }

    void MCalendarDateValidator::toPreviousToken()
    {
        if (m_currentToken < 0) {
            return;
        }
        --m_currentToken;
        m_currentToken %= m_tokens.size();
    }

    void MCalendarDateValidator::handleKeyEvent(QKeyEvent* keyEvent, QCalendar cal)
    {
        if (m_currentToken < 0) {
            return;
        }

        int key = keyEvent->key();
        if (m_lastSectionMove == MCalendarDateSectionValidator::NextSection) {
            if (key == Qt::Key_Back || key == Qt::Key_Backspace) {
                toPreviousToken();
            }
        }
        if (key == Qt::Key_Right) {
            toNextToken();
        } else if (key == Qt::Key_Left) {
            toPreviousToken();
        }

        m_lastSectionMove = m_tokens[m_currentToken].validator->handleKey(key);

        applyToDate(cal);
        if (m_lastSectionMove == MCalendarDateSectionValidator::NextSection) {
            toNextToken();
        } else if (m_lastSectionMove == MCalendarDateSectionValidator::PrevSection) {
            toPreviousToken();
        }
    }

}
