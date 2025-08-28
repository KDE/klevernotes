/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>
*/

#include <QApplication>
#include <QQmlApplicationEngine>

// Do not remove this, breaks Qt6 build
#include <QQuickStyle>
#include <QtQml>
#include <QtWebEngineQuick>

#include "app.h"
#include "kleverconfig.h"
#include <KAboutData>
#include <KIconTheme>
#include <KLocalizedContext>
#include <KLocalizedString>

#ifdef WITH_BREEZEICONS_LIB
#include <BreezeIcons>
#endif

int main(int argc, char *argv[])
{
    QtWebEngineQuick::initialize();

    KIconTheme::initTheme();
    QIcon::setFallbackThemeName(QStringLiteral("breeze"));

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));

    KLocalizedString::setApplicationDomain("klevernotes");

    KAboutData aboutData(
        // The program name used internally.
        QStringLiteral("klevernotes"),
        // A displayable program name string.
        i18nc("@title", "KleverNotes"),
        // The program version string.
        QStringLiteral("1.2.3"),
        // Short description of what the app does.
        i18n("Note taking and management application"),
        // The license this code is released under.
        KAboutLicense::GPL,
        // Copyright Statement.
        i18n("(c) 2022-2025"));
    aboutData.addAuthor(i18nc("@info:credit", "Louis Schul"),
                        // i18nc("@info:credit", "Author Role"),
                        QStringLiteral("schul9louis@outlook.fr")
                        // QStringLiteral("https://yourwebsite.com")
    );
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    KAboutData::setApplicationData(aboutData);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.klevernotes")));

    QQmlApplicationEngine engine;

    auto config = KleverConfig::self();
    // General
    QObject::connect(config, &KleverConfig::storagePathChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::defaultFolderNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::defaultNoteNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::editorVisibleChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::previewVisibleChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::useSpaceForTabChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::spacesForTabChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::visibleToolsChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::invisibleToolsChanged, config, &KleverConfig::save);
    // Plugins
    // TODO move a maximum of this to plugin helper or editorHandler
    QObject::connect(config, &KleverConfig::codeSynthaxHighlighterChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::noteMapEnabledChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::quickEmojiEnabledChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::emojiToneChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::quickEmojiDialogEnabledChanged, config, &KleverConfig::save);

    qmlRegisterSingletonType("org.kde.klevernotes", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });

    App application;
    qmlRegisterSingletonInstance("org.kde.klevernotes", 1, 0, "App", &application);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.klevernotes", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
