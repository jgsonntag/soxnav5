#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up the styles
    qApp->setStyleSheet("QGroupBox { background: white }");

    // Retrieve the saved settings
    readSettings();

    // Get starting info from user
    setupGui *setup = new setupGui(&host,&port);
    while (setup->exec() == QDialog::Rejected)
    {
        QMessageBox msgBox;
        msgBox.setText("Invalid input");
        msgBox.exec();
    }

    // Build the menu bar
    createActions();
    createMenus();

    // Window management
    setWindowTitle("SOXNav5");
    setMinimumSize(160,160);
    resize(300,520);
    QWidget *window = new QWidget(this);

    // Temporary stuff
    QGroupBox *tempwpbox = new QGroupBox("waypoint6");
    QHBoxLayout *tempwplayout = new QHBoxLayout();
    wp6label = new QLabel();
    tempwplayout->addWidget(wp6label);
    tempwpbox->setLayout(tempwplayout);
    //
    QGroupBox *tempdatabox = new QGroupBox("incoming data");
    QHBoxLayout *tempdatalayout = new QHBoxLayout();
    datalabel = new QLabel();
    tempdatalayout->addWidget(datalabel);
    tempdatabox->setLayout(tempdatalayout);
    //
    QGroupBox *tempcoordsbox = new QGroupBox("x and y");
    QHBoxLayout *tempcoordslayout = new QHBoxLayout();
    latlabel = new QLabel();
    lonlabel = new QLabel();
    tempcoordslayout->addWidget(latlabel);
    tempcoordslayout->addWidget(lonlabel);
    tempcoordsbox->setLayout(tempcoordslayout);

    // Finalize the overall GUI
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addWidget(tempwpbox);
    mainlayout->addWidget(tempdatabox);
    mainlayout->addWidget(tempcoordsbox);
    window->setLayout(mainlayout);
    setCentralWidget(window);

    // Create the connection to the GPS server
    myClient = new GenericClient(this,host,port);
    connect(myClient,SIGNAL(newLine(QString)),this,SLOT(slotFirstData(QString)));


}


void MainWindow::createMenus()
{

    fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(openAct);
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(aboutAct);

}



void MainWindow::createActions()
{

    // File menu
    openAct = new QAction("Open waypoint file",this);
    connect(openAct,SIGNAL(triggered()),this,SLOT(slotGetWPFilename()));
    exitAct = new QAction("Exit",this);
    connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));

    // Help menu
    aboutAct = new QAction("About",this);
    connect(aboutAct,SIGNAL(triggered()),this,SLOT(slotAbout()));

}



MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::readSettings()
{

    QSettings settings("NASA","SOXNav5");
    host = settings.value("host","localhost").toString();
    port = settings.value("port",4062).toInt();
    wpfilename = settings.value("waypoints","").toString();

}



void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
}



void MainWindow::writeSettings()
{

    QSettings settings("NASA","SOXNav5");
    settings.setValue("host",host);
    settings.setValue("port",port);
    settings.setValue("waypoints",wpfilename);

}



void MainWindow::slotGetWPFilename()
{

    // Prompt the user for the desired waypoint filename
    wpfilename = QFileDialog::getOpenFileName(this,"Select a waypoint file",
                                              ".",
                                              "Sequence files (*.sequence)");
    slotOpenWPFile(wpfilename);

}



void MainWindow::slotOpenWPFile(QString wpfilename)
{

    // Ingest the waypoint file if it exists
    wpfile = new QFile(wpfilename);
    if (wpfile->exists())
    {
        stemp = "Close waypoint file";
        stemp.append(wpfilename);
        closeAct = new QAction(stemp,this);
        connect(closeAct,SIGNAL(triggered()),this,SLOT(slotCloseWPFile()));
        fileMenu->removeAction(openAct);
        fileMenu->insertAction(exitAct,closeAct);
        ingestWaypoints();
    }

    // Selected file does not exist
    else if (wpfilename!="")
    {
        stemp = "Waypoint file ";
        stemp.append(wpfilename);
        stemp.append(" does not exist");
        QMessageBox msgBox;
        msgBox.setText(stemp);
        msgBox.exec();
        wpfilename = "";
        waypoints.clear();
    }

    // Null filename
    else
        waypoints.clear();

}


