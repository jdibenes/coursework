
#ifndef GOLTIMING_H
#define GOLTIMING_H

#include <time.h>

namespace gol {
class timing {
private:
    timespec m_start;
    timespec m_stop;

public:
    void start();
    void stop();
    float ms();
};
}

#endif // GOLTIMING_H
