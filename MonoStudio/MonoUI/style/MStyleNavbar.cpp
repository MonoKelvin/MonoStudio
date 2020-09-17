#include "MStyleNavbar.h"

#include <QLayout>
#include <QScrollBar>
#include <QButtonGroup>
#include <QEvent>
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
        const bool isBtnsEmpty = mNavGroup->buttons().empty();

        QPushButton* btn = new QPushButton(icon, text, this);

        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
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

        if (isBtnsEmpty)
        {
            btn->setChecked(true);
        }

        _updateItemId();
    }

    void MStyleNavbar::setItemSize(int w, int h, int index)
    {
        const int len = mNavGroup->buttons().size();
        if (index < 0 || index >= len)
        {
            if (getOrientation() == Qt::Vertical)
            {
                for (auto& btn : mNavGroup->buttons())
                {
                    btn->setFixedHeight(h);
                }
            }
            else
            {
                for (auto& btn : mNavGroup->buttons())
                {
                    btn->setFixedWidth(w);
                }
            }
        }
        else
        {
            QAbstractButton* btn = mNavGroup->button(index);
            if (getOrientation() == Qt::Vertical)
            {
                btn->setFixedHeight(h);
            }
            else
            {
                btn->setFixedWidth(w);
            }
        }
    }

    void MStyleNavbar::setItemEnabled(int index, bool isEnabled)
    {
        QAbstractButton* btn = mNavGroup->button(index);
        if (nullptr != btn)
        {
            btn->setEnabled(isEnabled);
        }
    }

    void MStyleNavbar::removeItem(int index)
    {
        auto& item = mNavGroup->buttons().at(index);
        if (nullptr != item)
        {
            mNavGroup->buttons().removeAt(index);
            item->deleteLater();

            _updateItemId();
        }
    }

    QPushButton* MStyleNavbar::getItem(int index)
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

        _updateTrackBarPosition();
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
            _updateTrackBarPosition();
            emit navigated(qobject_cast<QPushButton*>(mNavGroup->button(index)));
        }
    }

    bool MStyleNavbar::event(QEvent* e)
    {
        if (e->type() == QEvent::StyleChange || e->type() == QEvent::LayoutRequest)
        {
            _updateScrollBars();
        }

        return QAbstractScrollArea::event(e);
    }

    bool MStyleNavbar::eventFilter(QObject* o, QEvent* e)
    {
        if (o == mContent && e->type() == QEvent::Resize)
        {
            _updateScrollBars();
        }

        return QAbstractScrollArea::eventFilter(o, e);
    }

    QSize MStyleNavbar::viewportSizeHint() const
    {
        return mContent->sizeHint();
    }

    void MStyleNavbar::scrollContentsBy(int, int)
    {
        _updateContentPosition();
    }

    int MStyleNavbar::getItemDistance(int index, bool isCenter)
    {
        if (index < 0)
        {
            index = mNavGroup->checkedId();
            if (index < 0)
            {
                return -1;
            }
        }

        int dist = index * mLayout->spacing();

        if (getOrientation() == Qt::Vertical)
        {
            dist += mLayout->contentsMargins().top();

            for (int i = 0; i < index; ++i)
            {
                dist += mNavGroup->button(i)->height();
            }

            if (isCenter)
            {
                dist += mNavGroup->button(index)->height() / 2;
            }
        }
        else
        {
            dist += mLayout->contentsMargins().left();

            for (int i = 0; i < index; ++i)
            {
                dist += mNavGroup->button(i)->width();
            }

            if (isCenter)
            {
                dist += mNavGroup->button(index)->width() / 2;
            }
        }

        return dist;
    }

    int MStyleNavbar::getItemOffset(int index, bool isCenter)
    {
        if (getOrientation() == Qt::Vertical)
        {
            return getItemDistance(index, isCenter) + mContent->y();
        }
        else
        {
            return getItemDistance(index, isCenter) + mContent->x();
        }
    }

    void MStyleNavbar::_updateItemId()
    {
        const int len = mNavGroup->buttons().length();
        for (int i = 0; i < len; ++i)
        {
            mNavGroup->setId(mNavGroup->buttons().at(i), i);
        }
        _updateTrackBarPosition(true);
    }

    void MStyleNavbar::_updateTrackBarPosition(bool isStopAnimation)
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
            const int dist = getItemDistance();
            if (dist < 0)
            {
                mTrackBar->setVisible(false);
                return;
            }

            mTrackBar->setVisible(true);

            QRect geo = mTrackBar->geometry();
            if (getOrientation() == Qt::Vertical)
            {
                if (mTrackBarStyle.isFitItem)
                {
                    geo.setHeight(getCurrentItem()->height());
                }

                geo.moveCenter(QPoint(geo.center().x(), dist));
            }
            else
            {
                if (mTrackBarStyle.isFitItem)
                {
                    geo.setWidth(getCurrentItem()->width());
                }
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

    int MStyleNavbar::_inverseTrack(QAbstractButton* btn)
    {
        if (nullptr == btn)
        {
            mTrackBar->setVisible(false);
            return -1;
        }

        const int dist = getItemOffset();

        mTrackBar->setVisible(true);

        QRect geo = mTrackBar->geometry();
        if (getOrientation() == Qt::Vertical)
        {
            if (mTrackBarStyle.isFitItem)
            {
                geo.setHeight(getCurrentItem()->height());
            }

            geo.moveCenter(QPoint(geo.center().x(), dist));
        }
        else
        {
            if (mTrackBarStyle.isFitItem)
            {
                geo.setWidth(getCurrentItem()->width());
            }
            geo.moveCenter(QPoint(dist, geo.center().y()));
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
            setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
            mLayout = new QBoxLayout(QBoxLayout::TopToBottom, mContent);
        }
        else
        {
            setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
            mLayout = new QBoxLayout(QBoxLayout::LeftToRight, mContent);
        }

        mLayout->setSpacing(10);
        mLayout->setMargin(10);
        mLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

        mContent->installEventFilter(this);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15,0))
        connect(mNavGroup, &QButtonGroup::idClicked, this, &MStyleNavbar::navigate);
