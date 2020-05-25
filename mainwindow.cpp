#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImageReader>
#include <QInputDialog>
#include <QMessageBox>
#include "Settings.h"
#include <QClipboard>
#include <QMdiSubWindow>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QThread>
#include <QMimeData>
#include <QDesktopServices>
#include <QTimer>
#include <qpainter.h>
namespace
{
    const QString UNTITLED_TAB_NAME = QObject::tr("Untitled");
}
class MdiArea : public QMdiArea
{
public:
    MdiArea(QWidget *parent = 0,QString FileName="")
        :
            QMdiArea(parent),
            m_pixmap(FileName)
    {
    }
protected:
    void paintEvent(QPaintEvent *event)
    {
        QMdiArea::paintEvent(event);

        QPainter painter(viewport());
        this->setFixedWidth(m_pixmap.width());
        this->setFixedHeight(m_pixmap.height());
        // Calculate the logo position - the bottom right corner of the mdi area.
        int x =0;
        int y = 0;
        painter.drawPixmap(x, y, m_pixmap);
    }
private:
    // Store the logo image.
    QPixmap m_pixmap;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addZoomCombo()
{
   // zoomCombo = new QComboBox;
   // zoomCombo->setFocusPolicy( Qt::NoFocus );
    // FIXME: String below should probably be localized.
    //QStringList list(QStringList() << "" << "10%" << "25%" << "33%" << "50%" << "66%" << "75%" << "100%" << "200%" << "300%" << "400%" << "500%" << "600%" << "800%");
    //zoomCombo->addItems(list);
    //zoomCombo->setEnabled(true);
    //zoomCombo->insertSeparator(1);
   // zoomCombo->insertSeparator(8);
    //zoomCombo->insertSeparator(10);
    //ui->mainToolBar->addWidget(zoomCombo);
    //connect(zoomCombo, SIGNAL(activated(const QString&)), this, SLOT(onZoomChanged(const QString&)));
    //connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::onSubWindowActivated);
}

void MainWindow::on_actionOpen_triggered()
{
    const QStringList& fileName = QFileDialog::getOpenFileNames(this, tr("Open File"),
    SETTINGS->getOpenFolder(), tr("Image Files (*.png *.jpg *.jpeg *.gif *.tif *.tiff *.bmp *.ico);;"
                                "PNG(*.png);;"
                                "JPEG(*.jpg *.jpeg);;"
                                "GIF(*.gif);;"
                                "TIFF(*.tif *.tiff);;"
                                "BMP(*.bmp);;"
                                "ICO(*.ico);;"
                                "All Files (*)"));
    if(fileName.length()>0)
    {
        for(int i=0;i<fileName.length();i++)
        {
            openFile(fileName[i]);
        }

        if(SETTINGS->getPreviouslyOpened() == true)
        {
            QDir d = QFileInfo(fileName[0]).absoluteDir();
            SETTINGS->setOpenFolder(d.absolutePath());
        }
    }
}

bool MainWindow::fileTypeSupported(QList<QByteArray> formats, QString ext)
{
    bool status = false;
    for(int i=0;i<formats.length();i++)
    {
        if(formats[i] == ext)
        {
            status = true;
        }
    }
    return status;
}

QString MainWindow::prepareFile(const QString& fileName)
{
    QString newFileName = fileName;
    QFileInfo info(fileName);
    QImageReader reader(fileName);
    reader.setDecideFormatFromContent(true); // Autodetect file type without depending on extension

    if(info.completeSuffix().toLower() != "jpg" && info.completeSuffix() != reader.format() && fileTypeSupported(reader.supportedImageFormats(),reader.format()))
    {
        int ret = QMessageBox::warning(this,
                    tr("Incorrect file extension detected"),
                    tr("Do you want to update this extension?"),
                    QMessageBox::Save,QMessageBox::Cancel);

        if(ret == QMessageBox::Save)
        {
          newFileName = info.path()+QDir::separator()+info.baseName()+"."+reader.format();
          QDir dir (info.baseName());
          dir.rename(fileName,newFileName);
        }
        else if(ret == QMessageBox::Cancel)
        {
            newFileName = "";
        }
    }
    else if(!fileTypeSupported(reader.supportedImageFormats(),reader.format()))
    {
        newFileName = "";
        showError(tr("Please open a valid image file"));
    }

    return newFileName;
}

void MainWindow::openFile(const QString& fileName)
{
    if(!fileExists(fileName))
    {
        showError(tr("Image does not exist at this file path"));
    }
    else
    {
        QString updatedFileName = prepareFile(fileName);

        if (!updatedFileName.isEmpty())
        {
            this->fileName = fileName;
            MdiArea *md = new MdiArea(this,fileName);
            this->setCentralWidget(md);
            this->show();
        }
    }
}
void MainWindow::updateRecentFilesMenu()
{
    ui->menuRecent_Files->clear();

    QList<QVariant> recentFiles = SETTINGS->getRecentFiles();
    QList<QVariant>::iterator i;

    for(i = recentFiles.begin(); i != recentFiles.end(); i++)
    {
        const QString& fileName = (*i).toString();
        if(fileExists(fileName))
        {
            QAction* action = ui->menuRecent_Files->addAction(fileName);
            connect(action, &QAction::triggered, [this, fileName] () {
                openFile(fileName);
            });
        }
    }
}


bool MainWindow::fileExists(QString path) {
    QFileInfo check_file(path);

    return (check_file.exists() && check_file.isFile());
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

