#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <memory>
#include <QHash>
#include <vector>
#include <QString>
#include <QFutureWatcher>
#include <QFuture>
#include <atomic>
#include <ctime>
#include <cmath>
#include <QElapsedTimer>

namespace Ui {
    class MainWindow;
}

struct threads {
    std::atomic_bool index;
    std::atomic_bool find_string;

    threads() : index(false), find_string(false) {}
};

struct for_index {
    QFutureWatcher<QPair<bool, QHash<QString, std::vector<int>> > > watcher;
    std::atomic_bool finish_index;
};

struct for_search {
    QFutureWatcher<QPair<bool, QVector<QString>> > watcher;
};

struct my_time_t {
    QElapsedTimer index;
    QElapsedTimer find_string;
};

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);
    ~main_window();
    bool get_index_run() {
        return thread_run.index;
    }

    bool get_find_string_run() {
        return thread_run.find_string;
    }

private slots:
    void index_directory();
    void try_find_string();
    void index_finished();
    void search_finished();
    void close();
    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void select_file(QTreeWidgetItem *item, int column);
    void cancel_index();
    void cancel_find_string();
    //void progressBar_index();
    //void progressBar_find_string();

private:
    void stop_indexation(std::atomic_bool & index_run);
    void stop_find_string(std::atomic_bool & find_string_trun);
    void find_string();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    QString dir_path;
    QHash<QString, std::vector<int>> trigrams;
    threads thread_run;
    for_index future_for_index;
    for_search future_for_search;
    bool given_string;
    QString string_for_search;
    QVector<QString> files_with_string;
    bool want_to_close;
    my_time_t timer;
    bool flag_want_to_find_string;
};

#endif // MAINWINDOW_H
