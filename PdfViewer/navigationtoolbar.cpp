/*
 * Copyright (C) 2008-2009, Pino Toscano <pino@kde.org>
 * Copyright (C) 2013, Fabio D'Urso <fabiodurso@hotmail.it>
 * Copyright (C) 2019, Albert Astals Cid <aacid@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "navigationtoolbar.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QComboBox>

NavigationToolBar::NavigationToolBar(fz_context *ctx, QWidget *parent)
    : QToolBar(parent)
    , m_ctx(ctx)
{
    m_firstAct = addAction(u8"首页", this, SLOT(onGoFirst()));
    m_prevAct = addAction(u8"上一页", this, SLOT(onGoPrev()));
    m_pageCombo = new QComboBox(this);
    // TODO replace with qOverload once we start requiring Qt 5.7
    connect(m_pageCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &NavigationToolBar::onComboActivated);
    addWidget(m_pageCombo);
    m_nextAct = addAction(u8"下一页", this, SLOT(onGoNext()));
    m_lastAct = addAction(u8"尾页", this, SLOT(onGoLast()));

    addSeparator();

    m_zoomCombo = new QComboBox(this);
    m_zoomCombo->setEditable(true);
    m_zoomCombo->addItem(u8"10%");
    m_zoomCombo->addItem(u8"25%");
    m_zoomCombo->addItem(u8"33%");
    m_zoomCombo->addItem(u8"50%");
    m_zoomCombo->addItem(u8"66%");
    m_zoomCombo->addItem(u8"75%");
    m_zoomCombo->addItem(u8"100%");
    m_zoomCombo->addItem(u8"125%");
    m_zoomCombo->addItem(u8"150%");
    m_zoomCombo->addItem(u8"200%");
    m_zoomCombo->addItem(u8"300%");
    m_zoomCombo->addItem(u8"400%");
    m_zoomCombo->setCurrentIndex(6); // "100%"
    // TODO replace with qOverload once we start requiring Qt 5.7
    connect(m_zoomCombo, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &NavigationToolBar::onZoomComboChanged);
    addWidget(m_zoomCombo);

    m_rotationCombo = new QComboBox(this);
    // NOTE: \302\260 = degree symbol
    m_rotationCombo->addItem(u8"0°");
    m_rotationCombo->addItem(u8"90°");
    m_rotationCombo->addItem(u8"180°");
    m_rotationCombo->addItem(u8"270°");
    // TODO replace with qOverload once we start requiring Qt 5.7
    connect(m_rotationCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &NavigationToolBar::onRotationComboChanged);
    addWidget(m_rotationCombo);

    documentClosed();
}

NavigationToolBar::~NavigationToolBar() { }

void NavigationToolBar::documentLoaded()
{
    fz_document* doc = document();
    int iPages = fz_count_pages(m_ctx, doc);
    for (int i = 0; i < iPages; ++i) {
        m_pageCombo->addItem(QString::number(i + 1));
    }
    m_pageCombo->setEnabled(true);
}

void NavigationToolBar::documentClosed()
{
    m_firstAct->setEnabled(false);
    m_prevAct->setEnabled(false);
    m_nextAct->setEnabled(false);
    m_lastAct->setEnabled(false);
    m_pageCombo->clear();
    m_pageCombo->setEnabled(false);
}

void NavigationToolBar::pageChanged(int page)
{
    fz_document* doc = document();
    int iPages = fz_count_pages(m_ctx, doc);
    m_firstAct->setEnabled(page > 0);
    m_prevAct->setEnabled(page > 0);
    m_nextAct->setEnabled(page < (iPages - 1));
    m_lastAct->setEnabled(page < (iPages - 1));
    m_pageCombo->setCurrentIndex(page);
}

void NavigationToolBar::onGoFirst()
{
    setPage(0);
}

void NavigationToolBar::onGoPrev()
{
    setPage(page() - 1);
}

void NavigationToolBar::onGoNext()
{
    setPage(page() + 1);
}

void NavigationToolBar::onGoLast()
{
    fz_document* doc = document();
    int iPages = fz_count_pages(m_ctx, doc);
    setPage(iPages - 1);
}

void NavigationToolBar::onComboActivated(int index)
{
    setPage(index);
}

void NavigationToolBar::onZoomComboChanged(const QString &_text)
{
    QString text = _text;
    text.remove(QLatin1Char('%'));
    bool ok = false;
    int value = text.toInt(&ok);
    if (ok && value >= 10) {
        emit sigZoomChanged(qreal(value) / 100);
    }
}

void NavigationToolBar::onRotationComboChanged(int idx)
{
    emit sigRotationChanged(idx * 90);
}
