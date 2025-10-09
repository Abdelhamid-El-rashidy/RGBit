#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setupToolBar();

    connect(ui->loadImageButton, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveImage);
    connect(ui->applyGrayButton, &QPushButton::clicked, this, &MainWindow::applyGray);
    connect(ui->applyInvertButton, &QPushButton::clicked, this, &MainWindow::applyInvert);
    connect(ui->applyBlurButton, &QPushButton::clicked, this, &MainWindow::applyBlur);
    connect(ui->applyRotateButton, &QPushButton::clicked, this, &MainWindow::applyRotate);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::on_resetButton_clicked);
    ui->horizontalSlider->hide();
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
        originalShown = false;
        filteredImage = originalImage;
        showImages();
    }
    filtered = filteredImage.toQImage();
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
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    filteredImage.frame(10,{180,75,120});
    showImages();
}

void MainWindow::applyInvert() {
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    filteredImage.invert();
    showImages();
}

void MainWindow::applyBlur() {
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }

    filteredImage.blur(5,4);
    showImages();
}

void MainWindow::applyRotate() {
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    filteredImage.rotate270();
    showImages();
}

void MainWindow::showImages() {
    if (!originalShown && originalImage.imageData != nullptr) {
        ui->originalLabel->setPixmap(QPixmap::fromImage(originalImage.toQImage())
            .scaled(ui->originalLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        originalShown = true;
    }

    if (filteredImage.imageData != nullptr) {
        undoStack.push(filtered);
        filtered = filteredImage.toQImage();
        ui->filteredLabel->setPixmap(QPixmap::fromImage(filtered)
            .scaled(ui->filteredLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

}

void MainWindow::undo() {
    if (undoStack.empty()) {
        QMessageBox::warning(this, "Error", "Undo Stack is empty.");
        return;
    }
    filtered = undoStack.top();
    undoStack.pop();
    redoStack.push(filtered);
}

void MainWindow::redo() {
    if (redoStack.empty()) {
        QMessageBox::warning(this, "Error", "Redo Stack is empty.");
        return;
    }
    filtered = redoStack.top();
    redoStack.pop();
    undoStack.push(filtered);
}


void MainWindow::on_resetButton_clicked()
{
    filteredImage = originalImage;
    showImages();
}

void MainWindow::setupToolBar() {
    filterToolBar = new QToolBar("Filters", this);
    filterToolBar->setMovable(false);
    addToolBar(Qt::TopToolBarArea, filterToolBar);

    // Create menus
    filtersMenu = new QMenu("Filters", this);
    filtersMenu->addAction("Grayscale", this, &MainWindow::applyGray);
    filtersMenu->addAction("Blur", this, &MainWindow::applyBlur);
    filtersMenu->addAction("Skew", this, [](){ /* TODO */ });
    filtersMenu->addAction("Rotate", this, &MainWindow::applyRotate);
    filtersMenu->addAction("Resize", this, [](){ /* TODO */ });
    filtersMenu->addAction("Merge", this, [](){ /* TODO */ });
    filtersMenu->addAction("Flip Horizontal", this, [](){ /* TODO */ });
    filtersMenu->addAction("Flip Vertical", this, [](){ /* TODO */ });
    filtersMenu->addAction("Black & White", this, [](){ /* TODO */ });
    filtersMenu->addAction("Frame", this, [](){ /* TODO */ });
    filtersMenu->addAction("Invert", this, &MainWindow::applyInvert);

    effectsMenu = new QMenu("Effects", this);
    effectsMenu->addAction("Oil Painting", this, [](){ /* TODO */ });
    effectsMenu->addAction("Sunlight", this, [](){ /* TODO */ });
    effectsMenu->addAction("TV", this, [](){ /* TODO */ });
    effectsMenu->addAction("Infrared", this, [](){ /* TODO */ });
    effectsMenu->addAction("Purple", this, [](){ /* TODO */ });

    // Create tool buttons
    QToolButton *filtersButton = new QToolButton(this);
    filtersButton->setText("Filters");
    filtersButton->setMenu(filtersMenu);
    filtersButton->setPopupMode(QToolButton::InstantPopup);

    QToolButton *effectsButton = new QToolButton(this);
    effectsButton->setText("Effects");
    effectsButton->setMenu(effectsMenu);
    effectsButton->setPopupMode(QToolButton::InstantPopup);

    // Add to toolbar
    filterToolBar->addWidget(filtersButton);
    filterToolBar->addWidget(effectsButton);

    // Add spacing or stretch
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    filterToolBar->addWidget(spacer);
}



