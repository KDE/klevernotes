#pragma once

#include <QObject>
#include <QString>

#include "blockLexer.h"
#include "inlineLexer.h"
#include "kleverconfig.h"

class Parser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString notePath WRITE setNotePath)
    Q_PROPERTY(bool highlightEnabled WRITE setHighlightEnabled)
public:
    explicit Parser(QObject *parent = nullptr);

    Q_INVOKABLE QString parse(QString src);

    QString getNotePath();
    void setNotePath(const QString &notePath);

    QVector<QVariantMap> tokens;
    QMap<QString, QMap<QString, QString>> links;

    // Syntax highlight
    void setHighlightEnabled(const bool highlightEnabled);
    bool highlightEnabled() const;
    void addToNoteCodeBlocks(const QString &codeBlock);

public slots:
    void newHighlightStyle();

private:
    QString tok();
    QString parseText();
    QString peekType();
    bool getNextToken();

    BlockLexer blockLexer = BlockLexer(this);
    InlineLexer inlineLexer = InlineLexer(this);

    QString m_notePath;
    QVariantMap m_token;

    // Synthax highlight
    bool m_highlightEnabled = KleverConfig::codeSynthaxHighlightEnabled();
    bool m_newHighlightStyle = true;
    bool m_sameCodeBlocks = false;
    int m_currentBlockIndex = 0;
    QStringList m_noteCodeBlocks;
    QStringList m_previousHighlightedBlocks;
    QStringList m_previousNoteCodeBlocks;
};
