#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "find_trigrams_for_dir.h"
#include "find_string_on_dir.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <iostream>
#include <ctime>
#include <QDebug>
#include <QCheckBox>
#include <algorithm>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QKeyEvent>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), dir_path(""), given_string(false) {
    ui->setupUi(this);
    future_for_index.finish_index = false;

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    QCommonStyle style;
    ui->actionIndex_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));

    connect(ui->actionIndex_Directory, &QAction::triggered, this, &main_window::index_directory);
    connect(ui->actionFind_string_on_directory, SIGNAL(clicked()), this, SLOT(try_find_string()));

    connect(&future_for_index.watcher, SIGNAL(finished()), this, SLOT(index_finished()));
    connect(&future_for_search.watcher, SIGNAL(finished()), this, SLOT(search_finished()));
}

void main_window::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter) {
        try_find_string();
    }
}

void main_window::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Tab) {
        ui->actionFind_string_on_directory->click();
    }
}

main_window::~main_window() {
    close();
}

void main_window::index_finished() {
    if (!thread_run.index && !thread_run.find_string) {
        QWidget::close();
        return;
    }

    if (!future_for_index.watcher.result().first) {
        qDebug() << "finish with error";
        return;
    }

    qDebug() << "main_window::index_finished";
    trigrams = future_for_index.watcher.result().second;

    future_for_index.finish_index = true;

    if (given_string) {
        find_string();
    }
}

void main_window::stop_indexation(std::atomic_bool &index_run) {
    index_run = false;
    future_for_index.finish_index = false;
}

void main_window::index_directory() {
    stop_indexation(thread_run.index);

    QString dir;
    if (dir_path.size() > 0) {
        dir = QFileDialog::getExistingDirectory(this, "Select Directory for Indexation", dir_path,
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    } else {
        dir = QFileDialog::getExistingDirectory(this, "Select Directory for Indexation",
                                                        QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }

    if (dir.isEmpty()) {
        // todo
        return;
    }

    dir_path = dir;

    ui->treeWidget->clear();

    thread_run.index = true;

    future_for_index.watcher.setFuture(QtConcurrent::run(my::find_trigrams, dir_path, std::ref(thread_run.index)));
}

void main_window::search_finished() {
    if (!thread_run.index && !thread_run.find_string) {
        QWidget::close();
        return;
    }

    if (!future_for_search.watcher.result().first) {
        qDebug() << "search finished with cancel";
        return;
    }

    files_with_string = future_for_search.watcher.result().second;

    QDir base(dir_path);

    for (int i = 0; i < files_with_string.size(); ++i) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, QString::number(i + 1));
        item->setText(1, base.relativeFilePath(files_with_string[i]));

        QFileInfo cur = QFileInfo(files_with_string[i]);
        item->setText(2, QString::number(cur.size() / 1000.0));

        ui->treeWidget->addTopLevelItem(item);
    }
}

void main_window::find_string() {
    thread_run.find_string = true;

    future_for_search.watcher.setFuture(QtConcurrent::run
                                        (my_find_string::find_string, stringForSearch, trigrams, std::ref(thread_run.find_string)));
}

void main_window::try_find_string() {
    ui->treeWidget->clear();

    stop_find_string(thread_run.find_string);

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::Stretch);

    stringForSearch = ui->StringForSearch->text();

    if (stringForSearch.size() < 3) {
        // todo
        return;
    }

    given_string = true;

    if (future_for_index.finish_index) {
        find_string();
    }
}

void main_window::stop_find_string(std::atomic_bool &find_string_run) {
    find_string_run = false;
    given_string = false;
}

void main_window::close() {
    stop_indexation(thread_run.index);
    stop_find_string(thread_run.find_string);
    //QWidget::close();
}

