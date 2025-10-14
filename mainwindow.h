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
#include <QEasingCurve>
#include <QIntValidator>




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
    void on_resetButton_clicked();
    void undo();
    void redo();


    void on_rot90_clicked();

    void on_rot180_clicked();

    void on_rot270_clicked();

    void on_flipv_clicked();

    void on_fliph_clicked();

    void on_BW_clicked();

    void on_Sunlight_clicked();

    void on_Invert_clicked();

    void on_Gray_clicked();

    void on_Infrared_clicked();

    void on_Purple_clicked();

    void on_Oil_clicked();

    void on_OldTv_clicked();

    void on_CropBtn_clicked();

    void on_ResizeBtn_clicked();

    void on_blur_clicked();

    void on_Increase_clicked();

    void on_decrease_clicked();

    void on_merge_clicked();



    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_skewleft_clicked();

    void on_skewright_clicked();

    void on_undo_clicked();

    void on_redo_clicked();

    void on_emboss_clicked();

private:
    Ui::MainWindow *ui;
    QLabel *statusLabel;

    QPixmap currentImage;

    // for sidebar
    bool sidebarVisible = false;
    QPropertyAnimation *sidebarAnim;
    void showSidebar(QWidget *page);
    void hideSidebar();

    // Optional (for slide animation)
    QPropertyAnimation *panelAnimation;

    void updateImageDisplay(const QPixmap &pix);
    Image originalImage;
    Image filteredImage;
    QImage filtered;
    bool originalShown = false;


    void showImages();
    std::queue<Image> recent;
    std::stack<Image> undoStack;
    std::stack<Image> redoStack;
    // recent
    // push, pop, top
};

#endif // MAINWINDOW_H
