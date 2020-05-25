#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool fileTypeSupported(QList<QByteArray> formats, QString ext);
    QString prepareFile(const QString& fileName);
    void openFile(const QString& fileName);
    bool fileExists(QString path);
    void showError(const QString &message);
    void updateRecentFilesMenu();
    void addZoomCombo();
    void onSelectionChanged(bool visible);
    void refreshTools();

private slots:
    void on_actionOpen_triggered();

private:
    Ui::MainWindow *ui;
    QString fileName;
};
#endif // MAINWINDOW_H
