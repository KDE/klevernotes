// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "cliHelper.h"

#include <KSandbox>
#include <QProcess>
#include <QStandardPaths>

QString CLIHelper::findExecutable(const QString &command)
{
    if (KSandbox::isInside()) {
        QProcess process;
        process.setProgram(QStringLiteral("which"));
        process.setArguments(QStringList() << command);
        KSandbox::startHostProcess(process);

        if (!process.waitForStarted()) {
            return {};
        }

        process.waitForFinished(1000);

        return process.exitCode() == 0 ? QString::fromUtf8(process.readAllStandardOutput()).trimmed() : QLatin1String();
    }

    return QStandardPaths::findExecutable(command);
}

QString CLIHelper::execCommand(const QString &input)
{
    const QString sh = findExecutable(QStringLiteral("sh"));
    if (sh.isEmpty()) {
        return {};
    }

    QProcess process;
    // We let sh handle the pipes and all the args, easier than using multiple QProcess
    process.setProgram(sh);
    process.setArguments(QStringList() << QStringLiteral("-c") << input);
    KSandbox::startHostProcess(process);

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
