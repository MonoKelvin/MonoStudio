#include "MStyleNavbar.h"

#include <QLayout>
#include <QScrollBar>
#include <QButtonGroup>
#include <QEvent>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QWheelEvent>

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

    int MStyleNavbar::getHScrollValue() const noexcept
    {
        return horizontalScrollBar()->value();
    }

    void MStyleNavbar::setHScrollValue(int value)
    {
        horizontalScrollBar()->setValue(value);
        _updateContentPosition();
    }

    int MStyleNavbar::getVScrollValue() const noexcept
    {
        return verticalScrollBar()->value();
    }

    void MStyleNavbar::setVScrollValue(int value)
    {
        verticalScrollBar()->setValue(value);
        _updateContentPosition();
    }

    void MStyleNavbar::addItem(const QString& text, const QIcon& icon, int index)
    {
        QPushButton* btn = new QPushButton(icon, text, this);

        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        btn->setCheckable(true);
        btn->setObjectName(QStringLiteral(MUI_NAVBAR_ITEM));

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

    int MStyleNavbar::getItemCount() const noexcept
    {
        return mNavGroup->buttons().size();
    }

    void MStyleNavbar::setItemSize(int w, int h, int index)
    {
        const int len = mNavGroup->buttons().size();
        if (index < 0 || index >= len) {
            if (getOrientation() == Qt::Vertical) {
                for (auto& btn : mNavGroup->buttons()) {
                    btn->setFixedHeight(h);
                }
            } else {
                for (auto& btn : mNavGroup->buttons()) {
                    btn->setFixedWidth(w);
                }
            }
        } else {
            QAbstractButton* btn = mNavGroup->button(index);
            if (getOrientation() == Qt::Vertical) {
                btn->setFixedHeight(h);
            } else {
                btn->setFixedWidth(w);
            }
        }
    }

    void MStyleNavbar::setItemEnabled(int index, bool isEnabled)
    {
        QAbstractButton* btn = mNavGroup->button(index);
        if (nullptr != btn) {
            btn->setEnabled(isEnabled);
        }
    }

    void MStyleNavbar::removeItem(int index)
    {
        auto& item = mNavGroup->buttons().at(index);
        if (nullptr != item) {
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
        return (mLayout->direction() == QBoxLayout::TopToBottom ||
                mLayout->direction() == QBoxLayout::BottomToTop)
               ? Qt::Vertical
               : Qt::Horizontal;
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
        if (index >= 0 && index < len && index != mPrevIndex) {
            mNavGroup->button(index)->toggle();
            updateTrackBarStyle(false);
            mPrevIndex = index;
            emit navigated(index);
        }
    }

    bool MStyleNavbar::event(QEvent* e)
    {
        if (e->type() == QEvent::StyleChange || e->type() == QEvent::LayoutRequest) {
            _updateScrollBars();
        }

        return QAbstractScrollArea::event(e);
    }

    bool MStyleNavbar::eventFilter(QObject* o, QEvent* e)
    {
        if (o == mContent) {
            switch (e->type()) {
            case QEvent::Resize:
                _updateScrollBars();
                break;
            case QEvent::Wheel: {
                const auto& wheelEvt = static_cast<QWheelEvent*>(e);
                const auto scrollValue = wheelEvt->angleDelta() / 6;
                if (getOrientation() == Qt::Vertical) {
                    auto vbar = verticalScrollBar();
                    vbar->setValue(vbar->value() - scrollValue.y());
                } else {
                    auto hbar = horizontalScrollBar();
                    hbar->setValue(hbar->value() - scrollValue.y());
                }
            }
            break;
            default:
                break;
            }
        }

        return QAbstractScrollArea::eventFilter(o, e);
    }

    void MStyleNavbar::resizeEvent(QResizeEvent* event)
    {
        _updateScrollBars();

        return QAbstractScrollArea::resizeEvent(event);
    }

    void MStyleNavbar::scrollContentsBy(int, int)
    {
        _updateContentPosition();
    }

    void MStyleNavbar::showEvent(QShowEvent* e)
    {
        int index = mNavGroup->checkedId();
        index = index == -1 ? 0 : index;

        const int len = mNavGroup->buttons().length();
        if (index >= 0 && index < len && index != mPrevIndex) {
            mNavGroup->button(index)->toggle();
            updateTrackBarStyle();
            mPrevIndex = index;
            emit navigated(index);
        }

        return QAbstractScrollArea::showEvent(e);
    }

    int MStyleNavbar::getItemDistance(int index, bool isCenter)
    {
        if (index < 0) {
            index = mNavGroup->checkedId();
            if (index < 0) {
                return -1;
            }
        }

        int dist = index * mLayout->spacing();

        if (getOrientation() == Qt::Vertical) {
            dist += mLayout->contentsMargins().top();

            for (int i = 0; i < index; ++i) {
                dist += mNavGroup->button(i)->height();
            }

            if (isCenter) {
                dist += mNavGroup->button(index)->height() / 2;
            }
        } else {
            dist += mLayout->contentsMargins().left();

            for (int i = 0; i < index; ++i) {
                dist += mNavGroup->button(i)->width();
            }

            if (isCenter) {
                dist += mNavGroup->button(index)->width() / 2;
            }
        }

        return dist;
    }

    int MStyleNavbar::getItemOffset(int index, bool isCenter)
    {
        if (getOrientation() == Qt::Vertical) {
            return getItemDistance(index, isCenter) + mContent->y();
        } else {
            return getItemDistance(index, isCenter) + mContent->x();
        }
    }

    void MStyleNavbar::setBoundaryOffset(int value)
    {
        if (!mNavGroup->buttons().isEmpty()) {
            const int count = mNavGroup->buttons().size();
            int minValue = 0;
            if (getOrientation() == Qt::Vertical) {
                minValue = qMin(mNavGroup->button(0)->height(), mNavGroup->button(count - 1)->height());
            } else {
                minValue = qMin(mNavGroup->button(0)->width(), mNavGroup->button(count - 1)->width());
            }
            mBoundaryOffset = qMin(minValue, value);
        }
    }

    void MStyleNavbar::scroll(int h, int v)
    {
        QPropertyAnimation* hAni = nullptr;
        if (h != 0) {
            const int hCurValue = horizontalScrollBar()->value();
            hAni = new QPropertyAnimation(this, QByteArrayLiteral("hScrollValue"));
            hAni->setEasingCurve(QEasingCurve::Type(mTrackBarStyle.animationType));
            hAni->setDuration(mTrackBarStyle.duration);
            hAni->setStartValue(hCurValue);
            hAni->setEndValue(qMax(0, qMin(hCurValue - h, horizontalScrollBar()->maximum())));
        }

        QPropertyAnimation* vAni = nullptr;
        if (v != 0) {
            const int vCurValue = verticalScrollBar()->value();
            vAni = new QPropertyAnimation(this, QByteArrayLiteral("vScrollValue"));
            vAni->setEasingCurve(QEasingCurve::Type(mTrackBarStyle.animationType));
            vAni->setDuration(mTrackBarStyle.duration);
            vAni->setStartValue(vCurValue);
            vAni->setEndValue(qMax(0, qMin(vCurValue - v, verticalScrollBar()->maximum())));
        }

        if (nullptr != vAni && nullptr != hAni) {
            QParallelAnimationGroup* group = new QParallelAnimationGroup;
            group->addAnimation(hAni);
            group->addAnimation(vAni);
            group->start(QAbstractAnimation::DeleteWhenStopped);
        } else if (nullptr != vAni) {
            vAni->start(QAbstractAnimation::DeleteWhenStopped);
        } else if (nullptr != hAni) {
            hAni->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }

    void MStyleNavbar::_init(Qt::Orientation ori)
    {
        viewport()->setBackgroundRole(QPalette::NoRole);
        horizontalScrollBar()->setSingleStep(15);
        verticalScrollBar()->setSingleStep(15);
        horizontalScrollBar()->setValue(0);
        verticalScrollBar()->setValue(0);
        setObjectName(QStringLiteral(MUI_NAVBAR));

        mBoundaryOffset = 20;
        mPrevIndex = -1;
        mContent = new QWidget(this);
        mNavGroup = new QButtonGroup(this);
        mTrackBar = new QWidget(mContent);
        mTrackBar->resize(4, 4);
        mTrackBar->setVisible(false);
        mTrackBar->setObjectName(QStringLiteral(MUI_NAVBAR_TRACKBAR));

        // 布局
        if (ori == Qt::Vertical) {
            setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
            mLayout = new QBoxLayout(QBoxLayout::TopToBottom, mContent);
        } else {
            setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
            mLayout = new QBoxLayout(QBoxLayout::LeftToRight, mContent);
        }

        mLayout->setSpacing(10);
        mLayout->setMargin(10);
        mLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

        mContent->installEventFilter(this);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
        connect(mNavGroup, &QButtonGroup::idClicked, this, &MStyleNavbar::navigate);
#else
        connect(mNavGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
                this, &MStyleNavbar::navigate);
#endif

        mContent->raise();
        mTrackBar->raise();
    }

    void MStyleNavbar::_updateItemId()
    {
        const int len = mNavGroup->buttons().length();
        for (int i = 0; i < len; ++i) {
            mNavGroup->setId(mNavGroup->buttons().at(i), i);
        }
        updateTrackBarStyle();
    }

    void MStyleNavbar::updateTrackBarStyle(bool stopAnimation)
    {
        mTrackBar->setVisible(mTrackBarStyle.isEnabled);

        if (!mTrackBarStyle.isEnabled) {
            return;
        }

        if (mNavGroup->checkedId() < 0) {
            mTrackBar->setVisible(false);
        } else {
            const int dist = _updateItemPosition(mNavGroup->checkedId());
            QRect geo = mTrackBar->geometry();
            int x = 0, y = 0;

            if (getOrientation() == Qt::Vertical) {
                if (mTrackBarStyle.isFitItem) {
                    geo.setHeight(getCurrentItem()->height());
                }
                x = ((mTrackBarStyle.offset >= 0) ? 0 : (mContent->width() - mTrackBar->width() + 1))
                    + mTrackBarStyle.offset;
                y = dist - mContent->y() - geo.height() / 2;

            } else {
                if (mTrackBarStyle.isFitItem) {
                    geo.setWidth(getCurrentItem()->width());
                }
                x = dist - mContent->x() - geo.width() / 2;
                y = ((mTrackBarStyle.offset < 0) ? 1 : (mContent->height() - mTrackBar->height()))
                    + mTrackBarStyle.offset;
            }
            geo.moveTopLeft(QPoint(x, y));

            if (!stopAnimation && mTrackBarStyle.isAnimation) {
                if (geo == mTrackBar->geometry()) {
                    return;
                }

                QPropertyAnimation* ani = new QPropertyAnimation(mTrackBar, QByteArrayLiteral("geometry"));
                ani->setEasingCurve(QEasingCurve::Type(mTrackBarStyle.animationType));
                ani->setDuration(mTrackBarStyle.duration);
                ani->setStartValue(mTrackBar->geometry());
                ani->setEndValue(geo);
                ani->start(QAbstractAnimation::DeleteWhenStopped);
            } else {
                mTrackBar->setGeometry(geo);
            }
        }
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
        if (getOrientation() == Qt::Vertical) {
            mContent->resize(viewport()->width(), mContent->height());
        } else {
            mContent->resize(mContent->width(), viewport()->height());
        }

        horizontalScrollBar()->setRange(0, v.width() - p.width());
        horizontalScrollBar()->setPageStep(p.width());
        verticalScrollBar()->setRange(0, v.height() - p.height());
        verticalScrollBar()->setPageStep(p.height());

        _updateContentPosition();
        updateTrackBarStyle();
    }

    void MStyleNavbar::_updateContentPosition()
    {
        const auto& hbar = horizontalScrollBar();
        const auto& vbar = verticalScrollBar();

        const Qt::LayoutDirection dir = layoutDirection();
        const QRect scrolled = QStyle::visualRect(dir, viewport()->rect(),
                               QRect(QPoint(-hbar->value(), -vbar->value()), mContent->size()));
        const QRect aligned = QStyle::alignedRect(dir, Qt::AlignLeft | Qt::AlignTop,
                              mContent->size(), viewport()->rect());
        mContent->move(mContent->width() < viewport()->width() ? aligned.x() : scrolled.x(),
                       mContent->height() < viewport()->height() ? aligned.y() : scrolled.y());
    }

    int MStyleNavbar::_updateItemPosition(int index)
    {
        const auto& btn = mNavGroup->button(index);
        Q_ASSERT(nullptr != btn);

        const int offset = getItemOffset(index, false);
        if (mBoundaryOffset < 0) {
            return offset;
        }

        int boundary = mBoundaryOffset;
        int scrollValue = 0;

        if (getOrientation() == Qt::Vertical) {
            if (index == 0) {
                boundary = mLayout->contentsMargins().top();
            } else if (index == getItemCount() - 1) {
                boundary = mLayout->contentsMargins().bottom();
            }

            if (offset < boundary) {
                scrollValue = boundary - offset;
            } else if (offset + boundary + btn->height() > height()) {
                scrollValue = height() - offset - btn->height() - boundary;
            }

            scroll(0, scrollValue);
            return offset + btn->height() / 2;
        } else {
            if (index == 0) {
                boundary = mLayout->contentsMargins().left();
            } else if (index == getItemCount() - 1) {
                boundary = mLayout->contentsMargins().right();
            }

            if (offset < boundary) {
                scrollValue = boundary - offset;
            } else if (offset + boundary + btn->width() > width()) {
                scrollValue = width() - offset - btn->width() - boundary;
            }

            scroll(scrollValue, 0);
            return offset + btn->width() / 2;
        }
    }

} // namespace mui