void MainWindow::ingestWaypoints()
{

    wpfile->open(QIODevice::ReadOnly);
    QTextStream instream(wpfile);
    while (!instream.atEnd())
    {
        stemp = instream.readLine();
        newwaypoint.name = stemp.section(' ',0,0,QString::SectionSkipEmpty);
        stemp2 = stemp.section(' ',1,1,QString::SectionSkipEmpty);
        newwaypoint.lat = (stemp2.toDouble())*DEG2RAD;
        stemp2 = stemp.section(' ',2,2,QString::SectionSkipEmpty);
        newwaypoint.lon = (stemp2.toDouble())*DEG2RAD;
        stemp2 = stemp.section(' ',3,3,QString::SectionSkipEmpty);
        newwaypoint.gs = stemp2.toDouble();
        stemp2 = stemp.section(' ',4,4,QString::SectionSkipEmpty);
        newwaypoint.manmin = stemp2.toDouble();
        if (proj==0)
        {
            geod2ps(newwaypoint.lat/DEG2RAD,newwaypoint.lon/DEG2RAD,
                    70.0,315.0,1.0,AE,FLAT,&x,&y);
        }
        else if (proj==1)
        {
            geod2utm(newwaypoint.lat/DEG2RAD,newwaypoint.lon/DEG2RAD,
                     lon0,&y,&x,&zone);
        }
        else
        {
            geod2ps(newwaypoint.lat/DEG2RAD,newwaypoint.lon/DEG2RAD,
                    -71.0,0.0,1.0,AE,FLAT,&x,&y);
        }
        newwaypoint.x = x;
        newwaypoint.y = y;
        waypoints.append(newwaypoint);
    }
    wp6label->setText(waypoints[5].name);

    //for (i=0;i<waypoints.size();i++)
    //    qDebug() << waypoints[i].name << waypoints[i].x << waypoints[i].y;

}


void MainWindow::slotCloseWPFile()
{

    fileMenu->removeAction(closeAct);
    fileMenu->insertAction(exitAct,openAct);
    wpfilename = "";
    waypoints.clear();
    //wpindex = 0;

}



void MainWindow::slotFirstData(QString lineQstring)
{

    //qDebug() << "in slotFirstData";

    // Disconnect this slot - we only want to use it once
    disconnect(myClient,SIGNAL(newLine(QString)),this,SLOT(slotFirstData(QString)));

    // Determine the message type and parse as appropriate
    received10 = false;
    received11 = false;
    received13 = false;
    stemp = lineQstring.section(',',0,0);
    if (stemp=="13")
    {
      received13 = true;
      stemp = lineQstring.section(',',1,1);
      //ymd = stemp.toDouble();
      //parseymd(ymd,&year,&month,&day);
      stemp = lineQstring.section(',',2,2);
      //hms = stemp.toDouble();
      //parsehms(hms,&hour,&min,&sec);
      stemp = lineQstring.section(',',3,3);
      nlat = stemp.toDouble();
      stemp = lineQstring.section(',',4,4);
      nlon = stemp.toDouble();
      stemp = lineQstring.section(',',5,5);
      nhell = stemp.toDouble();
      stemp = lineQstring.section(',',6,6);
      ncrs = stemp.toDouble();
      stemp = lineQstring.section(',',7,7);
      nspeed = stemp.toDouble();
      stemp = lineQstring.section(',',8,8);
      nroc = stemp.toDouble();
    }
    else if (stemp=="11")
    {
      received11 = true;
      stemp = lineQstring.section(',',1,1);
      //ymd = stemp.toDouble();
      //parseymd(ymd,&year,&month,&day);
      stemp = lineQstring.section(',',2,2);
      //hms = stemp.toDouble();
      //parsehms(hms,&hour,&min,&sec);
      stemp = lineQstring.section(',',3,3);
      nlat = stemp.toDouble();
      stemp = lineQstring.section(',',4,4);
      nlon = stemp.toDouble();
      stemp = lineQstring.section(',',5,5);
      nhell = stemp.toDouble();
      stemp = lineQstring.section(',',6,6);
      ncrs = stemp.toDouble();
      stemp = lineQstring.section(',',7,7);
      nspeed = stemp.toDouble();
      stemp = lineQstring.section(',',8,8);
      nroc = stemp.toDouble();
    }
    else if (stemp=="10")
    {
      received10 = true;
      stemp = lineQstring.section(',',1,1);
      //wks = stemp.toDouble();
      //sec2hms(wks,&hour,&min,&sec);
      //year = 9999;
      //month = 99;
      //day = 99;
      stemp = lineQstring.section(',',2,2);
      nlat = stemp.toDouble();
      stemp = lineQstring.section(',',3,3);
      nlon = stemp.toDouble();
      stemp = lineQstring.section(',',4,4);
      nhell = stemp.toDouble();
      stemp = lineQstring.section(',',6,6);
      ncrs = stemp.toDouble();
      stemp = lineQstring.section(',',7,7);
      nspeed = stemp.toDouble();
      stemp = lineQstring.section(',',8,8);
      nroc = stemp.toDouble();
    }
    else
    {
      qDebug() << "Unrecognized network message type\n";
    }

    // Set projection parameters
    lon0 = nlon;
    if (nlat>59.0)
        proj = 0; //north polar stereographic
    else if (nlat<=59.0&&nlat>-52.0)
        proj = 1; // mercator
    else
        proj = 2; // south polar stereographic

    // Update the airplane class
    myplane.setState(nlat,nlon,nhell,ncrs,nspeed,nroc,proj,lon0);

    // Open the previously saved waypoint file, now that we know projection
    slotOpenWPFile(wpfilename);

    // Now connect new network messages to the regular slot
    connect(myClient,SIGNAL(newLine(QString)),this,SLOT(slotNewData(QString)));

    if (received10)
        datalabel->setText("message10");
    else if (received11)
        datalabel->setText("message11");
    else if (received13)
        datalabel->setText("message13");
    latlabel->setNum(myplane.getx());
    lonlabel->setNum(myplane.gety());

}



