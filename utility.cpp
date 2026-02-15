#include "utility.h"


int8_t orient(QPointF src, QPointF dst, QPointF pt)
{
  int8_t orientation = dir::UNK;

  double val = (dst.x() - src.x()) * (pt.y() - src.y()) - ((dst.y() - src.y()) * (pt.x() - src.x()));

  if (val < 0)   orientation = dir::LEFT;
  else if (val > 0) orientation = dir::RIGHT;
  else orientation = dir::UNK;

  return orientation;
}