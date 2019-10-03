// Copyright (c) 2019 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "DockWidgetProxyStyle.h"

#include <QApplication>
#include <QIcon>
#include <QFontMetrics>
#include <QDockWidget>
#include <QStyleFactory>
#include <QStyleOption>
#include <QStyleOptionDockWidget>

DockWidgetProxyStyle::DockWidgetProxyStyle()
    : QProxyStyle(QStyleFactory::create("fusion"))
{}

int DockWidgetProxyStyle::pixelMetric(PixelMetric which,
    const QStyleOption *option, const QWidget *widget) const
{
    switch (which) {
    case PM_DockWidgetTitleMargin:
        return 6;
    default:
        return QProxyStyle::pixelMetric(which, option, widget);
    }
}

QIcon DockWidgetProxyStyle::standardIcon(QStyle::StandardPixmap icon,
    const QStyleOption *option, const QWidget *widget) const
{
    // Set custom float and close button icons.
    static const QIcon floatIcon = QIcon(":/images/Restore_NoHalo_24x.png");
    static const QIcon closeIcon = QIcon(":/images/Close_NoHalo_24x.png");

    switch (icon) {
    case QStyle::SP_TitleBarNormalButton:
        return floatIcon;
    case QStyle::SP_TitleBarCloseButton:
        return closeIcon;
    case QStyle::SP_DialogCloseButton: // Tab Close Button
        return closeIcon;
    default:
        return QProxyStyle::standardIcon(icon, option, widget);
    }
}

void DockWidgetProxyStyle::drawControl(QStyle::ControlElement element,
    const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const QStyleOptionDockWidget *dwOption;
    const QDockWidget *dw;

    if (element == CE_DockWidgetTitle &&
       (dwOption = qstyleoption_cast<const QStyleOptionDockWidget *>(option)) &&
       (dw = qobject_cast<const QDockWidget *>(widget)) &&
       !dw->titleBarWidget())
    {
        // Calculate the title bar height.
        int iconSize = dw->style()->pixelMetric(PM_SmallIconSize, nullptr, dw);
        int buttonMargin = dw->style()->pixelMetric(PM_DockWidgetTitleBarButtonMargin, nullptr, dw);
        int buttonHeight = iconSize + 2+buttonMargin;
        QFontMetrics titleFontMetrics = dw->fontMetrics();
        int titleMargin = dw->style()->pixelMetric(PM_DockWidgetTitleMargin, nullptr, dw);
        int titleHeight = qMax(buttonHeight + 2, titleFontMetrics.height() + 2*titleMargin);

        // Draw a background rectangle for the title bar.
        QColor bgColor = dwOption->palette.window().color().darker(104);
        QRect bgRect = dwOption->rect;
        bgRect.setHeight(titleHeight);
        painter->fillRect(bgRect, bgColor);

        // Add some left padding to the title text.
        QStyleOptionDockWidget customOption = *dwOption;
        customOption.rect.setLeft(5);

        QProxyStyle::drawControl(element, &customOption, painter, widget);
        return;
    }

    QProxyStyle::drawControl(element, option, painter, widget);
}
