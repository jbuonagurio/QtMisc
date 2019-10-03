// Copyright (c) 2019 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "FilterHeaderView.h"

#include <QApplication>
#include <QEvent>
#include <QIcon>
#include <QMenu>
#include <QPixmap>
#include <QStyle>

#include <QDebug>

FilterHeaderView::FilterHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    static const QIcon sortAscendingIcon = QIcon(":/images/SortAscending_16x.png");
    static const QIcon sortDescendingIcon = QIcon(":/images/SortDescending_16x.png");
    static const QIcon clearFilterIcon = QIcon(":/images/DeleteFilter_16x.png");
    static const QIcon editFilterIcon = QIcon(":/images/EditFilter_16x.png");

    sortAscendingAct = new QAction(sortAscendingIcon, tr("Sort A to Z"), this);
    sortDescendingAct = new QAction(sortDescendingIcon, tr("Sort Z to A"), this);
    clearFilterAct = new QAction(clearFilterIcon, tr("Clear Filter"), this);
    editFilterAct = new QAction(editFilterIcon, tr("Edit Filter..."), this);

    clearFilterAct->setDisabled(true);
    editFilterAct->setDisabled(true);
}

void FilterHeaderView::setAutoFilterEnabled(bool enabled)
{
    if (enabled && autoFilterEnabled())
        return;

    if (enabled) {
        setSectionsClickable(false);
        setHighlightSections(false);
        setMouseTracking(true);
        setSortIndicator(-1, Qt::AscendingOrder);
        setSortIndicatorShown(false);
        autoFilter = true;
    }
    else {
        setSectionsClickable(true);
        setHighlightSections(true);
        setMouseTracking(false);
        autoFilter = false;
    }
}

bool FilterHeaderView::autoFilterEnabled() const
{
    return autoFilter;
}

void FilterHeaderView::showAutoFilter(int section)
{
    QMenu filterMenu;
    filterMenu.addAction(sortAscendingAct);
    filterMenu.addAction(sortDescendingAct);
    filterMenu.addSeparator();
    filterMenu.addAction(clearFilterAct);
    filterMenu.addAction(editFilterAct);

    // Align with the right edge of the column.
    int width = filterMenu.sizeHint().width();
    int x = sectionViewportPosition(section) + sectionSize(section) - width;
    int y = viewport()->height();
    QPoint pos = QPoint(x, y);
    QPoint globalPos = viewport()->mapToGlobal(pos);

    QAction *selected = filterMenu.exec(globalPos);

    if (selected == sortAscendingAct) {
        columnStateMap[section] = SortFilterState::SortAscending;
        setSortIndicator(section, Qt::AscendingOrder);
    }
    else if (selected == sortDescendingAct) {
        columnStateMap[section] = SortFilterState::SortDescending;
        setSortIndicator(section, Qt::DescendingOrder);
    }

    return;
}

void FilterHeaderView::leaveEvent(QEvent *e)
{
    if (autoFilter && orientation() == Qt::Horizontal) {
        int prevSection = hoverSection;
        hoverSection = -1;
        headerDataChanged(Qt::Horizontal, prevSection, prevSection);
    }
}

void FilterHeaderView::mouseMoveEvent(QMouseEvent *e)
{
    QHeaderView::mouseMoveEvent(e);

    if (autoFilter && orientation() == Qt::Horizontal) {
        int pos = e->x();
        int section = logicalIndexAt(pos);
        if (section != hoverSection && section >= 0) {
            int prevSection = hoverSection;
            hoverSection = section;
            headerDataChanged(Qt::Horizontal, section, section);
            headerDataChanged(Qt::Horizontal, prevSection, prevSection);
        }
    }
}

void FilterHeaderView::mousePressEvent(QMouseEvent *e)
{
    if (autoFilter && orientation() == Qt::Horizontal || e->button() == Qt::LeftButton) {
        int pos = e->x();
        int section = logicalIndexAt(pos);
        if (isPointOnIcon(section, e->pos())) {
            return;
        }
    }

    QHeaderView::mousePressEvent(e);
}

void FilterHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
    if (autoFilter && orientation() == Qt::Horizontal || e->button() == Qt::LeftButton) {
        int pos = e->x();
        int section = logicalIndexAt(pos);
        if (isPointOnIcon(section, e->pos())) {
            showAutoFilter(section);
            return;
        }
    }

    QHeaderView::mouseReleaseEvent(e);
}

bool FilterHeaderView::isPointOnIcon(int section, const QPoint& pos) const
{
    if (orientation() != Qt::Horizontal)
        return false;

    QRect sectionRect = QRect(sectionViewportPosition(section), 0,
                              sectionSize(section), viewport()->height());

    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.section = section;
    opt.rect = sectionRect;

    QRect iconRect = style()->subElementRect(QStyle::SE_HeaderArrow, &opt, this);
    return iconRect.contains(pos);
}

void FilterHeaderView::paintIndicator(QPainter *painter, const QRect &rect, FilterHeaderView::SortFilterState state) const
{
    static const QPointF filter[6] = {
        QPointF(0.7500, 0.2500),
        QPointF(0.2500, 0.2500),
        QPointF(0.4375, 0.5625),
        QPointF(0.4375, 0.8125),
        QPointF(0.5625, 0.8125),
        QPointF(0.5625, 0.5625)
    };

    static const QPointF arrowUp[9] = {
        QPointF(0.5000, 0.1875),
        QPointF(0.3125, 0.3750),
        QPointF(0.3125, 0.5000),
        QPointF(0.4375, 0.3750),
        QPointF(0.4375, 0.8125),
        QPointF(0.5625, 0.8125),
        QPointF(0.5625, 0.3750),
        QPointF(0.6875, 0.5000),
        QPointF(0.6875, 0.3750)
    };

    static const QPointF arrowDown[9] = {
        QPointF(0.5000, 0.8750),
        QPointF(0.3125, 0.6875),
        QPointF(0.3125, 0.5625),
        QPointF(0.4375, 0.6875),
        QPointF(0.4375, 0.2500),
        QPointF(0.5625, 0.2500),
        QPointF(0.5625, 0.6875),
        QPointF(0.6875, 0.5625),
        QPointF(0.6875, 0.6875)
    };

    static const QPointF overlayFilter[6] = {
        QPointF(0.5625, 0.2500),
        QPointF(0.0625, 0.2500),
        QPointF(0.2500, 0.5625),
        QPointF(0.2500, 0.8125),
        QPointF(0.3750, 0.8125),
        QPointF(0.3750, 0.5625)
    };

    static const QPointF overlayArrowUp[9] = {
        QPointF(0.7500, 0.2500),
        QPointF(0.5625, 0.4375),
        QPointF(0.5625, 0.5625),
        QPointF(0.6875, 0.4375),
        QPointF(0.6875, 0.8125),
        QPointF(0.8125, 0.8125),
        QPointF(0.8125, 0.4375),
        QPointF(0.9375, 0.5625),
        QPointF(0.9375, 0.4375)
    };

    static const QPointF overlayArrowDown[9] = {
        QPointF(0.7500, 0.8125),
        QPointF(0.5625, 0.6250),
        QPointF(0.5625, 0.5000),
        QPointF(0.6875, 0.6250),
        QPointF(0.6875, 0.2500),
        QPointF(0.8125, 0.2500),
        QPointF(0.8125, 0.6250),
        QPointF(0.9375, 0.5000),
        QPointF(0.9375, 0.6250)
    };

    const QBrush arrowBrush = QColor(0, 83, 156);
    const QBrush filterBrush = QColor(66, 66, 66);

    int side = qMax(rect.width(), rect.height());
    const int x = rect.center().x() - side / 2;
    const int y = rect.center().y() - side / 2;

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(x, y);
    painter->scale(side, side);
    painter->setPen(Qt::NoPen);

    switch (state) {
    case SortFilterState::SortAscending:
        painter->setBrush(arrowBrush);
        painter->drawPolygon(arrowUp, sizeof arrowUp / sizeof arrowUp[0]);
        break;
    case SortFilterState::SortAscendingFilter:
        painter->setBrush(arrowBrush);
        painter->drawPolygon(overlayArrowUp, sizeof overlayArrowUp / sizeof overlayArrowUp[0]);
        painter->setBrush(filterBrush);
        painter->drawPolygon(overlayFilter, sizeof overlayFilter / sizeof overlayFilter[0]);
        break;
    case SortFilterState::SortDescending:
        painter->setBrush(arrowBrush);
        painter->drawPolygon(arrowDown, sizeof arrowDown / sizeof arrowDown[0]);
        break;
    case SortFilterState::SortDescendingFilter:
        painter->setBrush(arrowBrush);
        painter->drawPolygon(overlayArrowDown, sizeof overlayArrowDown / sizeof overlayArrowDown[0]);
        painter->setBrush(filterBrush);
        painter->drawPolygon(overlayFilter, sizeof overlayFilter / sizeof overlayFilter[0]);
        break;
    case SortFilterState::Filter:
        painter->setBrush(filterBrush);
        painter->drawPolygon(filter, sizeof filter / sizeof filter[0]);
        break;
    default:
        break;
    }

    painter->restore();
}

