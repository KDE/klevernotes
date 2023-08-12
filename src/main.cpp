/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
*/

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtQml>
#include <QtWebEngine/QtWebEngine>

#include "app.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "kleverconfig.h"
#include "logic/documentHandler.h"
#include "logic/kleverUtility.h"
#include "logic/mdHandler.h"
#include "logic/qmlLinker.h"
#include "logic/todoHandler.h"
#include "logic/treeview/noteTreeModel.h"

#include "logic/painting/pressureequation.h"
#include "logic/painting/sketchmodel.h"
#include "logic/painting/sketchserializer.h"
#include "logic/painting/sketchview.h"
#include "logic/painting/strokeitem.h"
#include "logic/painting/strokelistitem.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QtWebEngine::initialize();
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setApplicationName(QStringLiteral("KleverNotes"));

    KLocalizedString::setApplicationDomain("klevernotes");

    KAboutData aboutData(
        // The program name used internally.
        QStringLiteral("KleverNotes"),
        // A displayable program name string.
        i18nc("@title", "KleverNotes"),
        // The program version string.
        QStringLiteral("1.0"),
        // Short description of what the app does.
        i18n("Application Description"),
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
    QObject::connect(config, &KleverConfig::storagePathChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::defaultCategoryNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::defaultGroupNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::defaultNoteNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::categoryDisplayNameChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewBodyColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewTextColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewTitleColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewLinkColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewVisitedLinkColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::viewCodeColorChanged, config, &KleverConfig::save);
    QObject::connect(config, &KleverConfig::stylePathChanged, config, &KleverConfig::save);

    qmlRegisterType<QmlLinker>("qtMdEditor", 1, 0, "QmlLinker");

    qmlRegisterSingletonInstance("org.kde.Klever", 1, 0, "Config", config);

    qmlRegisterSingletonType("org.kde.Klever", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });

    App application;
    qmlRegisterSingletonInstance("org.kde.Klever", 1, 0, "App", &application);

    TodoHandler todoHandler;
    qmlRegisterSingletonInstance<TodoHandler>("org.kde.Klever", 1, 0, "TodoHandler", &todoHandler);

    KleverUtility kleverUtility;
    qmlRegisterSingletonInstance<KleverUtility>("org.kde.Klever", 1, 0, "KleverUtility", &kleverUtility);

    DocumentHandler documentHandler;
    qmlRegisterSingletonInstance<DocumentHandler>("org.kde.Klever", 1, 0, "DocumentHandler", &documentHandler);

    MDHandler mdHandler;
    qmlRegisterSingletonInstance<MDHandler>("org.kde.Klever", 1, 0, "MDHandler", &mdHandler);

    qmlRegisterType<NoteTreeModel>("org.kde.Klever", 1, 0, "NoteTreeModel");

    qRegisterMetaType<StrokeSample>();
    qmlRegisterUncreatableType<StrokeSample>("WashiPad", 1, 0, "StrokeSample", "Use the createSample function on SketchViewHandler instead");

    qRegisterMetaType<Stroke>();
    qRegisterMetaType<Stroke::Type>();
    qmlRegisterUncreatableType<Stroke>("WashiPad", 1, 0, "Stroke", "Use the createStroke function on SketchViewHandler instead");

    qRegisterMetaType<Event>();
    qmlRegisterUncreatableType<Event>("WashiPad", 1, 0, "Event", "They are provided by the SketchViewHandler");

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
