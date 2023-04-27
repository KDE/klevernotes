// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QModelIndex>
#include <QStandardItemModel>

class MyTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit MyTreeModel(QObject *parent = 0);

    enum Roles {
        DisplayedName = Qt::UserRole,
        Path,
        Level,
        UseCase,
        Icon,
    };

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void makeModel(const QString &path);
    Q_INVOKABLE void removeFromTree(const QModelIndex &rowModelIndex);
    Q_INVOKABLE void renameRow(const QModelIndex &rowModelIndex, const QString &newName);
    Q_INVOKABLE void addRow(const QString &rowPath, const int rowLevel, const QModelIndex &parentModelIndex = QModelIndex());

    Q_INVOKABLE QModelIndex getParentIndex(const QModelIndex &index);

private:
    QStandardItem *getHierarchy(const QString &path, const int lvl);
    QStandardItem *makeRow(const QString &path, const int lvl);
    void changePath(QStandardItem *&row, const QString &newPart, int newPartIdx = -1);
    void reorder();

    QStandardItem *m_basedCategoryObject = nullptr;
    QString m_storagePath;
};
