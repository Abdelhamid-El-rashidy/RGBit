/*
FCAI – OOP – 2025 - Assignment 1 - Winged Dragon Competition
Dr. Mohamed ElRamly
------------------------------------------------------------------------------
Authors Info: Abdelhamid Ahmed => 20240728, S8
                Anas Ibrahim => 20241017, S22
                Hazem Mahmoud => 20240153, S8

Note: You will find needed resources like installation steps, report, and video demo links in Readme file.
*/

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
