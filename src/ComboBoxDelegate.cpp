// Copyright (c) 2019 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "ComboBoxDelegate.h"

#include <QComboBox>

ComboBoxDelegate::ComboBoxDelegate(QAbstractItemModel *model, int column, QObject *parent)
    : QStyledItemDelegate(parent), m_editorModel(model), m_editorColumn(column)
{}

void ComboBoxDelegate::setEditorModel(QAbstractItemModel *model)
{
    m_editorModel = model;
}

void ComboBoxDelegate::setEditorModelColumn(int column)
{
    m_editorColumn = column;
}

QWidget* ComboBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    QComboBox *cb = new QComboBox(parent);
    cb->setFrame(false);
    cb->setModel(m_editorModel);
    cb->setModelColumn(m_editorColumn);

    connect(cb, QOverload<int>::of(&QComboBox::activated),
            this, &ComboBoxDelegate::emitCommitData);

    return cb;
}

void ComboBoxDelegate::setEditorData(QWidget *editor,
    const QModelIndex& index) const
{
    if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
       int cbIndex = cb->findData(index.data(Qt::EditRole), Qt::EditRole);
       if (cbIndex >= 0)
           cb->setCurrentIndex(cbIndex);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ComboBoxDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model,
    const QModelIndex& index) const
{
    if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
        model->setData(index, cb->currentData(Qt::EditRole), Qt::EditRole);
        // The current index is stored as Qt::UserRole.
        model->setData(index, cb->currentIndex(), Qt::UserRole);
    }
    else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem& option,
    const QModelIndex&) const
{
    editor->setGeometry(option.rect);
}

bool ComboBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
    const QStyleOptionViewItem& option, const QModelIndex& index)
{
    emit editRequested();
    return false;
}

void ComboBoxDelegate::emitCommitData()
{
    // Notify other application components of the change.
    emit commitData(qobject_cast<QWidget *>(sender()));
}