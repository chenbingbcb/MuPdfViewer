/*
 * Copyright (C) 2008-2009, Pino Toscano <pino@kde.org>
 * Copyright (C) 2013, Fabio D'Urso <fabiodurso@hotmail.it>
 * Copyright (C) 2017, Albert Astals Cid <aacid@kde.org>
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

#include "pageview.h"

#include "mupdf/fitz/document.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtGui/QImage>
#include <QtWidgets/QLabel>
#include <QtGui/QPixmap>
#include <QDebug>
#include <QtWidgets/QScrollBar>
#include <QtGui/QWheelEvent>

PageView::PageView(fz_context *ctx, QWidget* root, QWidget *parent)
    : QScrollArea(parent)
    , m_ctx(ctx)
    , m_zoom(1.0)
    , m_rotation(0)
    , m_dpiX(QApplication::desktop()->physicalDpiX())
    , m_dpiY(QApplication::desktop()->physicalDpiY())
    , m_scrolling(false)
    , m_root(root)
{
    m_imageLabel = new QLabel(this);
    m_imageLabel->setStyleSheet("QLabel { background-color : white;}");
    m_imageLabel->resize(0, 0);
    setWidget(m_imageLabel);
    setAlignment(Qt::AlignHCenter);
}

PageView::~PageView() { }

void PageView::documentLoaded() { }

void PageView::documentClosed()
{
    m_imageLabel->clear();
    m_imageLabel->resize(0, 0);
}

void PageView::pageChanged(int iPage)
{
    fz_document* doc = document();
    fz_page *page = fz_load_page(m_ctx, doc, iPage);
    if (!page) {
        qDebug()<<"Cant load page.Droping page & exiting.";
        return;
    }

    fz_rect bounds;
    fz_irect bbox;
    bounds = fz_bound_page(m_ctx, page);
    bbox = fz_round_rect(bounds);

    // build transform matrix
    fz_matrix transform = fz_rotate(m_rotation);
    transform = fz_pre_scale(transform, m_zoom * 1.97f, m_zoom * 1.97f);

    // get transformed page size
    bounds = fz_transform_rect(bounds, transform);
    bbox = fz_round_rect(bounds);

    fz_pixmap *pix =fz_new_pixmap_from_page_number(m_ctx,doc,iPage,transform,fz_device_bgr(m_ctx),1);

    QImage image;
    unsigned char *samples = NULL;
    unsigned char  *copyed_samples = NULL;
    int width = 0;
    int height = 0;

    samples = fz_pixmap_samples(m_ctx, pix);
    width = fz_pixmap_width(m_ctx, pix);
    height = fz_pixmap_height(m_ctx, pix);

    copyed_samples=samples;

#if QT_VERSION < 0x050200
    // most computers use little endian, so Format_ARGB32 means bgra order
    // note: this is not correct for computers with big endian architecture
    image = QImage(copyed_samples,
                   width, height, QImage::Format_ARGB32, imageCleanupHandler, copyed_samples);
#else
    // with Qt 5.2, Format_RGBA8888 is correct for any architecture
    image = QImage(copyed_samples,
                   width, height, QImage::Format_ARGB32, NULL, copyed_samples);
#endif

    fz_drop_page(m_ctx,page);

    if (!image.isNull()) {
        m_imageLabel->resize(image.size());
        m_imageLabel->setPixmap(QPixmap::fromImage(image));
    } else {
        m_imageLabel->resize(0, 0);
        m_imageLabel->setPixmap(QPixmap());
    }

    if (m_scrolling)
    {
        //滚动翻页
        QScrollBar * vScrollBar = verticalScrollBar();
        if (vScrollBar)
        {
            int vsliderPos = iPage < this->page() ? vScrollBar->maximum() : vScrollBar->minimum();
            vScrollBar->setSliderPosition(vsliderPos);
        }
    }
    else
    {
        //点击按钮翻页
        QScrollBar * vScrollBar = verticalScrollBar();
        if (vScrollBar)
        {
            vScrollBar->setSliderPosition(vScrollBar->minimum());
        }
        QScrollBar * hScrollBar = horizontalScrollBar();
        if (hScrollBar)
        {
            hScrollBar->setSliderPosition(hScrollBar->minimum());
        }
    }
}

void PageView::onZoomChanged(qreal value)
{
    m_zoom = value;
    if (!document()) {
        return;
    }
    reloadPage();
}

void PageView::onRotationChanged(int value)
{
    m_rotation = value;
    if (!document()) {
        return;
    }
    reloadPage();
}

#if QT_CONFIG(wheelevent)
void PageView::wheelEvent(QWheelEvent * e)
{
    m_scrolling = true;
    QScrollArea::wheelEvent(e);

    QScrollBar * vScrollBar = verticalScrollBar();
    if (vScrollBar)
    {
        int curSliderPos = vScrollBar->sliderPosition();
        if (curSliderPos == vScrollBar->minimum())
        {
            if (page() > 0)
            {
                setPage(page() - 1);
            }
        }
        else if (curSliderPos == vScrollBar->maximum())
        {
            fz_document* doc = document();
            int iPages = fz_count_pages(m_ctx, doc);
            if (page() < iPages - 1)
            {
                setPage(page() + 1);
            }
        }
    }

    m_scrolling = false;
}
#endif
