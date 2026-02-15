#pragma once

#include <QRect>
#include <QPoint>
#include <QVector>
#include <QColor>

class QGraphicsScene;

typedef unsigned char BYTE;

class CHexagon
{
public:
    enum { HORIZONTAL =0, VERTICAL = 1, NONE=2 };

    CHexagon(float, float, float, int mode = 0);
    const CHexagon& operator=(const CHexagon& rhs);
    ~CHexagon();

    static uint32_t  getNextNdx() { return m_curNdx++; }

    uint32_t         getIndex() { return m_ndx; }
    float            getSide() { return m_nSide; }
    QVector<QPointF> getVertices() { return m_verticies; }
    const char*      getLabel() { return m_pLabel; }
    QPoint           getCenter() { return QPoint(m_nX, m_nY); }
    QRect            getCntRect() { return QRect(m_nX - 1, m_nY - 1, m_nX + 1, m_nY + 1); }
    void             setState(uint8_t s) { m_nState = s; }
    uint8_t          getState() { return m_nState; }
    void             setColor(QColor c) { m_fColor = c; }
    QColor           getColor() { return m_fColor; }

    void             draw(QGraphicsScene*);

    bool             contains(QPointF);
    
    void             makeLabel(int, int, int);
    void             makeLabel(int);

private:
  static uint32_t m_curNdx;
  uint32_t        m_ndx;
  float           m_nX;
  float           m_nY;
  float           m_nSide;
  char*           m_pLabel;
  int             m_nMode;
  QColor          m_fColor;                            // color represented in RGB format
  uint8_t         m_nState;                            // flags for state bit0 : filled(1), hollow(0)   bit1 : gray(0), color(1) bit2: noCenter(0), showCenter(1) bit3: noNdx(0), showNdx(1)

    QVector<QPointF>   m_verticies;

    void calcVertices();
};
