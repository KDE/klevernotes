/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>
*/

#include <QApplication>
#include <QQmlApplicationEngine>

// Do not remove this, breaks Qt6 build
#include <QtQml>

#include <QtWebEngineQuick>

#include "app.h"
#include "kleverconfig.h"
#include <KAboutData>
#include <KIconTheme>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "logic/colorschemer.h"
#include "logic/documentHandler.h"
#include "logic/kleverUtility.h"
#include "logic/qmlLinker.h"

#include "logic/editor/editorHandler.hpp"

// #include "logic/treeview/noteTreeModel.h"
#include "logic/treeview/treeModel.h"

#include "logic/printing/printingHelper.h"

#include "logic/preview/styleHandler.hpp"

// Plugins
#include "logic/parser/plugins/emoji/emojiModel.h"
#include "logic/parser/plugins/emoji/emoticonFilterModel.h"
#include "logic/parser/plugins/noteMapper/noteMapper.h"
#include "logic/parser/plugins/syntaxHighlight/highlightHelper.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QtWebEngineQuick::initialize();

    KIconTheme::initTheme();
    QIcon::setFallbackThemeName(QStringLiteral("breeze"));

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));

    KLocalizedString::setApplicationDomain("klevernotes");

    KAboutData aboutData(
        // The program name used internally.
        QStringLiteral("klevernotes"),
        // A displayable program name string.
        i18nc("@title", "KleverNotes"),
        // The program version string.
        QStringLiteral("1.1.1"),
        // Short description of what the app does.
        i18n("Note taking and management application"),
        // The license this code is released under.
        KAboutLicense::GPL,
        // Copyright Statement.
        i18n("(c) 2022-2023"));
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
    // Appearence
    QObject::connect(config, &KleverConfig::categoryDisplayNameChanged, config, &KleverConfig::save);
    // Plugins
    // TODO move a maximum of this to plugin helper or editorHandler
    QObject::connect(config, &KleverConfig::codeSynthaxHighlighterChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::noteMapEnabledChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::quickEmojiEnabledChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::emojiToneChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::quickEmojiDialogEnabledChanged, config, &KleverConfig::save);

    qmlRegisterType<QmlLinker>("qtMdEditor", 1, 0, "QmlLinker");

    qmlRegisterSingletonInstance("org.kde.Klever", 1, 0, "Config", config);

    qmlRegisterSingletonType("org.kde.Klever", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });

    App application;
    qmlRegisterSingletonInstance("org.kde.Klever", 1, 0, "App", &application);

    KleverUtility kleverUtility;
    qmlRegisterSingletonInstance<KleverUtility>("org.kde.Klever", 1, 0, "KleverUtility", &kleverUtility);

    DocumentHandler documentHandler;
    qmlRegisterSingletonInstance<DocumentHandler>("org.kde.Klever", 1, 0, "DocumentHandler", &documentHandler);

    MdEditor::EditorHandler editorHandler;
    qmlRegisterSingletonInstance<MdEditor::EditorHandler>("org.kde.Klever", 1, 0, "EditorHandler", &editorHandler);

    ColorSchemer colorScheme;
    qmlRegisterSingletonInstance<ColorSchemer>("org.kde.Klever", 1, 0, "ColorSchemer", &colorScheme);

    StyleHandler styleHandler;
    qmlRegisterSingletonInstance<StyleHandler>("org.kde.Klever", 1, 0, "StyleHandler", &styleHandler);

    qmlRegisterType<PrintingUtility>("org.kde.Klever", 1, 0, "PrintingUtility");

    qmlRegisterType<NoteTreeModel>("org.kde.Klever", 1, 0, "NoteTreeModel");

    // === PLUGINS ===
    qmlRegisterType<EmoticonFilterModel>("neochatComponents", 1, 0, "EmoticonFilterModel");
    qmlRegisterSingletonInstance("neochatComponents", 1, 0, "EmojiModel", &EmojiModel::instance());

    NoteMapper noteMapper;
    qmlRegisterSingletonInstance<NoteMapper>("org.kde.Klever", 1, 0, "NoteMapper", &noteMapper);

    HighlightHelper highlightHelper;
    qmlRegisterSingletonInstance<HighlightHelper>("org.kde.Klever", 1, 0, "HighlightHelper", &highlightHelper);
    // === === === ===

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:/contents/ui/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
