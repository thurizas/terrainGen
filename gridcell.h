#pragma once
#include <QPoint>

class QGraphicsScene;

class CGridCell
{
public:
    enum    {SQUARE, HEXAGONAL};

    CGridCell(int r, int c, int t) : cntRows(r), cntCols(c), type(t) {}
    CGridCell() : cntRows(0), cntCols(0), type(0) {}
    ~CGridCell() {}

    void setRows(int r) { cntRows = r; }
    int  getRows() { return cntRows; }
    void setCols(int c) { cntCols = c; }
    int getCols() { return cntCols; }

    virtual int getActiveNeighbors(int, int) = 0;
    virtual void setState(int, int, bool) = 0;
    virtual bool getState(int, int) = 0;

    virtual void setCenter(QPoint ptCen) = 0;
    virtual void setCenter(int x, int y) = 0;
    virtual QGraphicsScene* drawGrid(QGraphicsScene*, bool bLabel = false) = 0;
    virtual void    calcVertices() = 0;
    virtual void clear()=0;

private:
    int     cntRows;
    int     cntCols;
    int     type;



};
