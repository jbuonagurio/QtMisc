// Copyright (c) 2019 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "ExtendedComboBox.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QPoint>
#include <QScrollBar>
#include <QWidget>

ExtendedComboBox::ExtendedComboBox(QWidget *parent)
    : QComboBox(parent)
{
    QAbstractItemView *v = this->view();
    if (v == nullptr)
        return;

    v->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    v->setAutoScroll(false);

    // Store the internal QFrame (QComboBoxPrivateContainer) and scrollers
    // (QComboBoxPrivateScroller). These are constucted after the view.
    container_ = this->findChild<QFrame *>();
    if (container_ == nullptr)
        return;

    QList<QWidget *> widgets = container_->findChildren<QWidget *>(
        QString(), Qt::FindDirectChildrenOnly);

    for (QWidget *c : widgets) {
        const QString name = c->metaObject()->className();
        if (name == "QComboBoxPrivateScroller") {
            scrollers_.push_back(c);
        }
    }

    // Disconnect the scrollbar from the internal QFrame.
    QScrollBar *scrollbar = v->verticalScrollBar();
    if (scrollbar) {
        disconnect(scrollbar, &QScrollBar::valueChanged, container_, nullptr);
        disconnect(scrollbar, &QScrollBar::rangeChanged, container_, nullptr);
    }
}

void ExtendedComboBox::setPopupHeight(int height)
{
    popupHeight_ = height;
}

int ExtendedComboBox::popupHeight() const
{
    return popupHeight_;
}

void ExtendedComboBox::showPopup()
{
    this->setUpdatesEnabled(false);
    QComboBox::showPopup();

    // Update the height of the internal QFrame.
    if (container_ && popupHeight_ > 0) {
        container_->resize(container_->width(), popupHeight_);
        QPoint containerPos = container_->pos();
        QPoint globalPos = this->mapToGlobal(containerPos);
        container_->move(container_->x(), globalPos.y() - containerPos.y());
    }

    // Hide the up/down scrollers.
    for (QWidget *scroller : scrollers_)
        scroller->hide();

    this->setUpdatesEnabled(true);
}
