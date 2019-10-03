// Copyright (c) 2019 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <QAbstractItemDelegate>
#include <QBrush>
#include <QPixmap>

class ColorPickerDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit ColorPickerDelegate(QObject *parent = nullptr);
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
        const QStyleOptionViewItem& option, const QModelIndex& index) override;

    static QPixmap brushValuePixmap(const QBrush& brush);
};
