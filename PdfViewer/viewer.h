/*
 * Copyright (C) 2008, Pino Toscano <pino@kde.org>
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

#ifndef PDFVIEWER_H
#define PDFVIEWER_H

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplitter>

#include "mupdf/pdf.h"

#include "documentobserver.h"

class PdfViewer : public QWidget
{
    Q_OBJECT
    friend class DocumentObserver;
public:
    PdfViewer(QWidget *parent = nullptr);
    ~PdfViewer();

    /**
    * @brief 加载打开pdf文档
    * @param szFileName pdf文件路径
    */
    int loadDocument(const QString &szFileName,QString& szRetError);

    /**
    * @brief 关闭文档
    */
    void closeDocument();

    /**
    * @brief 设置pdf页数 从0开始 显示页数要+1
    * @param iPage pdf页数
    */
    void setPage(int iPage);

    /**
    * @brief 获取pdf页数 从0开始 显示页数要+1
    * @return  pdf页数
    */
    int page() const;

protected:
    QSize sizeHint() const override;

private:
    int m_nCurPage;
    QList<DocumentObserver *> m_observers;
    fz_context *m_ctx;
    fz_document *m_doc;
    QString m_szFileName;
};

#endif
