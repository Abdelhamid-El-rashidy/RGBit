#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QRubberBand>
#include <QMouseEvent>

class ImageView : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);

    void setOriginalImage(const QImage &image);
    void setFilteredImage(const QImage &image);
    void clearImages();
    void showOriginal(bool visible);

    void setCroppingEnabled(bool enabled);


    signals:
        void cropAreaSelected(const QRect &rect);  // signal to notify selection

protected:
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *originalItem;
    QGraphicsPixmapItem *filteredItem;
    double scaleFactor = 1.0;
    bool croppingEnabled = false;


    void fitView();

    QRubberBand *rubberBand = nullptr;
    QPoint origin;
};

#endif
