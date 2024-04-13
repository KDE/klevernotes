/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
*/

#include <QApplication>
#include <QQmlApplicationEngine>

// Do not remove this, breaks Qt6 build
#include <QtQml>

#include <QtWebEngineQuick>

#include "app.h"
#include "kleverconfig.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "logic/colorschemer.h"
#include "logic/documentHandler.h"
#include "logic/kleverUtility.h"
#include "logic/mdHandler.h"
#include "logic/qmlLinker.h"

#include "logic/treeview/noteTreeModel.h"

#include "logic/painting/pressureequation.h"
#include "logic/painting/sketchmodel.h"
#include "logic/painting/sketchserializer.h"
#include "logic/painting/sketchview.h"
#include "logic/painting/strokeitem.h"
#include "logic/painting/strokelistitem.h"

#include "logic/parser/parser.h"

#include "logic/printing/printingHelper.h"

// Plugins
#include "logic/plugins/emoji/emojiModel.h"
#include "logic/plugins/emoji/emoticonFilterModel.h"
#include "logic/plugins/noteMapper/noteMapper.h"
#include "logic/plugins/syntaxHighlight/highlightHelper.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QtWebEngineQuick::initialize();
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));

    KLocalizedString::setApplicationDomain("klevernotes");

    KAboutData aboutData(
        // The program name used internally.
        QStringLiteral("klevernotes"),
        // A displayable program name string.
        i18nc("@title", "KleverNotes"),
        // The program version string.
        QStringLiteral("1.0"),
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
    QObject::connect(config, &KleverConfig::defaultCategoryNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::defaultGroupNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::defaultNoteNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::editorVisibleChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::previewVisibleChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::useSpaceForTabChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::spacesForTabChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::visibleToolsChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::invisibleToolsChanged, config, &KleverConfig::save);
    // Appearence
    QObject::connect(config, &KleverConfig::editorFontChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::categoryDisplayNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewBodyColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewTextColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewTitleColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewLinkColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewVisitedLinkColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewCodeColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewHighlightColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewFontChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::codeFontChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::stylePathChanged, config, &KleverConfig::save);
    // Plugins
    QObject::connect(config, &KleverConfig::codeSynthaxHighlightEnabledChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::codeSynthaxHighlighterChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::codeSynthaxHighlighterStyleChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::noteMapEnabledChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::quickEmojiEnabledChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::emojiToneChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::quickEmojiDialogEnabledChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::pumlEnabledChanged, config, &KleverConfig::save);

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

    MDHandler mdHandler;
    qmlRegisterSingletonInstance<MDHandler>("org.kde.Klever", 1, 0, "MDHandler", &mdHandler);

    ColorSchemer colorScheme;
    qmlRegisterSingletonInstance<ColorSchemer>("org.kde.Klever", 1, 0, "ColorSchemer", &colorScheme);

    qmlRegisterType<PrintingUtility>("org.kde.Klever", 1, 0, "PrintingUtility");

    qmlRegisterType<Parser>("org.kde.Klever", 1, 0, "Parser");
    qmlRegisterType<NoteTreeModel>("org.kde.Klever", 1, 0, "NoteTreeModel");

    // === PLUGINS ===
    qmlRegisterType<EmoticonFilterModel>("org.kde.neochat", 1, 0, "EmoticonFilterModel");
    qmlRegisterSingletonInstance("org.kde.neochat", 1, 0, "EmojiModel", &EmojiModel::instance());

    qmlRegisterType<NoteMapper>("org.kde.Klever", 1, 0, "NoteMapper");

    HighlightHelper highlightHelper;
    qmlRegisterSingletonInstance<HighlightHelper>("org.kde.Klever", 1, 0, "HighlightHelper", &highlightHelper);
    // === === === ===

    qRegisterMetaType<StrokeSample>();
    qRegisterMetaType<Stroke>();
    qRegisterMetaType<Stroke::Type>();
    qRegisterMetaType<Event>();

    // Will complain about valueType names starting with capital letter, but changing it will break things and make CLazy angry
    qmlRegisterUncreatableType<StrokeSample>("WashiPad", 1, 0, "StrokeSample", QStringLiteral("Use the createSample function on SketchViewHandler instead"));
    qmlRegisterUncreatableType<Stroke>("WashiPad", 1, 0, "Stroke", QStringLiteral("Use the createStroke function on SketchViewHandler instead"));
    qmlRegisterUncreatableType<Event>("WashiPad", 1, 0, "Event", QStringLiteral("They are provided by the SketchViewHandler"));

    qmlRegisterType<PressureEquation>("WashiPad", 1, 0, "PressureEquation");

    qmlRegisterType<SketchViewHandler>("WashiPad", 1, 0, "SketchViewHandler");

    qmlRegisterType<SketchModel>("WashiPad", 1, 0, "SketchModel");
    qmlRegisterType<SketchSerializer>("WashiPad", 1, 0, "SketchSerializer");
    qmlRegisterType<StrokeItem>("WashiPad", 1, 0, "StrokeItem");
    qmlRegisterType<StrokeListItem>("WashiPad", 1, 0, "StrokeListItem");

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:/contents/ui/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