void FilterHeaderView::paintSection(QPainter *painter, const QRect &rect, int section) const
{
    // Use custom icon in place of PE_IndicatorHeaderArrow
    // See: qcommonstyle.cpp, drawControl, CE_Header

    if (!autoFilter) {
        QHeaderView::paintSection(painter, rect, section);
        return;
    }

    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.section = section;
    opt.rect = rect;

    // Draw the header.
    style()->drawControl(QStyle::CE_HeaderSection, &opt, painter, this);

    // Calculate icon rect and label rects with/without icon.
    QRect iconRect = style()->subElementRect(QStyle::SE_HeaderArrow, &opt, this);
    opt.sortIndicator = QStyleOptionHeader::None;
    QRect labelRect0 = style()->subElementRect(QStyle::SE_HeaderLabel, &opt, this);
    opt.sortIndicator = QStyleOptionHeader::SortUp;
    QRect labelRect1 = style()->subElementRect(QStyle::SE_HeaderLabel, &opt, this);

    // Draw sort or hover indicators?
    bool drawIndicator = (isSortIndicatorShown() && sortIndicatorSection() == section);
    bool drawIcon = (section == hoverSection || drawIndicator);

    // Set the header text and elide if necessary.
    QAbstractItemModel *m = model();
    if (m) {
        QString headerText = m->headerData(section, orientation(), Qt::DisplayRole).toString();
        QVariant textAlignment = m->headerData(section, orientation(), Qt::TextAlignmentRole);
        opt.textAlignment = Qt::Alignment(textAlignment.isValid() ?
            Qt::Alignment(textAlignment.toInt()) : defaultAlignment());

        // Use the label rect without icon if possible, to maintain position.
        QFontMetrics fm(font());
        if (drawIcon) {
            opt.text = fm.elidedText(headerText, Qt::ElideRight, labelRect1.width());
            QRect elidedRect = fm.boundingRect(labelRect0, opt.textAlignment, opt.text);
            opt.rect = (elidedRect.right() <= labelRect1.right()) ?
                labelRect0 : labelRect1;
        }
        else {
            opt.text = fm.elidedText(headerText, Qt::ElideRight, labelRect0.width());
            opt.rect = labelRect0;
        }

        if (opt.rect.isValid()) {
            style()->drawControl(QStyle::CE_HeaderLabel, &opt, painter, this);
        }
    }

    // TODO: allow multiple sorts; use column state map instead of sortIndicatorSection.
    if (drawIcon) {
        if (drawIndicator && sortIndicatorOrder() == Qt::AscendingOrder)
            paintIndicator(painter, iconRect, SortFilterState::SortAscending);
        else if (drawIndicator && sortIndicatorOrder() == Qt::DescendingOrder)
            paintIndicator(painter, iconRect, SortFilterState::SortDescending);
        else
            paintIndicator(painter, iconRect, SortFilterState::Filter);
    }
}
