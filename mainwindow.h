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
#include <queue>
#include <stack>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QActionGroup>
#include <QPropertyAnimation>


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
    void undo();
    void redo();


    void on_resetButton_clicked();

private:
    Ui::MainWindow *ui;
    QPixmap currentImage;
    bool adjustPanelVisible = false;

    // Optional (for slide animation)
    QPropertyAnimation *panelAnimation;

    void setupConnections(); // Helper to connect signals and slots
    void updateImageDisplay(const QPixmap &pix);
    Image originalImage;
    Image filteredImage;
    QImage filtered;
    bool originalShown = false;

    void setupToolBar();

    QToolBar *filterToolBar;
    QMenu *filtersMenu;
    QMenu *effectsMenu;

    void showImages();
    std::queue<Image> recent;
    std::stack<QImage> undoStack;
    std::stack<QImage> redoStack;
    // recent
    // push, pop, top
};

#endif // MAINWINDOW_H
