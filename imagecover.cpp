#include "imagecover.h"
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

#include <QApplication>
#include <QPixmap>
#include <QMessageBox>
#include <QString>
#include <QDir>
#include <QDateTime>
#include <QFileDialog>

ImageCover::ImageCover(QWidget *parent) :
    QLabel(parent)
{
    setAcceptDrops(true);

    actionChooseImage = new QAction( QIcon("://images/list-add.png"), tr("&Choose image"),this);
    actionSetNoImage  = new QAction( QIcon("://images/list-remove.png"), tr("Clea&n image"),this);
    cMenu.addAction(actionChooseImage);
    cMenu.addAction(actionSetNoImage);

    QObject::connect(actionChooseImage, SIGNAL(triggered()),
                    this, SLOT(chooseImage())  );
    QObject::connect(actionSetNoImage, SIGNAL(triggered()),
                    this, SLOT(noImage())  );

    emit noImage();
}

QString ImageCover::getImagePath() const{
    return imagePath;
}
void ImageCover::setImagePath( QString path ){
    imagePath = path;
}

QSize ImageCover::sizeHint() const
{
    return QSize( 194 , 582 );
//    return QSize( height()/3, height() );
}

void ImageCover::chooseImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open picture"), QDir::homePath(), tr("Images")+" (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG)");
//    QString fileName = QFileDialog::getOpenFileName()
    if( !fileName.isEmpty() ){
        setImagePath( fileName );
        setPixmap( QPixmap( fileName ) );
    }
}

void ImageCover::noImage()
{
    imagePath.clear();
    QPixmap p( "://images/DropHere.png" );
    this->setPixmap(p);
}

/*virtual*/ void ImageCover::dragEnterEvent(QDragEnterEvent* pe)
{
    if( pe->mimeData()->hasUrls() ){
        pe->acceptProposedAction();
    }
}

/*virtual*/ void ImageCover::dropEvent(QDropEvent* pe)
{
    QList<QUrl> urlList = pe->mimeData()->urls();

    QDir objQdir;
    QString coverPath( QDir::homePath() + "/."+QApplication::organizationName()+"/"+QApplication::applicationName() + "/animeCovers/" );
    if( objQdir.mkpath( coverPath ) ){
        setImagePath( urlList.at(0).toLocalFile() );
        this->setPixmap( QPixmap( urlList.at(0).toLocalFile() ) );
    }else{
        QMessageBox::warning( this, tr("Warning"), tr("It was not succeeded to load the picture") );
    }
}

void ImageCover::mouseDoubleClickEvent(QMouseEvent *)
{
    emit chooseImage();
}

void ImageCover::contextMenuEvent(QContextMenuEvent *pe)
{
    cMenu.exec( pe->globalPos() );
}

void ImageCover::resizeEvent(QResizeEvent *)
{
//    e->oldSize()
//    QMessageBox::information(this, windowTitle(), QString::number(this->height()) );
//    resize( height()/3, height() );
}
