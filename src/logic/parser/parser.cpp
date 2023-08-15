#include "parser.h"

#include <QJsonArray>

#include "renderer.h"

using namespace std;

Parser::Parser(QObject *parent)
    : QObject(parent)
{
}

void Parser::setNotePath(const QString &notePath)
{
    if (m_notePath != notePath)
        m_notePath = notePath;
}

QString Parser::getNotePath()
{
    return m_notePath;
}

QString Parser::parse(QString src)
{
    links.clear();
    tokens.clear();

    blockLexer.lex(src);

    std::reverse(tokens.begin(), tokens.end());

    QString out;
    while (getNextToken()) {
        out += tok();
    }

    return out;
}

QString Parser::tok()
{
    QString type = m_token["type"].toString();
    QString outputed, text, body;
    QVariantMap flags;

    if (type == "space") {
        return "";
    }

    if (type == "hr") {
        return Renderer::hr();
    }

    if (type == "heading") {
        text = m_token["text"].toString();

        outputed = inlineLexer.output(text);
        QString outputedText = inlineLexer.output(text, true);
        QString unescaped = Renderer::unescape(outputed);

        return Renderer::heading(outputed, m_token["depth"].toInt(), unescaped);
    }

    if (type == "code") {
        text = m_token["text"].toString();
        QString lang = m_token["lang"].toString();

        return Renderer::code(text, lang);
    }

    if (type == "table") {
        QString header = "";
        body = "";
        int i, j;

        QString cell = "";
        QStringList headersList = m_token["header"].toStringList();
        QStringList alignList = m_token["align"].toStringList();
        for (i = 0; i < headersList.size(); i++) {
            QString currentHeader = headersList[i];
            outputed = inlineLexer.output(currentHeader);

            flags = {{"header", true}, {"align", alignList[i]}};
            cell += Renderer::tableCell(outputed, flags);
        }
        header += Renderer::tableRow(cell);

        QJsonArray cellsList = m_token["cells"].toJsonArray();
        for (i = 0; i < cellsList.size(); i++) {
            QJsonArray row = cellsList[i].toArray();

            cell = "";
            for (j = 0; j < row.size(); j++) {
                QString currentCell = row[j].toString();
                outputed = inlineLexer.output(currentCell);

                flags = {{"header", false}, {"align", alignList[j]}};

                cell += Renderer::tableCell(outputed, flags);
            }
            body += Renderer::tableRow(cell);
        }

        return Renderer::table(header, body);
    }

    if (type == "blockquote_start") {
        body = "";

        while (getNextToken() && m_token["type"].toString() != "blockquote_end") {
            body += tok();
        }

        return Renderer::blockquote(body);
    }

    if (type == "list_start") {
        body = "";
        bool ordered = m_token["ordered"].toBool();
        QString start = m_token["start"].toString();

        while (getNextToken() && m_token["type"].toString() != "list_end") {
            body += tok();
        }

        return Renderer::list(body, ordered, start);
    }

    if (type == "list_item_start") {
        body = "";

        if (m_token["task"].toBool()) {
            body += Renderer::checkbox(m_token["checked"].toBool());
        }

        while (getNextToken() && m_token["type"].toString() != "list_item_end") {
            body += m_token["type"].toString() == "text" ? parseText() : tok();
        }

        return Renderer::listItem(body);
    }

    if (type == "loose_item_start") {
        body = "";

        while (getNextToken() && m_token["type"].toString() != "list_item_end") {
            body += tok();
        }

        return Renderer::listItem(body);
    }

    if (type == "html") {
        text = m_token["text"].toString();

        return Renderer::html(text);
    }

    if (type == "paragraph") {
        text = m_token["text"].toString();
        outputed = inlineLexer.output(text);

        return Renderer::paragraph(outputed);
    }

    if (type == "text") {
        QString parsedText = parseText();

        return Renderer::paragraph(parsedText);
    }

    return "";
}

QString Parser::parseText()
{
    QString body = m_token["text"].toString();

    while (peekType() == "text") {
        getNextToken();
        QString text = m_token["text"].toString();

        body += QString::fromStdString("\n") + text;
    }

    return inlineLexer.output(body);
}

bool Parser::getNextToken()
{
    m_token = (tokens.isEmpty()) ? QVariantMap{} : tokens.takeLast();

    return !m_token.isEmpty();
}

QString Parser::peekType()
{
    return (!tokens.isEmpty()) ? tokens.last()["type"].toString() : "";
}
