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
    * @brief ���ش�pdf�ĵ�
    * @param szFileName pdf�ļ�·��
    */
    int loadDocument(const QString &szFileName,QString& szRetError);

    /**
    * @brief �ر��ĵ�
    */
    void closeDocument();

    /**
    * @brief ����pdfҳ�� ��0��ʼ ��ʾҳ��Ҫ+1
    * @param iPage pdfҳ��
    */
    void setPage(int iPage);

    /**
    * @brief ��ȡpdfҳ�� ��0��ʼ ��ʾҳ��Ҫ+1
    * @return  pdfҳ��
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
