// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// In part based on: https://github.com/igormironchik/markdown-tools/tree/main 'md-editor'

#pragma once

// KleverNotes include
#include "colors.hpp"
#include "logic/parser/renderer.h"

// md4qt include.
#define MD4QT_QT_SUPPORT
#include "logic/md4qt/doc.hpp"
#include "logic/md4qt/traits.hpp"

// Qt include
#include <QObject>
#include <QQuickTextDocument>
#include <QTextDocument>
#include <QtQml>

Q_DECLARE_METATYPE(QQuickTextDocument);

namespace MdEditor
{

class Parser;
class SyntaxVisitor;
/* struct EditorPrivate; */
class EditorHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument *document READ qQuickDocument WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QString notePath READ getNotePath WRITE setNotePath)

    // NoteMapper
    Q_PROPERTY(QStringList headerInfo WRITE setHeaderInfo)
    Q_PROPERTY(QString headerLevel READ headerLevel CONSTANT)

public:
    explicit EditorHandler(QObject *parent = nullptr);

    void setDocument(QQuickTextDocument *document);
    QTextDocument *document() const;
    QQuickTextDocument *qQuickDocument() const;

    void parseDoc();
    Q_INVOKABLE void parse(const QString &src);
    QString getNotePath() const;
    void setNotePath(const QString &notePath);
    Parser *parser() const;

    // NoteMapper
    void setHeaderInfo(const QStringList &headerInfo);
    QString headerLevel() const;

    // md-editor
    std::shared_ptr<MD::Document<MD::QStringTrait>> currentDoc() const;
    void applyFont(const QFont &f);
    SyntaxVisitor *syntaxHighlighter() const;

Q_SIGNALS:
    void documentChanged();

    void parsingFinished(const QString &content);

    // NoteMapper
    void newLinkedNotesInfos(const QSet<QStringList> &linkedNotesInfos);
    void noteHeadersSent(const QString &notePath, const QStringList &noteHeaders);

    // md-editor
    void lineHovered(int lineNumber, const QPoint &pos);
    void hoverLeaved();
    /* void ready(); */

private Q_SLOTS:
    // Code highlight
    void newHighlightStyle();

    // PUML
    void pumlDarkChanged();

    // md-editor
    /* void highlightCurrentLine(); */
    void highlightSyntax(const Colors &colors, std::shared_ptr<MD::Document<MD::QStringTrait>> doc);

protected:
    // md-editor
    int lineNumber(const QPoint &p);

private:
    void addExtendedSyntax(const QStringList &details);
    void addExtendedSyntaxs(const QList<QStringList> &syntaxsDetails);

private:
    QQuickTextDocument *m_qQuickDocument = nullptr;
    QTextDocument *m_document = nullptr;

    enum ExtensionID : int {
        /* Plugins
         * ===============*/
        KleverPlugins = 256,
        /* Extended syntax
         * ===============*/
        ExtendedSyntax = KleverPlugins + 64,
    };
    int m_extendedSyntaxCount = 0;
    QString m_notePath;
    Renderer *m_renderer = nullptr;
    Parser *m_parser = nullptr;

    std::shared_ptr<MD::Document<MD::QStringTrait>> m_currentMdDoc = nullptr;
    Colors colors;

    SyntaxVisitor *m_syntaxvisitor = nullptr;
    bool m_highlighting = false;

    // md-editor
    /* friend struct EditorPrivate; */

    Q_DISABLE_COPY(EditorHandler)
    /* QScopedPointer< EditorPrivate > d; */
};
}
