#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QColorDialog>
#include  <QSlider>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QtConcurrent/QtConcurrent>
#include <QLabel>
#include <QPixmap>
#include <QComboBox>



// --- QSS for Modern Dialogs ---
// Note: This style sheet is defined here and applied directly to the QDialog widgets
// to ensure the custom pop-ups match the dark theme and button styles defined in the .ui file.
const QString MODERN_DIALOG_QSS = R"(
    QDialog {
        background-color: #1E1E2F; /* Dark dialog background */
        color: #E0E0E0;
        border: 1px solid #00BCD4; /* Subtle accent border */
        border-radius: 8px;
    }
    QLabel {
        color: #CCCCCC;
    }
    QLineEdit {
        background-color: #28283D; /* Input background */
        border: 1px solid #303045;
        color: #E0E0E0;
        padding: 5px;
        border-radius: 4px;
    }
    QPushButton {
        background-color: #28283D; /* Button background */
        color: #E0E0E0;
        border: none;
        border-radius: 4px;
        padding: 8px;
    }
    QPushButton:hover {
        background-color: #303045; /* Hover (shadow simulation) */
        color: #00BCD4; /* Modern Teal Accent */
        border: 1px solid #00BCD4;
    }
    QPushButton:pressed {
        background-color: #3949AB; /* Deep Indigo Pressed State */
        color: white;
    }
)";


MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // ui->Logo->setPixmap(QPixmap(":/images/logo.png")); // if using resource file
    // ui->Logo->setPixmap(QPixmap("C:\Users\windows 11\Downloads\logo.png"));
    // ui->Logo->setPixmap(QPixmap("logo.png"));
    // ui->Logo->setScaledContents(true); // make it resize with label
    // ui->Logo->setStyleSheet("background: transparent; border: none;");
    // ui->Logo->setPixmap(QPixmap(":/images/logo.png").scaled(
    //     ui->Logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // In your constructor or setup function
    QLabel *logLabel = new QLabel(this);
    logLabel->setFixedSize(300, 200);
    logLabel->setScaledContents(true);
    QPixmap logo(":/images/logo.png");
    logLabel->setPixmap(logo);
    logLabel->setGeometry(50,520,300,200);




    statusLabel = new QLabel("Ready");
    statusLabel->setAlignment(Qt::AlignCenter);
    ui->statusBar->addWidget(statusLabel);
    ui->statusBar->setVisible(false);

    setFocusPolicy(Qt::StrongFocus);



    // --- Sidebar Animation ---
    sidebarAnim = new QPropertyAnimation(ui->sidebar, "geometry", this);
    sidebarAnim->setDuration(300);
    sidebarAnim->setEasingCurve(QEasingCurve::InOutCubic);

    // Initially hidden off-screen
    ui->sidebar->setGeometry(-220, 0, 220, ui->sidebar->height());

    // --- Connect Sidebar Buttons ---
    connect(ui->EffectsBtn, &QPushButton::clicked, [=]() {
        showSidebar(ui->effectsPage);
    });

    connect(ui->TransformBtn, &QPushButton::clicked, [=]() {
        showSidebar(ui->transformPage);
    });
    connect(ui->BrightnessBtn, &QPushButton::clicked, [=]() {
    showSidebar(ui->Brightpage);
});

    // Add more if you make new sidebar pages later
    connect(ui->closeSidebar, &QPushButton::clicked, this, &MainWindow::hideSidebar);


    connect(ui->load, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveImage);
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


void MainWindow::showImages() {
    if (!originalImage.imageData)
        return;

    QImage origQImage = originalImage.toQImage();
    QImage filtQImage = filteredImage.imageData
        ? filteredImage.toQImage()
        : origQImage;

    QSize targetSize = ui->imageView->size();

    QImage resizedOriginal = origQImage.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage resizedFiltered = filtQImage.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->imageView->setOriginalImage(origQImage);
    ui->imageView->setFilteredImage(filtQImage);
}

void MainWindow::undo() {
    if (undoStack.empty()) {
        QMessageBox::warning(this, "Error", "Undo Stack is empty.");
        return;
    }
    redoStack.push(filteredImage);
    filteredImage = undoStack.top();
    undoStack.pop();
    filtered = filteredImage.toQImage();
    showImages();
}

