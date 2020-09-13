#include "MStyleNavbar.h"

#include <QLayout>
#include <QScrollBar>
#include <QButtonGroup>
#include <QPushButton>
#include <QPropertyAnimation>

namespace mui
{
    MStyleNavbar::MStyleNavbar(QWidget* parent)
        : QAbstractScrollArea(parent)
    {
        _init(Qt::Vertical);
    }

    MStyleNavbar::MStyleNavbar(Qt::Orientation ori, QWidget* parent)
        : QAbstractScrollArea(parent)
    {
        _init(ori);
    }

    MStyleNavbar::~MStyleNavbar()
    {

    }

    void MStyleNavbar::addItem(const QString& text, const QIcon& icon, int index)
    {
        QPushButton* btn = new QPushButton(icon, text, this);

        btn->setCheckable(true);
        btn->setObjectName(QStringLiteral(MUI_NAVITEM_OBJNAME));

#ifndef QT_NO_CURSOR
        btn->setCursor(Qt::PointingHandCursor);
#endif

#ifndef QT_NO_TOOLTIP
        btn->setToolTip(btn->text());
#endif

        mLayout->insertWidget(index, btn);
        mNavGroup->addButton(btn);
        _updateItemId();
    }

    void MStyleNavbar::removeItem(int pos)
    {
        auto& item = mNavGroup->buttons().at(pos);
        if (nullptr != item)
        {
            mNavGroup->buttons().removeAt(pos);
            item->deleteLater();

            _updateItemId();
        }
    }

    QPushButton* MStyleNavbar::getItem(uint index)
    {
        return qobject_cast<QPushButton*>(mNavGroup->button(index));
    }

    QPushButton* MStyleNavbar::getCurrentItem() const
    {
        return qobject_cast<QPushButton*>(mNavGroup->checkedButton());
    }

    Qt::Orientation MStyleNavbar::getOrientation() const
    {
        return (mLayout->direction() == QBoxLayout::TopToBottom
                || mLayout->direction() == QBoxLayout::BottomToTop)
               ? Qt::Vertical : Qt::Horizontal;
    }

    void MStyleNavbar::setTrackBarStyle(const SNavTrackBarStyle& trackBarStyle)
    {
        mTrackBarStyle = trackBarStyle;
        mTrackBar->setVisible(mTrackBarStyle.isEnabled);

        if (getOrientation() == Qt::Vertical)
        {
            const int x = ((mTrackBarStyle.offset >= 0) ? 0 : (width() - mTrackBar->width() + 1))
                          + mTrackBarStyle.offset;
            mTrackBar->move(x, 0);
        }
        else
        {
            const int y = ((mTrackBarStyle.offset < 0) ? 1 : (width() - mTrackBar->width()))
                          + mTrackBarStyle.offset;
            mTrackBar->move(0, y);
        }

        _adjustTrackBarPosition();
    }

    void MStyleNavbar::setSpacing(int spacing)
    {
        mLayout->setSpacing(spacing);
    }

    int MStyleNavbar::getSpacing() const
    {
        return mLayout->spacing();
    }

    void MStyleNavbar::setMargins(int margins)
    {
        mLayout->setMargin(margins);
    }

    void MStyleNavbar::setMargins(int left, int top, int right, int bottom)
    {
        mLayout->setContentsMargins(left, top, right, bottom);
    }

    QMargins MStyleNavbar::getMargins() const
    {
        return mLayout->contentsMargins();
    }

    void MStyleNavbar::navigate(int index)
    {
        const int len = mNavGroup->buttons().length();
        if (index >= 0 && index < len)
        {
            mNavGroup->button(index)->toggle();
            _adjustTrackBarPosition();
            emit navigated(qobject_cast<QPushButton*>(mNavGroup->button(index)));
        }
    }

    void MStyleNavbar::_updateItemId()
    {
        const int len = mNavGroup->buttons().length();
        for (int i = 0; i < len; ++i)
        {
            mNavGroup->setId(mNavGroup->buttons().at(i), i);
        }
        _adjustTrackBarPosition(true);
    }

    void MStyleNavbar::_adjustTrackBarPosition(bool isStopAnimation)
    {
        if (!mTrackBarStyle.isEnabled)
        {
            return;
        }

        if (mNavGroup->buttons().empty())
        {
            mTrackBar->setVisible(false);
        }
        else
        {
            const int dist = getCurrentItemDistance();
            if (dist < 0)
            {
                return;
            }

            mTrackBar->setVisible(true);

            QRect geo = mTrackBar->geometry();
            if (getOrientation() == Qt::Vertical)
            {
                geo.moveCenter(QPoint(geo.center().x(), dist));
            }
            else
            {
                geo.moveCenter(QPoint(dist, geo.center().y()));
            }

            if (mTrackBarStyle.isAnimation && !isStopAnimation)
            {
                QPropertyAnimation* ani = new QPropertyAnimation(mTrackBar, QByteArrayLiteral("geometry"), mTrackBar);
                ani->setEasingCurve(QEasingCurve::Type(mTrackBarStyle.animationType));
                ani->setDuration(mTrackBarStyle.duration);
                ani->setStartValue(mTrackBar->geometry());
                ani->setEndValue(geo);
                ani->start(QAbstractAnimation::DeleteWhenStopped);
            }
            else
            {
                mTrackBar->setGeometry(geo);
            }
        }
    }

    void MStyleNavbar::_init(Qt::Orientation ori)
    {
        viewport()->setBackgroundRole(QPalette::NoRole);
        horizontalScrollBar()->setSingleStep(10);
        verticalScrollBar()->setSingleStep(10);
        horizontalScrollBar()->setValue(0);
        verticalScrollBar()->setValue(0);
        setObjectName(QStringLiteral(MUI_NAVBAR_OBJNAME));

        mContent = new QWidget(this);
        mNavGroup = new QButtonGroup(this);
        mTrackBar = new QWidget(mContent);
        mTrackBar->setObjectName(QStringLiteral(MUI_NAV_TRACKBAR));

        // 布局
        if (ori == Qt::Vertical)
        {
            mLayout = new QBoxLayout(QBoxLayout::TopToBottom, mContent);
        }
        else
        {
            mLayout = new QBoxLayout(QBoxLayout::LeftToRight, mContent);
        }

        mLayout->setSpacing(10);
        mLayout->setMargin(10);

        mContent->installEventFilter(this);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15,0))
        connect(mNavGroup, &QButtonGroup::idClicked, this, &MStyleNavbar::navigate);
#else
        connect(mNavGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
                this, &MStyleNavbar::navigate);
#endif

        mTrackBar->lower();
        mContent->lower();
    }

    int MStyleNavbar::getCurrentItemDistance(bool isCenter)
    {
        const int curIdx = mNavGroup->checkedId();
        if (curIdx < 0)
        {
            return -1;
        }

        int dist = 0;

        if (getOrientation() == Qt::Vertical)
        {
            dist += mLayout->contentsMargins().top();

            for (int i = 0; i < curIdx; ++i)
            {
                dist += i * mLayout->spacing() + mNavGroup->button(i)->height();
            }

            if (isCenter)
            {
                dist += mNavGroup->button(curIdx)->height() / 2;
            }
        }
        else
        {
            dist += mLayout->contentsMargins().left();

            for (int i = 0; i < curIdx; ++i)
            {
                dist += i * mLayout->spacing() + mNavGroup->button(i)->width();
            }

            if (isCenter)
            {
                dist += mNavGroup->button(curIdx)->width() / 2;
            }
        }

        return dist;
    }
}
