#ifndef _terrainGen_h_
#define _terrainGen_h_

#include <QMainWindow>
#include <QString>
#include <random>

#include "constants.h"
#include "mapDisplay.h"

class QMenuBar;
class QStatusBar;
class QGraphicsView;
class QGraphicsScene;
class imagePropDlg;
class CGEVDist;
class CHexagon;
class QTimer;

class terrainGen : public QMainWindow
{
    Q_OBJECT

public:

    terrainGen();
    ~terrainGen();

public slots:
    void onFileOpen();
    void onFileNew();
    void onFileClose();
    void onFileSave();
    void onFileSaveAs();
    void onFileExit();
    void onViewZoom();
    void onViewZoomIn();
    void onViewZoomOut();
    void onViewRedraw();
    void onViewImage();
    void onSimCenters();
    void onSimPlates();
    void onSimMotion();
    void onSimTimeDelta();
    void onSimRun();
    void onEditPrefs();
    void onEditPlateColors();
    void onHelpHelp();
    void onHelpAbout();
    void onSaveConfig(imagePropDlg*);
   

protected:
    void closeEvent(QCloseEvent*);
    void mouseMoveEvent(QMouseEvent* evt) override;

private:
    // main window form components
    //QGraphicsView*  m_pDisplay;
    mapDisplay* m_pDisplay;
    QGraphicsScene* m_pScene;
    QMenuBar*       m_menubar;
    QStatusBar*     m_statusbar;

    // image parameters
    uint64_t           m_imageWidth;                // these are the default values
    uint64_t           m_imageHeight;
    uint64_t           m_hexagonSize;
    uint8_t            m_hexagonOrien;
    uint8_t            m_hexagonProps;
    float              m_amplitudes[nbrHarmonics];
    float              m_frequency[nbrHarmonics];
    struct imageProps* m_props;                      // properties of the current image

    // simulation properties
    uint32_t           m_cntPlates;
    uint64_t           m_timeStep;
    uint64_t           m_maxTime;
    uint64_t           m_curTime;
    QPointF*           m_centers;
    QTimer*            m_timer = nullptr;

    // actions of menus
    QAction* m_pFileOpen;
    QAction* m_pFileNew;
    QAction* m_pFileClose;
    QAction* m_pFileSave;
    QAction* m_pFileSaveAs;
    QAction* m_pFileExit;
    QAction* m_pViewZoom;
    QAction* m_pViewZoomIn;
    QAction* m_pViewZoomOut;
    QAction* m_pViewRedraw;
    QAction* m_pViewImageProps;
    QAction* m_pEditPerfs;
    QAction* m_pEditPlateColors;
    QAction* m_pSimCenters;
    QAction* m_pSimPlates;
    QAction* m_pSimMotion;
    QAction* m_pSimTimeDelta;
    QAction* m_pSimRun;
    QAction* m_pHelpHelp;
    QAction* m_pHelpAbout;

    // maintaining state of the application
    bool                     m_bDirty;
    bool                     m_bInit = false;
    QString                  m_fileName;
    std::vector<CHexagon*>   m_vecGrid;
    platesT*                 m_plates;

    std::random_device       m_rd;
    std::mt19937*            m_gen;


    // private functions
    void readSettings();
    void writeSettings();   
    void setupUI();
    void setupActions();
    void setupMenu();

    void doSave();
    void genGrid(QPen);
    void onSimPlatesImpl();
};



#endif
