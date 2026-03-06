
#include <QVariant>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QMenuBar>
#include <QStatusBar>
#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QTimer>

#include <random>
#include <iostream>

#include "logger.h"
#include "terrainGen.h"
#include "plateColorDlg.h"
#include "constants.h"
#include "hexagon.h"
#include "evDist.h"
#include "mapDisplay.h"
#include "graphicsLayer.h"

#include "imageProps.h"

terrainGen::terrainGen() : QMainWindow(), m_pDisplay(nullptr), m_pScene(nullptr), m_props(nullptr),  m_curTime(0), m_centers(nullptr), m_bDirty(false), m_fileName("")
{
  m_gen = new std::mt19937(m_rd());

  readSettings();                                 // read configuration for file....

  setupUI();                                      // build UI
  setupActions();                                 // build actions for menus
  setupMenu();                                    // setup menus

  m_pScene = new QGraphicsScene(this);
  m_pDisplay->setScene(m_pScene);

}



terrainGen::~terrainGen()
{
    if (m_vecGrid.size() > 0)
    {
        //std::vector<CHexagon*>::iterator iter = m_vecGrid.begin();
        //while (m_vecGrid.end() != iter)
        //{
        //    delete (*iter);
        //    (*iter) = nullptr;
        //    ++iter;
        //}

        m_vecGrid.erase(m_vecGrid.begin(), m_vecGrid.end());
    }

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private functions
void terrainGen::setupUI()
{
    QWidget*     centralwidget;
    QVBoxLayout* verticalLayout;

    if (this->objectName().isEmpty())
        this->setObjectName("MainWindow");

    this->resize(scrWidth, scrHeight);
    this->setWindowTitle("terrain genertor");

    centralwidget = new QWidget(this);
    centralwidget->setObjectName("centralwidget");

    verticalLayout = new QVBoxLayout(centralwidget);
    verticalLayout->setObjectName("verticalLayout");

    // setup main graphics display
    m_pDisplay = new mapDisplay(centralwidget);   // QGraphicsView(centralwidget);
    m_pDisplay->setObjectName("graphicsView");
    m_pDisplay->setMainWndPtr(this);
    m_pDisplay->setMouseTracking(true);
    m_pDisplay->setGeometry(QRect(5, 1, scrWidth-9, scrHeight-9));           // leave space for margins

    // set up menu bar
    m_menubar = new QMenuBar(this);
    m_menubar->setObjectName("menubar");
    m_menubar->setGeometry(QRect(0, 0, scrWidth, mbarHeight));

    // set up status bar
    m_statusbar = new QStatusBar(this);
    m_statusbar->setObjectName("statusbar");

    verticalLayout->addWidget(m_pDisplay);
    this->setCentralWidget(centralwidget);
    this->setMenuBar(m_menubar);
    this->setStatusBar(m_statusbar);
}


/************************************************************************************************************************
 * function  : setuActions 
 *
 * abstract  : This creates and configures actions associate with menu items.
 *
 * parameters: none
 *
 * returns   : void
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::setupActions()
{
    m_pFileOpen = new QAction("Open", this );
    m_pFileOpen->setShortcuts(QKeySequence::Open);
    m_pFileOpen->setStatusTip("open a terrain file");
    connect(m_pFileOpen, SIGNAL(triggered()), this, SLOT(onFileOpen()));

    m_pFileNew = new QAction("New", this);
    m_pFileNew->setShortcuts(QKeySequence::New);
    m_pFileNew->setStatusTip("creates a new image");
    connect(m_pFileNew, SIGNAL(triggered()), this, SLOT(onFileNew()));

    m_pFileClose = new QAction("Close", this);
    m_pFileClose->setShortcuts(QKeySequence::Close);
    m_pFileClose->setStatusTip("closes the currently open file");
    connect(m_pFileClose, SIGNAL(triggered()), this, SLOT(onFileClose()));

    m_pFileSave = new QAction("Save", this);
    m_pFileSave->setShortcuts(QKeySequence::Save);
    m_pFileSave->setStatusTip("save current terrain");
    connect(m_pFileSave, SIGNAL(triggered()), this, SLOT(onFileSave()));

    m_pFileSaveAs = new QAction("Save As", this);
    m_pFileSaveAs->setShortcuts(QKeySequence::SaveAs);
    m_pFileSaveAs->setStatusTip("save current terrain with a new name");
    connect(m_pFileSaveAs, SIGNAL(triggered()), this, SLOT(onFileSaveAs()));

    m_pFileExit = new QAction("Exit", this);
    m_pFileExit->setShortcuts(QKeySequence::Quit);
    m_pFileExit->setStatusTip("exit application");
    connect(m_pFileExit, SIGNAL(triggered()), this, SLOT(onFileExit()));

    m_pViewZoom = new QAction("Zoom", this);
    //m_pViewZoom->setShortcuts(QKeySequence::Open);
    m_pViewZoom->setStatusTip("arbitrary zoom");
    connect(m_pViewZoom, SIGNAL(triggered()), this, SLOT(onViewZoom()));

    m_pViewZoomIn = new QAction("Zoom", this);
    m_pViewZoomIn->setShortcuts(QKeySequence::ZoomIn);
    m_pViewZoomIn->setStatusTip("zoom in");
    connect(m_pViewZoomIn, SIGNAL(triggered()), this, SLOT(onViewZoomIn()));

    m_pViewZoomOut = new QAction("Zoom Out", this);
    m_pViewZoomOut->setShortcuts(QKeySequence::ZoomOut);
    m_pViewZoomOut->setStatusTip("zoom out");
    connect(m_pViewZoomOut, SIGNAL(triggered()), this, SLOT(onViewZoomOut()));

    m_pViewRedraw = new QAction("Redraw", this);
    m_pViewRedraw->setShortcuts(QKeySequence::Refresh);
    m_pViewRedraw->setStatusTip("redraw");
    connect(m_pViewRedraw, SIGNAL(triggered()), this, SLOT(onViewRedraw()));

    m_pViewImageProps = new QAction("Image Properties", this);
    //m_pViewImageProps->setShortcuts(QKeySequence::Open);
    m_pViewImageProps->setStatusTip("view/modify image settings");
    connect(m_pViewImageProps, SIGNAL(triggered()), this, SLOT(onViewImage()));

    for (uint32_t ndx = 0; ndx < mapLayers; ndx++)
    {
      m_pViewLayer[ndx] = new QAction(QString("view/hide layer: %1").arg(layerName[ndx]), this);
      m_pViewLayer[ndx]->setStatusTip(QString("toggle visibility of layer %1").arg(layerName[ndx]));
      m_pViewLayer[ndx]->setData(QVariant(ndx));
      connect(m_pViewLayer[ndx], &QAction::triggered, this, &terrainGen::onViewToggleVisibility);
    }

    m_pSimCenters = new QAction("Plate centers");
    // m_pSimCenters->setShortcuts();
    m_pSimCenters->setStatusTip("set plate centers");
    m_pSimCenters->setEnabled(true);
    connect(m_pSimCenters, SIGNAL(triggered()), this, SLOT(onSimCenters()));

    m_pSimPlates = new QAction("generate plates");
    // m_pSimCenters->setShortcuts();
    m_pSimPlates->setStatusTip("generate plate boundaries");
    m_pSimPlates->setEnabled(false);
    connect(m_pSimPlates, SIGNAL(triggered()), this, SLOT(onSimPlates()));

    m_pSimMotion = new QAction("plate motion");
    // m_pSimCenters->setShortcuts();
    m_pSimMotion->setStatusTip("generate plate motion vectors");
    m_pSimMotion->setEnabled(false);
    connect(m_pSimMotion, SIGNAL(triggered()), this, SLOT(onSimMotion()));

    m_pSimTimeDelta = new QAction("time step");
    // m_pSimCenters->setShortcuts();
    m_pSimTimeDelta->setStatusTip("run for one time step");
    m_pSimTimeDelta->setEnabled(false);
    connect(m_pSimTimeDelta, SIGNAL(triggered()), this, SLOT(onSimTimeDelta()));

     m_pSimRun = new QAction("run");
    // m_pSimCenters->setShortcuts();
    m_pSimRun->setStatusTip("run for total time duration");
    connect(m_pSimRun, SIGNAL(triggered()), this, SLOT(onSimRun()));

    m_pEditPerfs = new QAction("Preferences", this);
    m_pEditPerfs->setShortcuts(QKeySequence::Preferences);
    m_pEditPerfs->setStatusTip("view/edit application preferences");
    connect(m_pEditPerfs, SIGNAL(triggered()), this, SLOT(onEditPrefs()));

    m_pEditPlateColors = new QAction("Plate Colors", this);
    //m_pEditPlateColors->setShortcuts()
    m_pEditPlateColors->setStatusTip("edit default plate colors");
    connect(m_pEditPlateColors, SIGNAL(triggered()), this, SLOT(onEditPlateColors()));

    m_pHelpHelp = new QAction("Help", this);
    m_pHelpHelp->setShortcuts(QKeySequence::HelpContents);
    m_pHelpHelp->setStatusTip("shows the application help files");
    connect(m_pHelpHelp, SIGNAL(triggered()), this, SLOT(onHelpHelp()));

    m_pHelpAbout = new QAction("About", this);
    //m_pHelpAbout->setShortcuts(QKeySequence::About);
    m_pHelpAbout->setStatusTip("show the application about dialog");
    connect(m_pHelpAbout, SIGNAL(triggered()), this, SLOT(onHelpAbout()));
}


/************************************************************************************************************************
 * function  : setupMenu
 *
 * abstract  : This build the various menus used in the applications
 *
 * parameters: none
 *
 * returns   : void
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::setupMenu()
{
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_pFileOpen);
    fileMenu->addAction(m_pFileNew);
    fileMenu->addAction(m_pFileClose);
    fileMenu->addSeparator();
    fileMenu->addAction(m_pFileSave);
    fileMenu->addAction(m_pFileSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(m_pFileExit);

    QMenu* viewMenu = m_menubar->addMenu("&View");
    viewMenu->addAction(m_pViewZoom);
    viewMenu->addAction(m_pViewZoomIn);
    viewMenu->addAction(m_pViewZoomOut);
    viewMenu->addSeparator();
    viewMenu->addAction(m_pViewRedraw);
    viewMenu->addSeparator();
    QMenu* viewLayers = viewMenu->addMenu("viewLayers");
    for (uint32_t ndx = 0; ndx < mapLayers; ndx++) viewLayers->addAction(m_pViewLayer[ndx]);
    viewMenu->addAction(m_pViewImageProps);

    QMenu* editMenu = m_menubar->addMenu("&Edit");
    editMenu->addAction(m_pEditPerfs);
    editMenu->addAction(m_pEditPlateColors);

    QMenu* simMenu = m_menubar->addMenu("Simulation");
    simMenu->addAction(m_pSimCenters);
    simMenu->addAction(m_pSimPlates);
    simMenu->addAction(m_pSimMotion);
    simMenu->addAction(m_pSimTimeDelta);
    simMenu->addSeparator();
    simMenu->addAction(m_pSimRun);

    QMenu* helpMenu = m_menubar->addMenu("&Help");
    helpMenu->addAction(m_pHelpHelp);
    helpMenu->addAction(m_pHelpAbout);
}



/************************************************************************************************************************
 * function  : readSettings
 *
 * abstract  : This reads the configuration settings from either the registry or a configuration file depending on the 
 *             host OS.
 *
 * parameters: none
 *
 * returns   : void
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::readSettings()
{
  QSettings    settings("Anzus", "TerrainGen");

  m_imageWidth = settings.value("image/width", 1024).toInt();
  m_imageHeight = settings.value("image/height", 768).toInt();
  m_hexagonSize = settings.value("image/gridSize", 18).toInt();
  m_hexagonOrien = settings.value("image/orientation", hexagon::orien::VERTICAL).toInt();
  m_hexagonProps = settings.value("image/hexProps", 0).toInt();
  m_cntPlates = settings.value("simulation/plates", 0).toInt();
  m_timeStep = settings.value("simulation/timeStep", 100000).toInt();            // time step for simulation in years
  m_maxTime = settings.value("simulation/maxTime", 4500000000).toULongLong();    // max length of time for simulation in years.

  // create properties structure ....
  m_props = new struct imageProps;
  m_props->imageWidth = m_imageWidth;
  m_props->imageHeight = m_imageHeight;
  m_props->hexagonSize = m_hexagonSize;
  m_props->hexagonOrient = m_hexagonOrien;
  m_props->hexagonProps = m_hexagonProps;
  m_props->cntPlates = m_cntPlates;

  settings.beginWriteArray("noise");
  for (int ndx = 0; ndx < nbrHarmonics; ndx++)
  {
    settings.setArrayIndex(ndx);
    float amp = pow((ndx + 1), (ndx));                   // ndx = 0 => 1           ndx = 1 => 2           ndx = 2 => 9
    float val = pow((1.0 / (ndx + 1.0)), (ndx + 1));     // ndx = 0 => 1           ndx = 1 => 1/4         ndx = 2 => (1/9)^9
    m_amplitudes[ndx] = settings.value("amplitude", amp).toFloat();
    m_props->amplitude[ndx] = m_amplitudes[ndx];
    m_frequency[ndx] = settings.value("frequencey", val).toFloat();
    m_props->frequency[ndx] = m_frequency[ndx];
  }
}



/************************************************************************************************************************
 * function  : writeSettings
 *
 * abstract  : This writes settings out to the configuration location - either the registry or a configuration file based
 *             on the host OS.
 *
 * parameters: none
 *
 * returns   : void
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::writeSettings()
{
    qDebug() << "in terrainGen::writeSettings";
    QSettings   settings("Anzus", "TerrainGen");

    settings.setValue("image/width", m_imageWidth);
    settings.setValue("image/height", m_imageHeight);
    settings.setValue("image/gridSize", m_hexagonSize);
    settings.setValue("image/orientation", m_hexagonOrien);
    settings.setValue("image/hexProps", m_hexagonProps);
    settings.setValue("simulation/plates", m_cntPlates);
    settings.setValue("simulation/timeStep", m_timeStep);
    settings.setValue("simulation/maxDuration", m_maxTime);

    settings.beginWriteArray("noise");
    for (int ndx = 0; ndx < nbrHarmonics; ndx++)
    {
        settings.setArrayIndex(ndx);
        settings.setValue("amplitude", m_amplitudes[ndx]);
        settings.setValue("frequency", m_frequency[ndx]);
    }
}


/************************************************************************************************************************
 * function  : doSave 
 *
 * abstract  : This function performs the actual work of saving a document, it is called by both `onSave' and `onSaveAs'
 *             This function save the current document into the file represented by the variable m_fileName.
 *
 * parameters: none
 *
 * returns   : none
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::doSave()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// protected functions
void terrainGen::closeEvent(QCloseEvent* evt)
{
    qDebug() << "in closeEvent";
    writeSettings();
    evt->accept();

}

void terrainGen::mouseMoveEvent(QMouseEvent* evt)
{
  const int margin = 20;                                  // margin width in pixels

  QPointF   sceneLoc = m_pDisplay->mapToScene(evt->pos());

  QString strStatus = QString("cursor at %1, %2").arg(sceneLoc.x()).arg(sceneLoc.y());
  m_statusbar->showMessage(strStatus);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// public slots
void terrainGen::onFileOpen() { qDebug() << "in onFileOpen"; }



/************************************************************************************************************************
 * function  : onFileNew
 *
 * abstract  : This function creates a new document.  The following steps are preformed
 *               (1) check the the existing document (if any) has been saved
 *               (2) clear the scene
 *               (3) read image properties to get the size of the image
 *               (4) creates a triangular lattice to tile the image
 *
 * parameters: none
 *
 * returns   : void
 *
 * written   :  Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::onFileNew() 
{ 
    imagePropDlg    dlg(m_props, true);

    // check if current document needs saving
    if ((m_fileName != "") && m_bDirty)                    // do we have a loaded document, and has it been modified
    {
        int nRet = QMessageBox::question(nullptr, "Save current document", "The current document has been modified, do you with to save?");
        if (QMessageBox::Yes == nRet)
        {
            onFileSave();
        }
    }
    
    // clear the scene and clear the state variables...
    if (nullptr != m_centers) { delete[] m_centers; m_centers = nullptr;}
    std::vector<hexagon*>::iterator iter = m_vecGrid.begin();
    while (m_vecGrid.end() != iter)
    {
        delete (*iter);
        (*iter) = nullptr;
        ++iter;
    }
    m_vecGrid.erase(m_vecGrid.begin(), m_vecGrid.end());
    m_pScene->clear();

    // get the properties of the new image
    int32_t  ret = dlg.exec();


    // initialize array for layers
    for (uint32_t ndx = 0; ndx < mapLayers; ndx++)
    {
      m_layers[ndx] = new graphicsLayer;
      m_isVisible[ndx] = true;                      // by default all layers are visible
    }

    if(QDialog::Accepted == ret)
    {
      adjustBorderSize();
        // TODO : generate our noise function here...
        
        QPen  pen(Qt::black);
        pen.setWidth(2);

        genGrid(pen);

        QGraphicsRectItem* pBorder = new QGraphicsRectItem(0, 0, m_props->imageWidth, m_props->imageHeight, m_layers[4]);
        pBorder->setData(0, QVariant("border"));
        pBorder->setPen(pen);


        // add all the layers to the scene
        for (uint ndx = 0; ndx < mapLayers; ndx++) m_pScene->addItem(m_layers[ndx]);

    }

    m_curTime = 0;
    m_pSimCenters->setEnabled(true);
    m_pSimPlates->setEnabled(false);
    m_pSimMotion->setEnabled(false);
}


/************************************************************************************************************************
 * function : onFileClose
 *
 * abstract  : This function closes the currently opened document, and clears the display
 *
 * parameters: void
 *
 * returns   : void 
 *
 * written   : Jan 2022 (GKHuber)
************************************************************************************************************************/
void terrainGen::onFileClose() 
{
    // TODO : clear the state of the simulation...
    // TODO : close the file, if open
    // TODO : clear the display
    qDebug() << "in onFileClose"; 
}



/************************************************************************************************************************
 * function  : onFileSave 
 *
 * abstract  : THis function save the current document.  If the current document does not have a file name, then 
 *             onFileSaveAs is called.  In both cases the actual heavy-lifting of saving to document is perfoemed by
 *             the function `doSave'.
 *
 * parameters: none
 *
 * returns   : void
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::onFileSave() 
{ 
    if (m_fileName == "")
        onFileSaveAs();
    else
        doSave();
}



/************************************************************************************************************************
 * function  : onFileSaveAs
 *
 * abstract  : This function saves the current document under a new name
 *
 * parameters: void
 *
 * returns   : void
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::onFileSaveAs() 
{ 
    QString fileName = "";

    fileName = QFileDialog::getSaveFileName(this, "Save current document as", "./terrain.dat", "terrain files (*.dat *.ter);;all files (*.*)");
    if (fileName != "")
    {
        m_fileName = fileName;
        doSave();
    }
}


/************************************************************************************************************************
 * function  : onFileExit 
 *
 * abstract  : responds to the menu item File | Exit, exitting the application.
 *
 * parameters: none
 *
 * returns   : void
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::onFileExit() 
{ 
    writeSettings();
    QApplication::exit();
}


void terrainGen::onViewZoom() { qDebug() << "in onViewZoom"; }
void terrainGen::onViewZoomIn() { qDebug() << "in onViewZoomIn"; }
void terrainGen::onViewZoomOut() { qDebug() << "in onViewZoomOut"; }

/************************************************************************************************************************
 * Function: 
 *
 * Abstract:
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Feb 2026 (gkhuber) 
 ***********************************************************************************************************************/
void terrainGen::onViewToggleVisibility()
{
  QAction* action = qobject_cast<QAction*>(sender());
  uint32_t ndx = action->data().toInt();

  std::cout << "in onViewToggleVisibility, index is " << ndx << std::endl;
  if (m_isVisible[ndx])
  {
    m_layers[ndx]->hide();
    m_isVisible[ndx] = false;
  }
  else
  {
    m_layers[ndx]->show();
    m_isVisible[ndx] = true;
  }
  
}

void terrainGen::onViewRedraw() 
{ 
  uint8_t mode = m_props->hexagonProps;

  m_pScene->setSceneRect(QRectF(QPointF(0, 0), QSizeF(m_imageWidth, m_imageHeight)));
  
  m_pScene->clear();
  //for (CHexagon* h : m_vecGrid)
  //{
  //  h->draw(m_pScene);
  //}
  m_pScene->update();
  
}


/************************************************************************************************************************
 * function  : onViewImage
 *
 * abstract  : This function displays the image properties that currently are in use.  This delagates the work to 
 *             onEditPrefs to display the actual properties dialog box.
 *
 * parameters:
 *
 * returns   :
 *
 * written   : Jan 2022 (GKHuber)
************************************************************************************************************************/
void terrainGen::onViewImage() 
{
  imagePropDlg    dlg(m_props, false);
  dlg.exec();
}

void terrainGen::onEditPrefs()
{
  imagePropDlg    dlg(m_props, true);
  dlg.exec();
}

void terrainGen::onEditPlateColors()
{
  plateColorDlg   dlg;

  int32_t ret = dlg.exec();
}



/************************************************************************************************************************
 * function  :  onSimCenters
 *
 * abstract  : This function randomly chooses the plate centers.  This function assums that a new map has been started,
 *             and at the end of the function the variable m_centers has been created and initialized.  It depends on 
 *             the value of m_cntPlates to determine how many plates to create.
 * 
 *             We generate two random numbers in the rand [0, RAND_MAX] and divide by (RAND_MAX + 1) to generte a 
 *             real number between [0, 1).  This is multiplied by the width (height) to generate the x- (y-) coordinate.
 * 
 *             We have the restriction that no plate shall occupy no less than 15% of the total area of the map.
 *
 * parameters: void 
 *
 * returns   : void
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::onSimCenters() 
{ 

    double margin = m_hexagonSize;

    // create a vector for plates structure
    m_plates = new platesT[m_props->cntPlates];
    m_centers = new QPointF[m_props->cntPlates];

    if (m_hexagonOrien == hexagon::orien::HORIZONTAL)
      margin = m_hexagonSize * (1 + 2 * sin30);
    else if (m_hexagonOrien == hexagon::orien::VERTICAL)
      margin = 2 * m_hexagonSize * cos30;
    else
      CLogger::getInstance()->outMsg(cmdLine, CLogger::level::WARNING, "unknown hexagon orientation.");

    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::INFO, "generating %d centes", m_props->cntPlates);

    for (int ndx = 0; ndx < m_props->cntPlates; ndx++)
    {
      bool isValid = false;
      float tempX;
      float tempY;

      std::uniform_real_distribution<>  dist(0.0, 1.0);  // generates a random real number in range[0,1.0)

      while (!isValid)                               // clamp location to at least one grid cell from image boundary.
      {      
        isValid = true;
        tempX = (m_props->imageWidth)* dist(*m_gen);
        tempY = (m_props->imageHeight)* dist(*m_gen);

        if ((tempX < margin) || ((m_imageWidth - tempX) < margin) || (tempY < margin) || ((m_imageHeight = tempY) < margin))
        {
          CLogger::getInstance()->outMsg(cmdLine, CLogger::level::DEBUG, "rejected point as too close to edge");
          isValid = false;
        }

      } while (!isValid);

      CLogger::getInstance()->outMsg(cmdLine, CLogger::level::INFO, "plate %d: center is at (%.4f, %.4f)", ndx, tempX, tempY);

      m_centers[ndx] = QPointF(tempX, tempY);
      m_plates[ndx].ndx = ndx + 1;                                 // set index for plate
      m_plates[ndx].center_x = tempX;
      m_plates[ndx].center_y = tempY;
      if(ndx < 10) m_plates[ndx].color = plateColors[ndx];         // TODO: handle case if more than 12 plates

      for (hexagon* ph : m_vecGrid)
      {
        if (ph->contains(m_centers[ndx]))
        {
          ph->setColor(m_plates[ndx].color);                       //ph->setColor(plateColors[ndx]);
          ph->setStyle(bFilled | bColor | bDispCenter);

          //ph->draw(m_pScene);
          m_plates[ndx].vec.push_back(ph->getId());                // add hex to plate list

          CLogger::getInstance()->outMsg(cmdLine, CLogger::level::DEBUG, "hexagon %d belongs to plate %d", ph->getId(), ndx);
          break;
        }
      } // end of for-each loop
    } // end of for loop iterating over plates

    this->update();

    m_pSimCenters->setEnabled(false);
    m_pSimPlates->setEnabled(true);
}


void terrainGen::onSimPlates()
{
  m_timer = new  QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &terrainGen::onSimPlatesImpl);
  m_timer->start(150);
}

void terrainGen::onSimPlatesImpl()
{
  bool mapChange = false;                                                          // flag to monitor is the map has changed      
  bool plateChange = false;
  static uint32_t step = 1;

  CLogger::getInstance()->outMsg(cmdLine, CLogger::level::DEBUG, "in onSimPlates, step %d", step);
  QApplication::processEvents();

  for (uint32_t plateNdx = 0; plateNdx < m_props->cntPlates; plateNdx++)           // iterate over each plate
  {
    plateChange = false;
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::DEBUG, "step %d, working with plate %d", step, plateNdx);

    std::vector<uint32_t> newBorder = {};

    platesT* thePlate = &m_plates[plateNdx];                                     // plate we are currently growing
    uint32_t   cntHexs = thePlate->vec.size();                                   // number of hexagons on the current border
    QColor     plateColor = thePlate->color;

    //for (uint32_t hexNdx = 0; hexNdx < cntHexs; hexNdx++)                        // iterate over the border hexagons
    //{
    //  uint32_t gridID = thePlate->vec.at(hexNdx);                                // grid ID of the border hexagon we are working with
    //  CHexagon* pHex = m_vecGrid.at(gridID);                                     // actual border hexagon we are working with.
    //  double cent_x = pHex->getCenter().x();                                     // coordinates of the center of the hexagon
    //  double cent_y = pHex->getCenter().y();

    //  for (uint32_t a = 0; a < 6; a++)                                             // iterate over the six neighbors
    //  {
    //    double tempX = cent_x + 2 * pHex->getSide() * cos((a * 60) * (pi / 180));    // neighbors coordinates
    //    double tempY = cent_y + 2 * pHex->getSide() * sin((a * 60) * (pi / 180));

    //    for (CHexagon* testHex : m_vecGrid)                                        // iterate over all grid cells
    //    {
    //      if (testHex->contains(QPoint(tempX, tempY)))
    //      {
    //        //CLogger::getInstance()->outMsg(cmdLine, CLogger::level::DEBUG,"   potential border grid cell %d at (%.4f, %.4f)", testHex->getIndex(), tempX, tempY);
    //        if (!((testHex->getState() & bFilled) == bFilled))                     // is cell filled, if so reject it.
    //        {
    //          testHex->setState(testHex->getState() | bFilled);
    //          testHex->setState(testHex->getState() | bColor);
    //          testHex->setColor(plateColor);
    //          newBorder.push_back(testHex->getIndex());                             // cell was accepted add to new border
    //          testHex->draw(m_pScene);
    //        }
    //        else
    //        {
    //          //CLogger::getInstance()->outMsg(cmdLine, CLogger::level::DEBUG, "   rejected");
    //        }
    //      } // end if block
    //    }   // end grid search for loop
    //  }     //end of neighbor search
    //}       // end of processing current neighbors

    thePlate->vec.clear();
    thePlate->vec = newBorder;                                                      // update border list
    if (newBorder.size() > 0) plateChange = true;
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::DEBUG, "plate %d changed this step %s", plateNdx, (plateChange ? "yes" : "no"));
  } // end of plate loop (i.e. plateNdx loop)

  mapChange |= plateChange;
  CLogger::getInstance()->outMsg(cmdLine, CLogger::level::DEBUG, "map changed this step %d : %s", step, (mapChange ? "yes" : "no"));

  if (!mapChange)                                                                  // map did not change on this iteration
  {
    m_timer->stop();
    m_pSimPlates->setEnabled(false);
    m_pSimMotion->setEnabled(true);
  }

  step++;

}

/**************************************************************************************************
 * Function: 
 *
 * Abstract:  tectonic plates velocity is between 1 to 10 cm a year, most are in the range of 2 to
 *            5 cm a year.  assume a normally distributed velociy with \mu = 4.5, and \sigma = 2.0
 *
 * Input   :
 *
 * Returns :
 *
 * Written : () 
 *************************************************************************************************/
void terrainGen::onSimMotion() 
{
  std::normal_distribution<double_t>  norDist(4.5, 2.0);
  std::uniform_real_distribution<double_t> dirDist(0.0, 360.0);
  // TODO : all plates have been drawn -- generate border and store as a path in the plate structure
  // 
  // m_plates is an array of platesT structures
  // TODO : generate motion vector and draw on display, motion vector oragin is plate center
  for (uint32_t ndx = 0; ndx < m_cntPlates; ndx++)
  {
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::INFO, "generating motion vector for plate %d", ndx);

    double_t plateSpeed = norDist(*m_gen);
    if (plateSpeed < 0) plateSpeed = 2.0;
    double_t plateDir = dirDist(*m_gen);
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::INFO, "      speed %.4f", plateSpeed);
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::INFO, "      direction %.4f", plateDir);

    // TODO : draw vector
    double_t OrigX = m_plates[ndx].center_x;
    double_t OrigY = m_plates[ndx].center_y;
    double_t DestX = OrigX + 10*plateSpeed * cos(plateDir * (pi / 180));
    double_t DestY = OrigY + 10*plateSpeed * sin(plateDir * (pi / 180));

    QGraphicsLineItem* line = new QGraphicsLineItem(OrigX, OrigY, DestX, DestY);
    line->setPen(QPen(Qt::black));
    m_pScene->addItem(line);

    double_t HeadX = DestX + 4 * cos(45 * pi / 180);
    double_t HeadY = DestY + 4 * cos(45 * pi / 180);
    QGraphicsLineItem* head = new QGraphicsLineItem(DestX, DestY, HeadX, HeadY);
    head->setPen(QPen(Qt::black));
    m_pScene->addItem(head);
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::INFO, "       orig: (%.4f,%.4f) dest:(%.4f,%.4f)", OrigX, OrigY, DestX, DestY);
    
  }

  m_pSimMotion->setEnabled(false);
  m_pSimTimeDelta->setEnabled(true);
}



