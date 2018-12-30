#include "mainwindow.h"
#include <QApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    main_window w;
    w.show();

   /*
   std::pair<std::vector<std::vector<QString>>, std::vector<QString>> ans = my::group_identical_files("/home/ira/Desktop/VeryBigtest");

   cout << "symlinks:" << endl;
   for (auto i : ans.second) {
        cout << i.toStdString() << endl;
   }

    std::cout << "equals" << std::endl;

    for (size_t i = 0; i < ans.first.size(); ++i) {
        std::cout << "group " << i << ":" << std::endl;
        for (auto j : ans.first[i]) {
            std::cout << j.toStdString() << std::endl;
        }
        std::cout << "end of group " << i << std::endl << std::endl;
    }

    return 0;
    */

    return a.exec();
}