void MainWindow::redo() {
    if (redoStack.empty()) {
        QMessageBox::warning(this, "Error", "Redo Stack is empty.");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage = redoStack.top();
    redoStack.pop();
    filtered = filteredImage.toQImage();
    showImages();
}



void MainWindow::on_resetButton_clicked()
{
    while (!undoStack.empty()) undoStack.pop();
    while (!redoStack.empty()) redoStack.pop();
    filteredImage = originalImage;
    showImages();
}

void MainWindow::showSidebar(QWidget *page)
{
    if (sidebarVisible && ui->sidebarStack->currentWidget() == page)
        return;

    ui->sidebarStack->setCurrentWidget(page);

    QRect endGeom(90, 0, 220, ui->sidebar->height());

    if (!sidebarVisible) {
        ui->sidebar->show();

        sidebarAnim->stop();
        sidebarAnim->disconnect();

        sidebarAnim->setStartValue(QRect(-220, 0, 220, ui->sidebar->height()));
        sidebarAnim->setEndValue(endGeom);
        sidebarAnim->start();

        sidebarVisible = true;
    } else {
        ui->sidebar->setGeometry(endGeom);
    }
}

void MainWindow::hideSidebar()
{
    if (!sidebarVisible)
        return;

    sidebarAnim->stop();
    sidebarAnim->disconnect();

    sidebarAnim->setStartValue(ui->sidebar->geometry());
    sidebarAnim->setEndValue(QRect(-220, 0, 220, ui->sidebar->height()));

    connect(sidebarAnim, &QPropertyAnimation::finished, this, [this]() {
        ui->sidebar->hide();
        sidebarVisible = false;
    });

    sidebarAnim->start();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space && originalImage.imageData) {
        ui->imageView->showOriginal(true);
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space && originalImage.imageData) {
        ui->imageView->showOriginal(false);
    }
    QMainWindow::keyReleaseEvent(event);
}








// Filters
void MainWindow::on_rot90_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.rotate90();
    showImages();
}


void MainWindow::on_rot180_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.rotate180();
    showImages();
}


void MainWindow::on_rot270_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.rotate270();
    showImages();
}


void MainWindow::on_flipv_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.flipV();
    showImages();
}


void MainWindow::on_fliph_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.flipH();
    showImages();
}


void MainWindow::on_BW_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.Black_White();
    showImages();
}


void MainWindow::on_Sunlight_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.sunlight();
    showImages();
}


void MainWindow::on_Invert_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.invert();
    showImages();
}


void MainWindow::on_Gray_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.toGray();
    showImages();
}


void MainWindow::on_Infrared_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.infrared();
    showImages();
}


void MainWindow::on_Purple_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.purple();
    showImages();
}


void MainWindow::on_Oil_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.oilPainting();
    showImages();
}


void MainWindow::on_OldTv_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    Image img("oldtv.jpg");
    filteredImage = img.merge(filteredImage);
    showImages();
}


void MainWindow::on_CropBtn_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load an image before cropping.");
        return;
    }
    undoStack.push(filteredImage);
    QDialog dialog(this);
    dialog.setWindowTitle("Crop Image");
    dialog.setModal(true);
    dialog.setFixedSize(300, 220);

    // Apply modern dark theme QSS
    dialog.setStyleSheet(MODERN_DIALOG_QSS);

    QLineEdit *xEdit = new QLineEdit(&dialog);
    QLineEdit *yEdit = new QLineEdit(&dialog);
    QLineEdit *wEdit = new QLineEdit(&dialog);
    QLineEdit *hEdit = new QLineEdit(&dialog);

    xEdit->setPlaceholderText("Top-left X");
    yEdit->setPlaceholderText("Top-left Y");
    wEdit->setPlaceholderText("Width");
    hEdit->setPlaceholderText("Height");

    xEdit->setValidator(new QIntValidator(0, originalImage.width - 1, &dialog));
    yEdit->setValidator(new QIntValidator(0, originalImage.height - 1, &dialog));
    wEdit->setValidator(new QIntValidator(1, originalImage.width, &dialog));
    hEdit->setValidator(new QIntValidator(1, originalImage.height, &dialog));

    QFormLayout *form = new QFormLayout();
    form->addRow("X:", xEdit);
    form->addRow("Y:", yEdit);
    form->addRow("Width:", wEdit);
    form->addRow("Height:", hEdit);

    QDialogButtonBox *buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    // Apply consistent button style to the box
    buttons->setStyleSheet(MODERN_DIALOG_QSS);

    form->addWidget(buttons);

    dialog.setLayout(form);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        int x = xEdit->text().toInt();
        int y = yEdit->text().toInt();
        int w = wEdit->text().toInt();
        int h = hEdit->text().toInt();

        if (x < 0 || y < 0 || w <= 0 || h <= 0 ||
            x + w > originalImage.width || y + h > originalImage.height) {
            QMessageBox::warning(this, "Invalid Input", "Invalid crop dimensions or coordinates.");
            return;
        }

        filteredImage.crop(x, y, w, h);
        showImages();
    }
}


