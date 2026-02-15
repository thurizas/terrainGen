#include "mapDisplay.h"
#include "terrainGen.h"

#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QStatusBar>

mapDisplay::mapDisplay(QWidget* p) : QGraphicsView(p)
{
  
}

mapDisplay::mapDisplay(QGraphicsScene* scene, QWidget* p) : QGraphicsView(p)
{

}

mapDisplay::~mapDisplay()
{

}

void mapDisplay::setMainWndPtr(QMainWindow* mw)
{
	m_pMainWnd = dynamic_cast<terrainGen*>(mw);
}


/**********************************************************************************************************************
* function  :
*
* abstract  :
*
* parameters:
*
* retuns    :
*
* written   : Feb2025 (gkhuber)
**********************************************************************************************************************/
void mapDisplay::contextMenuEvent(QContextMenuEvent*)
{
	qDebug() << "in context menu handler";
}


/**********************************************************************************************************************
* function  : wheelEvent
*
* abstract  : This handles operations with the mouse wheel.  An unmodified wheel event causes the immage to scroll
*             veritcally (the default behavior).  If the control (CTRL) button is held down, then the image is zoomed
*             either in or out based on the direction of the scroll wheel turning.  Finally, if the alternate key (ALT)
*             is held down we scroll either left or right.
*
* parameters:
*
* retuns    :
*
* written   : Feb2025 (gkhuber)
**********************************************************************************************************************/
void mapDisplay::wheelEvent(QWheelEvent* evt)
{
	float zoomFactor = 1.15;
	int   pixelsperclick = 4;

	float deltaX = evt->angleDelta().x();
	float deltaY = evt->angleDelta().y();

	if (Qt::AltModifier == evt->modifiers())
	{
		if (deltaX > 0) scroll(-pixelsperclick, 0);
		else scroll(pixelsperclick, 0);
	}
	else if (Qt::ControlModifier == evt->modifiers())                      // zooming in/out ...
	{
		if (deltaY > 0) scale(zoomFactor, zoomFactor);
		else scale(1 / zoomFactor, 1 / zoomFactor);
	}
	else
	{
		if (deltaY > 0) scroll(0, pixelsperclick);
		else scroll(0, -pixelsperclick);
	}

	evt->accept();
}


/**********************************************************************************************************************
* function  : mouseMoveEvent
*
* abstract  : By default this gets the current cursor location and shows the location in the status bar.
*             if the left-mouse button is held down, then we drag the map image.
*
* parameters:
*
* retuns    :
*
* written   : Feb2025 (gkhuber)
**********************************************************************************************************************/
void mapDisplay::mouseMoveEvent(QMouseEvent* evt)
{
	const int margin = 20;                                  // margin width in pixels

	QPointF   sceneLoc = mapToScene(evt->pos());

	QString strStatus = QString("cursor at %1, %2").arg(sceneLoc.x()).arg(sceneLoc.y());
	m_pMainWnd->statusBar()->showMessage(strStatus);


}

/**********************************************************************************************************************
* function  : mousePressEvent
*
* abstract  :
*
* parameters:
*
* retuns    :
*
* written   : Feb2025 (gkhuber)
**********************************************************************************************************************/
void mapDisplay::mousePressEvent(QMouseEvent* evt)
{
	QGraphicsView::mousePressEvent(evt);
}

/**********************************************************************************************************************
* function  : mouseReleaseEvent
*
* abstract  :
*
* parameters:
*
* retuns    :
*
* written   :  Feb2025 (gkhuber)
**********************************************************************************************************************/
void mapDisplay::mouseReleaseEvent(QMouseEvent* evt)
{
	QGraphicsView::mouseReleaseEvent(evt);
}
