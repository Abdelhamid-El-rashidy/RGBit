#include "imageview.h"
#include <QApplication>

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

    originalItem->setVisible(false);
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

void ImageView::showOriginal(bool visible) {
    originalItem->setVisible(visible);
    filteredItem->setVisible(!visible);
}

// cropping

void ImageView::mousePressEvent(QMouseEvent *event) {
    if (croppingEnabled && event->button() == Qt::LeftButton) {
        origin = event->pos();
        if (!rubberBand)
            rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->setGeometry(QRect(origin, QSize()));
        rubberBand->show();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void ImageView::mouseMoveEvent(QMouseEvent *event) {
    if (croppingEnabled && rubberBand)
        rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    else
        QGraphicsView::mouseMoveEvent(event);
}

void ImageView::mouseReleaseEvent(QMouseEvent *event) {
    if (croppingEnabled && rubberBand && event->button() == Qt::LeftButton) {
        rubberBand->hide();
        QRect viewRect = rubberBand->geometry();
        QRectF sceneRect = mapToScene(viewRect).boundingRect();
        emit cropAreaSelected(sceneRect.toRect());
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}


void ImageView::setCroppingEnabled(bool enabled) {
    croppingEnabled = enabled;
    setDragMode(enabled ? QGraphicsView::NoDrag : QGraphicsView::ScrollHandDrag);
    if (!enabled && rubberBand) {
        rubberBand->hide();
    }
}

