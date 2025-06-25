// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#include "printingHelper.h"

#include <QFile>
#include <QPrinter>
#include <QTextDocument>
#include <klocalizedstring.h>

PrintingUtility::PrintingUtility(QObject *parent)
    : QObject(parent)
{
}

void PrintingUtility::writePdf(const QString &path) const
{
    if (QFile::exists(path)) {
        return;
    }

    QTextDocument document;

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.print(&printer);
}

void PrintingUtility::copy(const QString &fromPath, const QString &toPath)
{
    static const QString errorMessage = i18n("An error occurred while trying to copy this PDF.");
    if (QFile::exists(toPath)) {
        const bool previousRemoved = QFile::remove(toPath);
        if (!previousRemoved) {
            Q_EMIT pdfCopyDone(false, errorMessage);
            return;
        }
    }

    const bool isCopied = QFile::copy(fromPath, toPath);
    if (!isCopied) {
        Q_EMIT pdfCopyDone(false, errorMessage);
        return;
    }

    Q_EMIT pdfCopyDone(true, {});
}