#else
        connect(mNavGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
                this, &MStyleNavbar::navigate);
#endif

        mContent->raise();
        mTrackBar->raise();
        mTrackBar->setVisible(false);
        setTrackBarStyle({});
    }

    void MStyleNavbar::_updateScrollBars(void)
    {
        QSize p = viewport()->size();
        //const auto& m = maximumViewportSize();
        const auto& min = mContent->minimumSize();
        const auto& max = mContent->maximumSize();

        /*if (mLayout->hasHeightForWidth())
        {
            const QSize p_hfw = p.expandedTo(min).boundedTo(max);
            const int h = mContent->heightForWidth(p_hfw.width());
            min = QSize(p_hfw.width(), qMax(p_hfw.height(), h));
        }

        if (m.expandedTo(min) == m && m.boundedTo(max) == m)
        {
            p = m;    // no scroll bars needed
        }*/

        const auto& v = p.expandedTo(min).boundedTo(max);
        if (getOrientation() == Qt::Vertical)
        {
            mContent->resize(width(), mContent->height());
        }
        else
        {
            mContent->resize(mContent->width(), height());
        }

        horizontalScrollBar()->setRange(0, v.width() - p.width());
        horizontalScrollBar()->setPageStep(p.width());
        verticalScrollBar()->setRange(0, v.height() - p.height());
        verticalScrollBar()->setPageStep(p.height());

        _updateContentPosition();
    }

    void MStyleNavbar::_updateContentPosition()
    {
        const auto& hbar = horizontalScrollBar();
        const auto& vbar = verticalScrollBar();

        const Qt::LayoutDirection dir = layoutDirection();
        const QRect scrolled = QStyle::visualRect(dir, viewport()->rect(), QRect(QPoint(-hbar->value(), -vbar->value()),
                               mContent->size()));
        const QRect aligned = QStyle::alignedRect(dir, Qt::AlignLeft | Qt::AlignTop, mContent->size(), viewport()->rect());
        mContent->move(mContent->width() < viewport()->width() ? aligned.x() : scrolled.x(),
                       mContent->height() < viewport()->height() ? aligned.y() : scrolled.y());
    }

}
