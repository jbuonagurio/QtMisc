// Copyright (c) 2019 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "ColorPickerDelegate.h"

#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QImage>
#include <QModelIndex>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionViewItem>

ColorPickerDelegate::ColorPickerDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{}

bool ColorPickerDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
    const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() != QEvent::MouseButtonDblClick)
        return false;

    QColor color = index.data(Qt::DisplayRole).value<QColor>();
    QColorDialog dialog(color);
    dialog.setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
    int rc = dialog.exec();
    if (rc != QDialog::Accepted)
        return false;

    color = dialog.currentColor();
    model->setData(index, color, Qt::EditRole);
    return true;
}

void ColorPickerDelegate::paint(QPainter *painter,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QColor color = index.data(Qt::DisplayRole).value<QColor>();
    QPixmap pixmap = brushValuePixmap(QBrush(color));

    const int x = option.rect.center().x() - pixmap.rect().width() / 2;
    const int y = option.rect.center().y() - pixmap.rect().height() / 2;

    // Draw background when selected.
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    else
        painter->fillRect(option.rect, option.palette.window());

    painter->drawPixmap(QRect(x, y, pixmap.rect().width(), pixmap.rect().height()), pixmap);
}

QSize ColorPickerDelegate::sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    int iconSize = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
    return QSize(iconSize, iconSize);
}

QPixmap ColorPickerDelegate::brushValuePixmap(const QBrush& brush)
{
    QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);

    QPainter painter(&img);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(0, 0, img.width(), img.height(), brush);
    QColor color = brush.color();
    if (color.alpha() != 255) { // indicate alpha by an inset
        QBrush opaqueBrush = brush;
        color.setAlpha(255);
        opaqueBrush.setColor(color);
        painter.fillRect(img.width() / 4, img.height() / 4,
                         img.width() / 2, img.height() / 2, opaqueBrush);
    }
    painter.end();

    return QPixmap::fromImage(img);
}