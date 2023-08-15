#pragma once

#include <QObject>
#include <QString>

#include "blockLexer.h"
#include "inlineLexer.h"

class Parser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString notePath WRITE setNotePath)
public:
    explicit Parser(QObject *parent = nullptr);

    Q_INVOKABLE QString parse(QString src);

    QString getNotePath();
    void setNotePath(const QString &notePath);

    QVector<QVariantHash> tokens;
    QMap<QString, QMap<QString, QString>> links;

private:
    QString tok();
    QString parseText();
    QString peekType();
    bool getNextToken();

    BlockLexer blockLexer = BlockLexer(this);
    InlineLexer inlineLexer = InlineLexer(this);

    QString m_notePath;
    QVariantHash m_token;
};
