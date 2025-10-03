#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QLabel>
#include <QPaintEvent>

#include "Image_Class.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadImage();
    void saveImage();
    void applyGray();
    void applyInvert();
    void applyBlur();
    void applyRotate();


    void on_resetButton_clicked();

private:
    Ui::MainWindow *ui;
    Image originalImage;
    Image filteredImage;

    void showImages();
};

#endif // MAINWINDOW_H
