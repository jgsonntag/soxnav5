#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QList>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

#include "setupgui.h"
#include "genericclient.h"
#include "constants.h"
#include "utility.h"
#include "airplane.h"

struct wp
{
    QString name;
    double lat;
    double lon;
    double x;
    double y;
    double gs;
    double manmin;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    GenericClient *myClient;
    QString host;
    quint16 port;
    void readSettings();
    void closeEvent(QCloseEvent *event);
    void writeSettings();
    void createMenus();
    void createActions();
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *openAct;
    QAction *closeAct;
    QAction *exitAct;
    QAction *aboutAct;
    QString wpfilename;
    QFile *wpfile;
    QString stemp;
    QString stemp2;
    QList<wp> waypoints;
    void ingestWaypoints();
    wp newwaypoint;
    bool received10;
    bool received11;
    bool received13;
    double nlat;
    double nlon;
    double nhell;
    double ncrs;
    double nspeed;
    double nroc;
    double lon0;
    int proj; // 0 = north polar stereo; 1 = mercator; 2 = south polar stereo
    double x;
    double y;
    int zone;
    airplane myplane;

    QLabel *wp6label;
    QLabel *datalabel;
    QLabel *latlabel;
    QLabel *lonlabel;
    int i;

private slots:
    void slotNewData(QString);
    void slotFirstData(QString);
    void slotGetWPFilename();
    void slotOpenWPFile(QString);
    void slotCloseWPFile();
    void slotAbout();

};

#endif // MAINWINDOW_H