void MainWindow::on_ResizeBtn_clicked()
{
    if (filteredImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load or apply a filter before resizing.");
        return;
    }
    undoStack.push(filteredImage);

    QDialog dialog(this);
    dialog.setWindowTitle("Resize Image");
    dialog.setFixedSize(250, 150);

    // Apply modern dark theme QSS
    dialog.setStyleSheet(MODERN_DIALOG_QSS);

    QFormLayout *layout = new QFormLayout(&dialog);

    QLineEdit *widthEdit = new QLineEdit(&dialog);
    QLineEdit *heightEdit = new QLineEdit(&dialog);

    widthEdit->setValidator(new QIntValidator(1, 5000, &dialog));
    heightEdit->setValidator(new QIntValidator(1, 5000, &dialog));

    layout->addRow("New Width:", widthEdit);
    layout->addRow("New Height:", heightEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout->addWidget(buttonBox);

    buttonBox->setStyleSheet(MODERN_DIALOG_QSS);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        int newWidth = widthEdit->text().toInt();
        int newHeight = heightEdit->text().toInt();

        if (newWidth <= 0 || newHeight <= 0) {
            QMessageBox::warning(this, "Invalid", "Dimensions must be positive.");
            return;
        }

        try {
            Image resized = filteredImage.resize(newWidth, newHeight);
            filteredImage = resized;
            showImages();
            QMessageBox::information(this, "Done", "Image resized successfully!");
        } catch (...) {
            QMessageBox::critical(this, "Error", "Failed to resize image.");
        }
    }
}


void MainWindow::on_blur_clicked()
{
    if (filteredImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load an image before applying Gaussian Blur.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Gaussian Blur");
    dialog.setModal(true);
    dialog.setFixedSize(320, 180);
    dialog.setStyleSheet(MODERN_DIALOG_QSS);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel("Blur Intensity:");
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(1, 10);       // 1–10 intuitive range
    slider->setValue(3);
    QLabel *valueLabel = new QLabel("3");
    valueLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(label);
    layout->addWidget(slider);
    layout->addWidget(valueLabel);

    QObject::connect(slider, &QSlider::valueChanged, [&](int val) {
        valueLabel->setText(QString::number(val));
    });

    // Buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("Apply");
    QPushButton *cancelButton = new QPushButton("Cancel");
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);
    layout->addLayout(buttonsLayout);

    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        int blurLevel = slider->value();

        // Map blurLevel → (kernelSize, sigma)
        int kernelSize = 2 * blurLevel + 1;
        double sigma = 0.8 * blurLevel;

        ui->statusBar->setVisible(true);
        statusLabel->setText("Processing... Please wait.");
        QApplication::processEvents();

        undoStack.push(filteredImage);

        QFuture<void> future = QtConcurrent::run([=, this]() {
            filteredImage.blur(kernelSize, sigma);
        });

        QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);

        connect(watcher, &QFutureWatcher<void>::finished, this, [=]() {
            showImages();
            statusLabel->setText("Done");
            QApplication::processEvents();

            QTimer::singleShot(1000, this, [=]() {
                ui->statusBar->setVisible(false);
            });

            watcher->deleteLater();
        });

        watcher->setFuture(future);
        dialog.accept();
    });

    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
    ui->statusBar->setVisible(true);

}

void MainWindow::on_Increase_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.toLighten();
    showImages();
}



void MainWindow::on_decrease_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.toDarken();
    showImages();
}


