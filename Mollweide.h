#ifndef _mollweide_h_
#define _mollweide_h_

#include "projection.h"
#include "constants.h"
#include "logger.h"


class MollweideProj : public projection
{
public:
    MollweideProj(float_t w, float_t h, float r);
    ~MollweideProj(void);

    void setWindowSize(double_t, double_t);
    std::pair<double_t, double_t> convert(double_t lat, double_t lon);

private:
    double_t      m_winWidth;
    double_t      m_winHeigth;
    double_t      m_fRadius;
    double_t      m_fCoeff;
    double_t      solveAux(double_t, double_t);
};

#endif

