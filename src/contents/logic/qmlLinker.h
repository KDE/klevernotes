#ifndef QMLLINKER_H
#define QMLLINKER_H

#include <QObject>
#include <QString>
#include <QVariantMap>

class QmlLinker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER changedText NOTIFY textChanged FINAL)
    Q_PROPERTY(QVariantMap css MEMBER changedCss NOTIFY cssChanged FINAL)
    Q_PROPERTY(QString homePath MEMBER changedHomePath NOTIFY homePathChanged FINAL)
    Q_PROPERTY(QString notePath MEMBER changedNotePath NOTIFY notePathChanged FINAL)

public:
    explicit QmlLinker(QObject *parent = nullptr);

signals:
    void textChanged(const QString &text);
    void cssChanged(const QVariantMap &css);
    void homePathChanged(const QString &homePath);
    void notePathChanged(const QString &notePath);

private:
    QString changedText;
    QVariantMap changedCss;
    QString changedHomePath;
    QString changedNotePath;
};

#endif // QMLLINKER_H
