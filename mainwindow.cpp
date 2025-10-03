#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->loadImageButton, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveImage);
    connect(ui->applyGrayButton, &QPushButton::clicked, this, &MainWindow::applyGray);
    connect(ui->applyInvertButton, &QPushButton::clicked, this, &MainWindow::applyInvert);
    connect(ui->applyBlurButton, &QPushButton::clicked, this, &MainWindow::applyBlur);
    connect(ui->applyRotateButton, &QPushButton::clicked, this, &MainWindow::applyRotate);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::on_resetButton_clicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadImage() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");

    if (!fileName.isEmpty()) {
        if (!originalImage.loadNewImage(fileName.toStdString())) {
            QMessageBox::warning(this, "Error", "Failed to load image.");
            return;
        }
        filteredImage = originalImage;  // copy original
        showImages();
    }
}

void MainWindow::saveImage() {
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Failed to Save image.");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save Image", "", "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)");

    if (!fileName.isEmpty()) {
        if (!filteredImage.saveImage(fileName.toStdString())) {
            QMessageBox::warning(this, "Error", "Failed to save image.");
        }
    }
}

void MainWindow::applyGray() {
    filteredImage.invert();
    showImages();
}

void MainWindow::applyInvert() {
    filteredImage.invert();
    showImages();
}

void MainWindow::applyBlur() {
    filteredImage.blur(5,1.0);
    showImages();
}

void MainWindow::applyRotate() {
    filteredImage.rotate90();
    showImages();
}

void MainWindow::showImages() {
    static bool originalShown = false;
    if (!originalShown && originalImage.imageData != nullptr) {
        ui->originalLabel->setPixmap(QPixmap::fromImage(originalImage.toQImage())
            .scaled(ui->originalLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        originalShown = true;
    }

    if (filteredImage.imageData != nullptr)
        ui->filteredLabel->setPixmap(QPixmap::fromImage(filteredImage.toQImage())
            .scaled(ui->filteredLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}




void MainWindow::on_resetButton_clicked()
{
    filteredImage = originalImage;
    showImages();
}