/************************************************************************************************************************
 * function  : onSimTimeDelta
 *
 * abstract  : This function simulates the plates moving for a single time step.  The current time is update, and then
 *             the plates are moved in the direction of their velocity vector.  
 *
 * parameters: void 
 *
 * returns   : void
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::onSimTimeDelta()
{ 
    qDebug() << "in onSimTimeDelta"; 
    m_curTime += m_timeStep;
    m_statusbar->showMessage(QString("updating to %1 years").arg(m_curTime));

    _sleep(1);
}



/************************************************************************************************************************
 * function  : onSimRun
 *
 * abstract  : this runs the simulation from the current time to the maximum time for the simulation.  The simulation is
 *             run in m_timeDelta steps.  a 4.5B year max time, with a step size of 100K years will require 45,000 
               iterations. If each itertion takes one second this is approximately 12.5 hours -- get a cup of coffee.
 *
 * parameters: void
 *
 * returns   : void 
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::onSimRun() 
{ 
    qDebug() << "in onSimRun"; 

    onSimCenters();        // generate plates centers, if not done...
    onSimPlates();         // generate plates if not done ....
    onSimMotion();         // generate initial movement vector ....

    for (uint64_t ndx = m_curTime; ndx <= m_maxTime; ndx += m_timeStep)
    {
        onSimTimeDelta();
        
        
        // todo : let message pump do it's things
        QEventLoop  evtLoop(this);
        evtLoop.processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    qDebug() << "current time is: " << m_curTime;
}



void terrainGen::onHelpHelp() { qDebug() << "in onHelpHelp"; }
void terrainGen::onHelpAbout() { qDebug() << "in onHelpAbout"; }



/************************************************************************************************************************
 * function : onSaveConfig
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   :
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void terrainGen::onSaveConfig(imagePropDlg* pdlg)
{
    // save image information....
    m_imageWidth = pdlg->width();
    m_imageHeight = pdlg->height();
    m_hexagonSize = pdlg->hexagon();
    m_cntPlates = pdlg->plates();
    m_hexagonOrien = pdlg->orientation();
    m_hexagonProps = pdlg->hexProps();

    int harmonics = 0;
    float* amps = pdlg->amplitudes(&harmonics);
    float* freqs = pdlg->frequencies(&harmonics);

    for (int ndx = 0; ndx < harmonics; ndx++)
    {
        m_amplitudes[ndx] = amps[ndx];
        m_frequency[ndx] = freqs[ndx];
    }
}
/*

    horizontal spacing is 1.0*width_hex, vertical spacing is 0.75*width_hex

horizontal:


*/
/**********************************************************************************************************************
 * Function: getGrid
 *
 * Abstract: This function generates either a verical or horizontal hexagonal grid.  This is done by modifying the 
 *           starting location of the center for the initial hexagon by to represent the center of each hexagon in the
 *           grid.
 *           The following geometric facts are used to build the grid strucure (taken from 
 *           https://www.redblobgames.com/grids/hexagons/ ).  In each case the variable size, s, is defined as the 
 *           radius of the circumscribed circle -- i.e. distance from center to each vertex.
 *                              vertical (pointy)                   horizontal (flat)
 *           side length, l,         s                                    s\sqrt(3)
 *           width, w,              s\sqrt(3)                             2*s
 *           height, h,             2*s                                   s\sqrt(3) 
 *           2-row width            2.5*w                                 2.5*w
 *           2-row height           1.75*h                                2*h
 *           center                 (0.5*w, 0.5*h)                        (0.5*w, 0.5*h)
 *           horizontal spacing     1.0*w                                 0.75*w
 *           vertical spacing       0.75*h                                1.0*h
 * 
 * Input   : void
 *
 * Returns : void.  modifies m_vecGrid 
 *
 * Written : Dec 2025 (gkhuber) 
 *           Mar 2025 (gkhuber) -- modified to incorporate new hexagon class derived from QGraphicsItem.  Also split
 *                                 out code to modify image size based on size of hexagon into a new function 
 *                                 (adjustBorderSize) and invoke this function from 'onFileNew'.  Leaving this function
 *                                 to just calculate the grid and the display them.  Also implementing the concept of 
 *                                 layers, with the border going in the border-layer, and the grid going in the 
 *                                 grid layer.
 *********************************************************************************************************************/
