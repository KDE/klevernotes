// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "cliHelper.h"

#include <QProcess>
#include <QStandardPaths>

QString CLIHelper::findExecutable(const QString &command)
{
    return QStandardPaths::findExecutable(command);
}

QString CLIHelper::execCommand(const QString &input)
{
    QProcess process;

    const QString sh = findExecutable(QStringLiteral("sh"));
    if (sh.isEmpty()) {
        return {};
    }

    // We let sh handle the pipes and all the args, easier than using multiple QProcess
    process.start(sh, QStringList() << QStringLiteral("-c") << input);

    if (!process.waitForStarted()) {
        return {};
    }

    process.waitForFinished(5000); // if it takes more then 5 secs, there's a problem !

    return process.exitCode() == 0 ? QString::fromUtf8(process.readAllStandardOutput()) : QLatin1String();
}

bool CLIHelper::commandExists(const QString &command)
{
    return !findExecutable(command).isEmpty();
}
