/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
*/

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtQml>
#include <QtWebEngine/QtWebEngine>

#include "about.h"
#include "app.h"
#include "version-klever.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "logic/documentHandler.h"
#include "logic/storageHandler.h"
#include "logic/kleverUtility.h"
#include "contents/logic/mdHandler.h"
#include "contents/logic/qmlLinker.h"
#include "contents/logic/view.h"
#include "kleverconfig.h"


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QtWebEngine::initialize();
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setApplicationName(QStringLiteral("Klever"));

    KAboutData aboutData(
                         // The program name used internally.
                         QStringLiteral("Klever"),
                         // A displayable program name string.
                         i18nc("@title", "Klever"),
                         // The program version string.
                         QStringLiteral(KLEVER_VERSION_STRING),
                         // Short description of what the app does.
                         i18n("Application Description"),
                         // The license this code is released under.
                         KAboutLicense::GPL,
                         // Copyright Statement.
                         i18n("(c) %{CURRENT_YEAR}"));
    aboutData.addAuthor(i18nc("@info:credit", "%{AUTHOR}"),
                        i18nc("@info:credit", "Author Role"),
                        QStringLiteral("%{EMAIL}"),
                        QStringLiteral("https://yourwebsite.com"));
    KAboutData::setApplicationData(aboutData);

    QQmlApplicationEngine engine;

    auto config = KleverConfig::self();
    QObject::connect(config, &KleverConfig::storagePathChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::categoryDisplayNameChanged, config, &KleverConfig::save);

    qmlRegisterType<QmlLinker>("qtMdEditor", 1, 0, "QmlLinker");

    qmlRegisterSingletonInstance("org.kde.Klever", 1, 0, "Config", config);

    AboutType about;
    qmlRegisterSingletonInstance("org.kde.Klever", 1, 0, "AboutType", &about);

    App application;
    qmlRegisterSingletonInstance("org.kde.Klever", 1, 0, "App", &application);

    StorageHandler storageHandler;
    qmlRegisterSingletonInstance<StorageHandler>("org.kde.Klever", 1, 0, "StorageHandler", &storageHandler);

    KleverUtility kleverUtility;
    qmlRegisterSingletonInstance<KleverUtility>("org.kde.Klever", 1, 0, "KleverUtility", &kleverUtility);

    DocumentHandler documentHandler;
    qmlRegisterSingletonInstance<DocumentHandler>("org.kde.Klever", 1, 0, "DocumentHandler", &documentHandler);

    MDHandler mdHandler;
    qmlRegisterSingletonInstance<MDHandler>("org.kde.Klever", 1, 0, "MDHandler", &mdHandler);

    View view;
    qmlRegisterSingletonInstance<View>("org.kde.Klever", 1, 0, "View", &view);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:/contents/ui/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