void terrainGen::genGrid(QPen pen)
{
  QPointF    initialCenter;  // center of top left hexagon
  float      width;          // width of hexagon, depends on orientation
  float      height;         // height of hexagon, depends on orientation

  CLogger::getInstance()->outMsg(cmdLine, CLogger::level::INFO, "current size of border is (%.4f, %.4f, %.4f, %.4f)", 0, 0, m_props->imageWidth, m_props->imageHeight);

  if (m_props->hexagonOrient == hexagon::orien::VERTICAL)
  {
    height = 2 * m_props->hexagonSize;
    width = m_props->hexagonSize * sqrt3;
    const double_t deltaHeight = 0.75 * height;
    const double_t deltaWidth = width;
    
    // calculate center of first hexagon
    initialCenter = QPointF(0.5 * width, 0.5 * height);

    // calculate number of rows and columns
    int32_t maxRow = ceil(m_props->imageHeight/(0.75*height));
    int32_t maxCol = ceil(m_props->imageWidth/width);

    // build grid
    QPointF center = initialCenter;
    for (int row = 0; row < maxRow; row++)
    {
      QPointF rowCenter = initialCenter + QPoint(0, deltaHeight * row);
      // if row is odd, then need to offset the rowCenter
      if ((row % 2))
      {
        rowCenter += QPointF(0.5 * width, 0);
      }
      
      for (int col = 0; col < maxCol; col++)
      {
        center = rowCenter + QPoint(deltaWidth * col, 0);
        hexagon* temp = new hexagon(center, m_props, m_layers[0]);

        // TODO : construct label and center -- need to do it here so we have access to layers
        QGraphicsEllipseItem* centerPt = new QGraphicsEllipseItem(center.x() - 2, center.y() - 2, 4, 4, m_layers[5]);
        centerPt->setBrush(QBrush(Qt::black, Qt::SolidPattern));
        QRectF bbox;
        QString txt = temp->getLabel(&bbox);
        if (bbox.width() < width - 5)              // hexagon is big enough to display label
        {
          QGraphicsTextItem* label = new QGraphicsTextItem(txt, m_layers[5]);
          label->setPos(center - QPoint(0.5*bbox.width(), bbox.height()+3)/* + QPointF(-0.5 * bbox.width(), -0.5 * bbox.height()) */);
        }
        m_vecGrid.push_back(temp);
      }
    }
  }
  else if(m_props->hexagonOrient == hexagon::orien::HORIZONTAL)
  {
    width = 2 * m_props->hexagonSize;
    height = m_props->hexagonSize * sqrt(3);
    const double_t deltaHeight = 0.5*height;                           // space between adjacent cneters in y direction
                                                                       // (half of vertical spacing to account for packing)
    const double_t deltaWidth = 1.50 * width;                          // space between adjacent centers in x direction 
                                                                       // (twice of horizontal spacing to account packing)
    initialCenter = QPointF(0.5 * width, 0.5 * height);

    hexagon* temp1 = new hexagon(initialCenter, m_props, m_layers[0]);
    m_vecGrid.push_back(temp1);

    // calculate number of rows and columns
    int32_t maxRow = m_props->imageHeight / (0.5*height);
    int32_t maxCol = ceil(m_props->imageWidth / (1.5*width));

    // build grid
    QPointF center = initialCenter;
    for (int row = 0; row < maxRow; row++)
    {
      QPointF rowCenter = initialCenter + QPoint(0, deltaHeight * row);
      // if row is odd, then need to offset the rowCenter
      if ((row % 2))
      {
        rowCenter += QPointF(0.75 * width, 0);
      }

      for (int col = 0; col < maxCol; col++)
      {
        center = rowCenter + QPoint(deltaWidth * col, 0);
        hexagon* temp = new hexagon(center, m_props, m_layers[0]);

        // TODO : construct label and center -- need to do it here so we have access to layers
        QGraphicsEllipseItem* centerPt = new QGraphicsEllipseItem(center.x() - 2, center.y() - 2, 4, 4, m_layers[5]);
        centerPt->setBrush(QBrush(Qt::black, Qt::SolidPattern));
        QRectF bbox;
        QString txt = temp->getLabel(&bbox);
        if (bbox.width() < width - 5)              // hexagon is big enough to display label
        {
          QGraphicsTextItem* label = new QGraphicsTextItem(txt, m_layers[5]);
          label->setPos(center - QPoint(0.5 * bbox.width(), bbox.height() + 3));
        }
        m_vecGrid.push_back(temp);
      }
    }
  }
  else
  {
    QMessageBox::warning(nullptr, "geometry error", "unsupported geometry");
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::ERR, "unsupported geometry, should be either VERTICAL(1) or "\
                                   "HORIZONTAL(2).Orientation is : % d", (int)m_props->hexagonOrient);
  }
}


