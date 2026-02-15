#ifndef _mapDisplay_h_
#define _mapDisplay_h_

#include <QGraphicsView>

class QGraphicsScene;
class QMainWindow;
class terrainGen;

class mapDisplay : public QGraphicsView
{
  Q_OBJECT

public:
  mapDisplay(QWidget* parent = nullptr);
  mapDisplay(QGraphicsScene* scene, QWidget* parent = nullptr);
  ~mapDisplay();

	void setMainWndPtr(QMainWindow*);

protected:
	void contextMenuEvent(QContextMenuEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

private:
	QWidget*           m_pParent;
	terrainGen*        m_pMainWnd = nullptr;






};

#endif

