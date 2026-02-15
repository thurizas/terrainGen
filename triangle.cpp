#include "triangle.h"
#include "constants.h"

#include <QGraphicsScene>

static float max(float f1, float f2) { return (f1 > f2 ? f1 : f2); }

/************************************************************************************************************************
 * function  : 
 *
 * abstract  : This constructs a defalut equilateral triangle with side length of 1, and the center at the origin.
 *             The points are (0, sqrt(3)/4), (-0.5, -sqrt(3)/4), and (0.5, -sqrt(3)/4).  We use the following to check
 *             for floating point comparison (see Dickheiser, Michael Ed. _Game_Programming_Gems_6_, 2006)
 * 
 *             fabs(f1 - f2) < max(f1, f2)*epsilion
 * 
 *             to allow for scaling 
 *
 * parameters: none
 *
 * returns   : none
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
eqTriangle::eqTriangle() : m_verticies{ {0, 0.25 * sqrt3}, {-0.5, -0.25 * sqrt3 }, {0.5, -0.25 * sqrt3 } }, m_length(0), m_bDrawCenter(false)
{
	
	float l1 = calcLength(m_verticies[0], m_verticies[1]);
	float l2 = calcLength(m_verticies[1], m_verticies[2]);
	float l3 = calcLength(m_verticies[2], m_verticies[0]);

	if ((fabs(l1 - l2) <= max(l1,l2)*epsilion) && (fabs(l2 - l3) <= max(l1, l2) * epsilion) && (fabs(l3 - l1) <= max(l1, l2) * epsilion))
	{
		m_length = sqrt(l1);
		calcHeight();
		calcCenter();
	}
	else
	{
		// TODO : handle error in triangle input...
	}
}



/************************************************************************************************************************
 * function  : 
 *
 * abstract  : This construct an equilateral triangle given the three verticies.  A sanity check is performed  to verify
 *             that the sides are equal length
 *
 * parameters: v1 -- [in] QPoint object containing the first vertex
 *             v2 -- [in[ QPoint object containing the second vertex
 *             v3 -- [in] QPoint object containing the third vertex
 *
 * returns   : none
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
eqTriangle::eqTriangle(QPointF v1, QPointF v2, QPointF v3) : m_verticies{ v1, v2, v3 }
{
	float l1 = calcLength(m_verticies[0], m_verticies[1]);
	float l2 = calcLength(m_verticies[1], m_verticies[2]);
	float l3 = calcLength(m_verticies[2], m_verticies[0]);

	if ((fabs(l1 - l2) <= max(l1, l2) * epsilion) && (fabs(l2 - l3) <= max(l1, l2) * epsilion) && (fabs(l3 - l1) <= max(l1, l2) * epsilion))
	{
		m_length = sqrt(l1);
		calcHeight();
		calcCenter();


		// TODO : check ccw winding -- correct if necessary
	}
	else
	{
		// TODO : handle error in triangle input...
	}
}


eqTriangle::~eqTriangle() { }



/************************************************************************************************************************
 * function  : draw 
 *
 * abstract  : This function adds the triangle to the graphics scene for rendering.  The verticies are assumed to be 
 *             counter-clockwise winding order.  We construct a closed-polygon that is then added to the scene.
 *
 * parameters: pScene -- [in] pointer to the scene to add to.
 *
 * returns   :
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void eqTriangle::draw(QGraphicsScene* pScene)
{
	QPolygonF  t1;

	t1 << m_verticies[0] << m_verticies[1] << m_verticies[2] << m_verticies[0];

	if (m_bDrawCenter)
	{
		double rad = 1;
		pScene->addEllipse(m_center.x() - rad, m_center.y() - rad, rad * 2.0, rad * 2.0, QPen(Qt::red), QBrush(Qt::SolidPattern));
	}

	pScene->addPolygon(t1);
}



/************************************************************************************************************************
 * function  : calcCenter 
 *
 * abstract  : This function calculates centroid of the triangle (based on the intersection of the bisectors of
 *             the side is located).   this is given by (v1 + v2 +v3)/3
 *
 * parameters: void
 *
 * returns   : void 
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void eqTriangle::calcCenter()
{
	m_center = (m_verticies[0] + m_verticies[1] + m_verticies[2]) / 3;
}



/************************************************************************************************************************
 * function : calcLength
 *
 * abstract  : This function calculates the square of the length of a side.  We avoid doing a square root operation 
 *             until it is needed.  In most cases checking the length squard is sufficient (i.e. confirming that we 
 *             have an equilateral triangle).
 *
 * parameters: v1 -- [in] QPoint containing first vertex
 *             v2 -- [in] Qpoint containing second vertex
 *
 * returns   : float
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
float eqTriangle::calcLength(QPointF v1, QPointF v2)
{
	float l2 = (v1.x() - v2.x()) * (v1.x() - v2.x()) + (v1.y() - v2.y()) * (v1.y() - v2.y());
	return l2;
}



/************************************************************************************************************************
 * function  : calcHeight  
 *
 * abstract  : This function returns the height of the triangle.  The height of an equilateral triangle is given by
 *             l*sqrt(3) / 4 by the following derivation:
 * 
 *                                      C                let the line CD be a bisector of angle C
 *                                     /\ 
 *                                    /  \               The triangle CDB is a right triangle, with angle D = 90
 *                                   /    \              we know that side BC has a length of `l'
 *                                  /      \             we know that side BD has a length of `l/2'
 *                                 /        \            let side CD have a length of x
 *                                /__________\           by pytherogean theorem we have l^2 = x^2 + (l/2)^2 or
 *                               A      D      B            x^2 = 3*l/4 or x = l*sqrt(3)/4
 *
 * parameters: void
 *
 * returns   : void 
 *
 * written   : Dec 2021 (GKHuber)
************************************************************************************************************************/
void eqTriangle::calcHeight()
{
	m_height = m_length * sqrt3 * 0.25;
}