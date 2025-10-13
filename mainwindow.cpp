#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>


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


    ui->originalLabel->setVisible(false);
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
        if (!originalShown && originalImage.imageData != nullptr) {
            ui->originalLabel->setPixmap(QPixmap::fromImage(originalImage.toQImage())
                .scaled(ui->originalLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            originalShown = true;
        }
        filtered = filteredImage.toQImage();
        ui->filteredLabel->setPixmap(QPixmap::fromImage(filtered)
            .scaled(ui->filteredLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
    if (!originalShown && originalImage.imageData != nullptr) {
        ui->originalLabel->setPixmap(QPixmap::fromImage(originalImage.toQImage())
            .scaled(ui->originalLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        originalShown = true;
    }

    if (filteredImage.imageData != nullptr) {
        filtered = filteredImage.toQImage();
        ui->filteredLabel->setPixmap(QPixmap::fromImage(filtered)
            .scaled(ui->filteredLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
void MainWindow::showImagesOrg() {
        if (!originalShown && originalImage.imageData != nullptr) {
            ui->originalLabel->setPixmap(QPixmap::fromImage(originalImage.toQImage()));
            originalShown = true;
        }

        if (filteredImage.imageData != nullptr) {
            filtered = filteredImage.toQImage();
            ui->filteredLabel->setPixmap(QPixmap::fromImage(filtered));
            ui->filteredLabel->adjustSize();
        }
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
    ui->filteredLabel->setPixmap(QPixmap::fromImage(filtered)
        .scaled(ui->filteredLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
    ui->filteredLabel->setPixmap(QPixmap::fromImage(filtered)
        .scaled(ui->filteredLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        if (originalImage.imageData != nullptr && filteredImage.imageData != nullptr) {
            ui->originalLabel->setVisible(true);
            ui->filteredLabel->setVisible(false);
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        if (originalImage.imageData != nullptr && filteredImage.imageData != nullptr) {
            ui->originalLabel->setVisible(false);
            ui->filteredLabel->setVisible(true);
        }
    }
    QMainWindow::keyReleaseEvent(event);
}


void MainWindow::on_resetButton_clicked()
{
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

    // Apply consistent button style to the box
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
            filteredImage.resize(newWidth, newHeight);
            showImagesOrg();
            QMessageBox::information(this, "Done", "Image resized successfully!");
        } catch (...) {
            QMessageBox::critical(this, "Error", "Failed to resize image.");
        }
    }
}


void MainWindow::on_blur_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);
    filteredImage.blur(4,5);
    showImages();
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
    undoStack.push(filteredImage);

    // Create dialog
    QDialog dialog(this);
    dialog.setWindowTitle("Add Frame");
    dialog.setModal(true);
    dialog.setFixedSize(300, 250);

    // Apply modern dark theme QSS
    dialog.setStyleSheet(MODERN_DIALOG_QSS);

    auto *layout = new QVBoxLayout(&dialog);

    // Border size input
    auto *sizeLayout = new QHBoxLayout();
    auto *sizeLabel = new QLabel("Border size:");
    auto *sizeEdit = new QLineEdit();
    sizeEdit->setValidator(new QIntValidator(1, 200, &dialog));
    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(sizeEdit);
    layout->addLayout(sizeLayout);

    // RGB inputs
    QHBoxLayout *rgbLayout = new QHBoxLayout();
    QLabel *rLabel = new QLabel("R:");
    QLabel *gLabel = new QLabel("G:");
    QLabel *bLabel = new QLabel("B:");
    QLineEdit *rEdit = new QLineEdit();
    QLineEdit *gEdit = new QLineEdit();
    QLineEdit *bEdit = new QLineEdit();

    rEdit->setValidator(new QIntValidator(0, 255, &dialog));
    gEdit->setValidator(new QIntValidator(0, 255, &dialog));
    bEdit->setValidator(new QIntValidator(0, 255, &dialog));

    rgbLayout->addWidget(rLabel);
    rgbLayout->addWidget(rEdit);
    rgbLayout->addWidget(gLabel);
    rgbLayout->addWidget(gEdit);
    rgbLayout->addWidget(bLabel);
    rgbLayout->addWidget(bEdit);
    layout->addLayout(rgbLayout);

    // Buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Cancel");
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);
    layout->addLayout(buttonsLayout);

    // Connections
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        bool ok1, ok2, ok3, ok4;
        int size = sizeEdit->text().toInt(&ok1);
        int r = rEdit->text().toInt(&ok2);
        int g = gEdit->text().toInt(&ok3);
        int b = bEdit->text().toInt(&ok4);

        if (!ok1 || !ok2 || !ok3 || !ok4) {
            QMessageBox::warning(&dialog, "Invalid Input", "Please fill all fields correctly.");
            return;
        }

        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            QMessageBox::warning(&dialog, "Invalid RGB", "RGB values must be between 0 and 255.");
            return;
        }

        filteredImage.frame(size, {r, g, b});
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


void MainWindow::on_skewright_clicked()
{
    if (originalImage.imageData == nullptr) {
        QMessageBox::warning(this, "Error", "Load Image to apply filter");
        return;
    }
    undoStack.push(filteredImage);

    filteredImage.skew(45,1);
    showImages();
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

