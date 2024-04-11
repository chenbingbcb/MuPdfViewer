/*
 * Copyright (C) 2008-2009, Pino Toscano <pino@kde.org>
 * Copyright (C) 2008, 2019, Albert Astals Cid <aacid@kde.org>
 * Copyright (C) 2009, Shawn Rutledge <shawn.t.rutledge@gmail.com>
 * Copyright (C) 2013, Fabio D'Urso <fabiodurso@hotmail.it>
 * Copyright (C) 2020, Oliver Sander <oliver.sander@tu-dresden.de>
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

#include "viewer.h"
#include "navigationtoolbar.h"
#include "pageview.h"

#include <QDebug>

PdfViewer::PdfViewer(QWidget *parent)
    : m_nCurPage(0)
    , m_doc(nullptr)
{
    m_ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    fz_try(m_ctx) {
        fz_register_document_handlers(m_ctx);
    }
    fz_catch(m_ctx)
    {
        qDebug()<<"cannot register document handlers: %s\n", fz_caught_message(m_ctx);
        fz_drop_context(m_ctx);
        return;
    }

    setObjectName("PdfViewer");
    resize(960, 720);


    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    NavigationToolBar* navbar = new NavigationToolBar(m_ctx, this);
    verticalLayout->addWidget(navbar, 0, Qt::AlignHCenter);

    QSplitter *c_MainSplitter = new QSplitter(this);
    c_MainSplitter->setObjectName(QString::fromUtf8("c_MainSplitter"));
    c_MainSplitter->setLineWidth(1);
    c_MainSplitter->setOrientation(Qt::Horizontal);
    c_MainSplitter->setHandleWidth(2);
    c_MainSplitter->setChildrenCollapsible(false);

    PageView* c_MainWidget = new PageView(m_ctx, parent, c_MainSplitter);
    c_MainWidget->setObjectName(QString::fromUtf8("c_MainWidget"));
    QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(c_MainWidget->sizePolicy().hasHeightForWidth());
    c_MainWidget->setSizePolicy(sizePolicy4);
    c_MainWidget->setMinimumSize(QSize(1065, 0));
    c_MainWidget->setMaximumSize(QSize(16777215, 16777215));
    c_MainWidget->setStyleSheet(QString::fromUtf8(""));
    c_MainSplitter->addWidget(c_MainWidget);

    verticalLayout->addWidget(c_MainSplitter);

    m_observers.append(navbar);
    m_observers.append(c_MainWidget);

    Q_FOREACH (DocumentObserver *obs, m_observers) {
        obs->m_viewer = this;
    }

    connect(navbar, &NavigationToolBar::sigZoomChanged, c_MainWidget, &PageView::onZoomChanged);
    connect(navbar, &NavigationToolBar::sigRotationChanged, c_MainWidget, &PageView::onRotationChanged);
}

QSize PdfViewer::sizeHint() const
{
    return QSize(500, 600);
}

PdfViewer::~PdfViewer()
{
    closeDocument();
    fz_drop_context(m_ctx);
    m_ctx = nullptr;
}

int PdfViewer::loadDocument(const QString &szFileName, QString& szRetError)
{
    closeDocument();

    fz_document *newdoc = nullptr;
    fz_try(m_ctx){
        newdoc= fz_open_document(m_ctx, szFileName.toUtf8().data());
    }
    fz_catch(m_ctx)
    {
        szRetError = u8"无法打开文件："+ szFileName;
        return -1;
    }

    m_doc = newdoc;

    Q_FOREACH (DocumentObserver *obs, m_observers) {
        obs->documentLoaded();
        obs->pageChanged(0);
    }

    m_szFileName = szFileName;
    return 0;
}

void PdfViewer::closeDocument()
{
    if (!m_doc) {
        return;
    }

    Q_FOREACH (DocumentObserver *obs, m_observers) {
        obs->documentClosed();
    }

    fz_drop_document(m_ctx, m_doc);
    m_nCurPage = 0;
    m_doc = nullptr;
    m_szFileName.clear();
}

void PdfViewer::setPage(int iPage)
{
    if (iPage < 0)
    {
        iPage = 0;
    }

    int iPages = fz_count_pages(m_ctx, m_doc);
    if (iPage >= iPages)
    {
        iPage = iPages - 1;
    }

    Q_FOREACH (DocumentObserver *obs, m_observers) {
        obs->pageChanged(iPage);
    }

    m_nCurPage = iPage;
}

//显示页数要+1
int PdfViewer::page() const
{
    return m_nCurPage;
}
