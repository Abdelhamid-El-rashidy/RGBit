/*
FCAI – OOP – 2025 - Assignment 1 - Winged Dragon Competition
Dr. Mohamed ElRamly
------------------------------------------------------------------------------
Authors Info: Abdelhamid Ahmed => 20240728, S8
                Anas Ibrahim => 20241017, S22
                Hazem Mahmoud => 20240153, S8

Note:
- You will find needed resources like installation steps, report, and video demo links in Readme file.
- There are some functionalites still in progress like undo, redo. They will be ready in final submission.
*/

/**
 * @File  : main.cpp
 * @brief : This file contains the declaration of the Image class.
 *
 * @authors : Abdelhamid Ahmed, Hazem Mahmoud, Anas Ibrahim,
 * @copyright : FCAI Cairo University
 * @version   : v1.0
 * @date      : 14/10/2025
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




 //// new