void MainWindow::slotNewData(QString lineQstring)
{

    //qDebug() << "in slotNewData; proj=" << proj;

    // Determine the message type and parse as appropriate
    received10 = false;
    received11 = false;
    received13 = false;
    stemp = lineQstring.section(',',0,0);
    if (stemp=="13")
    {
      received13 = true;
      stemp = lineQstring.section(',',1,1);
      //ymd = stemp.toDouble();
      //parseymd(ymd,&year,&month,&day);
      stemp = lineQstring.section(',',2,2);
      //hms = stemp.toDouble();
      //parsehms(hms,&hour,&min,&sec);
      stemp = lineQstring.section(',',3,3);
      nlat = stemp.toDouble();
      stemp = lineQstring.section(',',4,4);
      nlon = stemp.toDouble();
      stemp = lineQstring.section(',',5,5);
      nhell = stemp.toDouble();
      stemp = lineQstring.section(',',6,6);
      ncrs = stemp.toDouble();
      stemp = lineQstring.section(',',7,7);
      nspeed = stemp.toDouble();
      stemp = lineQstring.section(',',8,8);
      nroc = stemp.toDouble();
    }
    else if (stemp=="11")
    {
      received11 = true;
      stemp = lineQstring.section(',',1,1);
      //ymd = stemp.toDouble();
      //parseymd(ymd,&year,&month,&day);
      stemp = lineQstring.section(',',2,2);
      //hms = stemp.toDouble();
      //parsehms(hms,&hour,&min,&sec);
      stemp = lineQstring.section(',',3,3);
      nlat = stemp.toDouble();
      stemp = lineQstring.section(',',4,4);
      nlon = stemp.toDouble();
      stemp = lineQstring.section(',',5,5);
      nhell = stemp.toDouble();
      stemp = lineQstring.section(',',6,6);
      ncrs = stemp.toDouble();
      stemp = lineQstring.section(',',7,7);
      nspeed = stemp.toDouble();
      stemp = lineQstring.section(',',8,8);
      nroc = stemp.toDouble();
    }
    else if (stemp=="10")
    {
      received10 = true;
      stemp = lineQstring.section(',',1,1);
      //wks = stemp.toDouble();
      //sec2hms(wks,&hour,&min,&sec);
      //year = 9999;
      //month = 99;
      //day = 99;
      stemp = lineQstring.section(',',2,2);
      nlat = stemp.toDouble();
      stemp = lineQstring.section(',',3,3);
      nlon = stemp.toDouble();
      stemp = lineQstring.section(',',4,4);
      nhell = stemp.toDouble();
      stemp = lineQstring.section(',',6,6);
      ncrs = stemp.toDouble();
      stemp = lineQstring.section(',',7,7);
      nspeed = stemp.toDouble();
      stemp = lineQstring.section(',',8,8);
      nroc = stemp.toDouble();
    }
    else
    {
      qDebug() << "Unrecognized network message type\n";
    }

    // Update the airplane class
    myplane.setState(nlat,nlon,nhell,ncrs,nspeed,nroc,proj,lon0);

    if (received10)
        datalabel->setText("message10");
    else if (received11)
        datalabel->setText("message11");
    else if (received13)
        datalabel->setText("message13");
    latlabel->setNum(myplane.getx());
    lonlabel->setNum(myplane.gety());

    qDebug() << myplane.gethell();

}



void MainWindow::slotAbout()
{

    QMessageBox msgBox;
    msgBox.setText("SOXNav5\n"
                   "Qt5-Based Aircraft Navigation System\n"
                   "Author: John G. Sonntag\n"
                   "Released: 1 August 2018");
    msgBox.exec();

}
