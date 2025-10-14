#include "imageview.h"

ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent),
      scene(new QGraphicsScene(this)),
      originalItem(new QGraphicsPixmapItem()),
      filteredItem(new QGraphicsPixmapItem()) {

    setScene(scene);
    scene->addItem(originalItem);
    scene->addItem(filteredItem);

    setRenderHint(QPainter::SmoothPixmapTransform);
    setAlignment(Qt::AlignCenter);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    originalItem->setVisible(false); // start hidden
}

void ImageView::setOriginalImage(const QImage &image) {
    originalItem->setPixmap(QPixmap::fromImage(image));
    fitView();
}

void ImageView::setFilteredImage(const QImage &image) {
    filteredItem->setPixmap(QPixmap::fromImage(image));
    fitView();
}

void ImageView::clearImages() {
    scene->clear();
    scene->addItem(originalItem);
    scene->addItem(filteredItem);
}

void ImageView::fitView() {
    if (filteredItem && !filteredItem->pixmap().isNull()) {
        scene->setSceneRect(filteredItem->boundingRect());
        fitInView(filteredItem, Qt::KeepAspectRatio);
    } else if (originalItem && !originalItem->pixmap().isNull()) {
        scene->setSceneRect(originalItem->boundingRect());
        fitInView(originalItem, Qt::KeepAspectRatio);
    }
}

void ImageView::wheelEvent(QWheelEvent *event) {
    const double factor = event->angleDelta().y() > 0 ? 1.25 : 0.8;
    scale(factor, factor);
    scaleFactor *= factor;
}

void ImageView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        filteredItem->setVisible(false);
        originalItem->setVisible(true);
    }
    QGraphicsView::keyPressEvent(event);
}

void ImageView::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        originalItem->setVisible(false);
        filteredItem->setVisible(true);
    }
    QGraphicsView::keyReleaseEvent(event);
}

void ImageView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    fitView();
}