/**********************************************************************************************************************
 * Function: 
 *
 * Abstract:
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Mar 2026 (gkhuber) 
 *********************************************************************************************************************/
void terrainGen::adjustBorderSize()
{
  double_t width;
  double_t height;
  double_t twoRowHeight;
  double_t twoRowWidth;

  if (m_props->hexagonOrient == hexagon::orien::VERTICAL)
  {
    height = 2 * m_props->hexagonSize;
    width = m_props->hexagonSize * sqrt3;
    twoRowHeight = 1.75 * height;
    twoRowWidth = 2.50 * width;

    // sanity checkes, make sure map dimensions will hold hexagons
    double_t  cntHexagons = m_props->imageWidth / width;                  // how many hexagons will fit in defined width
    double_t  fractionPart = cntHexagons - floor(cntHexagons);            // is there part of a hexagon left over?
    if (fabs(fractionPart) > FLT_EPSILON)                                 // need to adjust imageWidth
    {
      m_props->imageWidth = (floor(cntHexagons)/* + 1 */) * width;
    }

    cntHexagons = m_props->imageHeight / twoRowHeight;
    fractionPart = cntHexagons - floor(cntHexagons);
    if (fabs(fractionPart) > FLT_EPSILON)                                 // need to adjust imageWidth
    {
      m_props->imageHeight = (floor(cntHexagons) + 1) * twoRowHeight;
    }
  }
  else if (m_props->hexagonOrient == hexagon::orien::HORIZONTAL)
  {
    width = 2 * m_props->hexagonSize;
    height = m_props->hexagonSize * sqrt(3);
    twoRowWidth = 1.75 * width;
    twoRowHeight = 1.50 * height;

    // sanity checkes, make sure map dimensions will hold hexagons
    // for width - must take into account the first and second rows
    double_t  cntHexagons = m_props->imageWidth / twoRowWidth;
    double_t  fractionPart = cntHexagons - floor(cntHexagons);
    if (fabs(fractionPart) > FLT_EPSILON)                                 // need to adjust imageWidth
    {
      m_props->imageWidth = (floor(cntHexagons) + 1) * twoRowWidth;
    }

    cntHexagons = m_props->imageHeight / twoRowHeight;
    fractionPart = cntHexagons - floor(cntHexagons);
    if (fabs(fractionPart) > FLT_EPSILON)                                 // need to adjust imageHeight
    {
      m_props->imageHeight = (floor(cntHexagons) + 1) * twoRowHeight;
    }
  }
  else
  {
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::WARNING, "Invalid orientation");
  }
}

