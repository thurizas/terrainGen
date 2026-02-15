#ifndef _utility_h_
#define _utility_h_

#include <cstdint>
#include <QPoint>


enum dir: std::int8_t{UNK=-1,LEFT=0, RIGHT=1, ON=2};

int8_t orient(QPointF, QPointF, QPointF);


#endif