void MainWindow::on_merge_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);

    QString fileName = QFileDialog::getOpenFileName(
    this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");

    Image newImage;
    if (!fileName.isEmpty()) {
        if (!newImage.loadNewImage(fileName.toStdString())) {
            QMessageBox::warning(this, "Error", "Failed to load image.");
            return;
        }
        filteredImage = filteredImage.merge(newImage);
        showImages();
    }
}






void MainWindow::on_pushButton_clicked()
{
    if (filteredImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load an image before adding a frame.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Add Frame");
    dialog.setModal(true);
    dialog.setFixedSize(300, 230);
    dialog.setStyleSheet(MODERN_DIALOG_QSS);

    auto *layout = new QVBoxLayout(&dialog);

    // Size
    auto *sizeLayout = new QHBoxLayout();
    auto *sizeLabel = new QLabel("Border size:");
    auto *sizeEdit = new QLineEdit();
    sizeEdit->setValidator(new QIntValidator(1, 200, &dialog));
    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(sizeEdit);
    layout->addLayout(sizeLayout);

    // dropdown border style
    auto *styleLayout = new QHBoxLayout();
    auto *styleLabel = new QLabel("Frame style:");
    auto *styleCombo = new QComboBox();
    styleCombo->setStyleSheet("background-color: #1F232D;");
    styleCombo->addItems({"Solid", "Dashed", "Dotted"});
    styleLayout->addWidget(styleLabel);
    styleLayout->addWidget(styleCombo);
    layout->addLayout(styleLayout);

    // color
    QPushButton *colorButton = new QPushButton("Choose Frame Color");
    QColor selectedColor = Qt::black;  // default
    layout->addWidget(colorButton);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Cancel");
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);
    layout->addLayout(buttonsLayout);

    QObject::connect(colorButton, &QPushButton::clicked, [&]() {
        QColor color = QColorDialog::getColor(Qt::black, &dialog, "Select Frame Color");
        if (color.isValid()) {
            selectedColor = color;
            colorButton->setText("Color: " + color.name());
            colorButton->setStyleSheet(QString("background-color: %1; color: white;").arg(color.name()));
        }
    });

    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        bool ok;
        int size = sizeEdit->text().toInt(&ok);
        if (!ok || size * 2 >= filteredImage.width) {
            QMessageBox::warning(&dialog, "Invalid Input", "Please enter a valid border size.");
            return;
        }

        QString style = styleCombo->currentText();
        int borderType = 0;  // 0 = Solid, 1 = Dashed, 2 = Dotted
        if (style == "Dashed") borderType = 1;
        else if (style == "Dotted") borderType = 2;

        undoStack.push(filteredImage);

        filteredImage.frame(
            size,
            {selectedColor.red(), selectedColor.green(), selectedColor.blue()},
            borderType
        );

        showImages();
        dialog.accept();
    });

    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
}



void MainWindow::on_pushButton_2_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);

    filteredImage.detect_edges();
    showImages();
}


void MainWindow::on_skewleft_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);

    filteredImage.skew(45,0);
    showImages();
}


void MainWindow::on_skewright_clicked() {
        QWidget *skewWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(skewWidget);



        QComboBox *directionBox = new QComboBox;
        directionBox->addItem("Horizontal");
        directionBox->addItem("Vertical");

    ui->horizontalSlider->setRange(-45,45);

        // QPushButton *applyButton = new QPushButton("Apply Skew");
        //
        //
        // layout->addWidget(directionBox);
        // layout->addWidget(applyButton);
        //
        // skewWidget->setLayout(layout);
        // skewWidget->show();
        //
        // connect(applyButton, &QPushButton::clicked, this, [=]() {
        //     double degree = slider->value();
        //     int dir = directionBox->currentText().toInt();
        //
        //     undoStack.push(filteredImage);
        //     filteredImage.skew(degree, dir);  // Apply your Image class function
        //     showImages();
        // });
}
void MainWindow::on_undo_clicked()
{
    undo();
}


void MainWindow::on_redo_clicked()
{
    redo();
}


void MainWindow::on_emboss_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);

    filteredImage.emboss();
    showImages();
}

void MainWindow::on_swirlbtn_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);

    filteredImage.swirl();
    showImages();
}

void MainWindow::on_burned_clicked() {
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);

    filteredImage.burn(50);
    showImages();
}

void MainWindow::on_moon_clicked() {
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);

    filteredImage.moonlight(50);
    showImages();
}


