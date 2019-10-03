// Copyright (c) 2019 John Buonagurio (jbuonagurio at exponent dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <QComboBox>
#include <QList>

QT_BEGIN_NAMESPACE
class QFrame;
class QWidget;
QT_END_NAMESPACE

class ExtendedComboBox : public QComboBox
{
public:
    explicit ExtendedComboBox(QWidget *parent = nullptr);
    void setPopupHeight(int height);
    int popupHeight() const;

protected:
    virtual void showPopup() override;

private:
    QFrame *container_ = nullptr;
    QList<QWidget *> scrollers_;
    int popupHeight_ = 0;
};
