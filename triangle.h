#ifndef _triangle_h_
#define _triangle_h_

#include <QPointF>


class QGraphicsScene;

class eqTriangle
{
public:
	eqTriangle();
	eqTriangle(QPointF, QPointF, QPointF);
	~eqTriangle();

	QPointF getCenter() { return m_center; }
	float   getLength() { return m_length; }
	float   getHeight() { return m_height; }

	void drawCenter(bool dc) { m_bDrawCenter = dc; }
	
	void draw(QGraphicsScene*);


private:
	QPointF m_verticies[3];
	QPointF m_center;
	float   m_length;
	float   m_height;
	bool    m_bDrawCenter;

	void  calcCenter();
	void  calcHeight();
	float calcLength(QPointF, QPointF);
};

#endif
