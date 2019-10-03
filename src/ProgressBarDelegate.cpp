// Copyright (c) 2019 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "ProgressBarDelegate.h"

#include <QApplication>
#include <QModelIndex>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionProgressBar>

ProgressBarDelegate::ProgressBarDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

void ProgressBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    if (index.data().canConvert<int>())
    {
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.state = QStyle::State_Enabled;
        progressBarOption.direction = QApplication::layoutDirection();
        progressBarOption.rect = option.rect;
        progressBarOption.fontMetrics = QApplication::fontMetrics();
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.textAlignment = Qt::AlignCenter;
        progressBarOption.textVisible = true;

        // Set the progress and text values of the style option.
        int progress = index.data(Qt::DisplayRole).toInt();
        progressBarOption.progress = progress < 0 ? 0 : progress;
        progressBarOption.text = QString::asprintf("%d%%", progressBarOption.progress);

        // Use base background fill even when selected.
        if (option.state & QStyle::State_Selected) {
            if (option.features & QStyleOptionViewItem::Alternate)
                painter->fillRect(option.rect, option.palette.alternateBase());
            else
                painter->fillRect(option.rect, option.palette.base());
        }

        // Draw a CE_ProgressBar without CE_ProgressBarGroove.
        QApplication::style()->drawControl(QStyle::CE_ProgressBarContents, &progressBarOption, painter);
        QApplication::style()->drawControl(QStyle::CE_ProgressBarLabel, &progressBarOption, painter);
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ProgressBarDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}