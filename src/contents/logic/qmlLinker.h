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

public:
    explicit QmlLinker(QObject *parent = nullptr);

signals:
    void textChanged(const QString &text);
    void cssChanged(const QVariantMap &css);

private:
    QString changedText;
    QVariantMap changedCss;
};

#endif // QMLLINKER_H
