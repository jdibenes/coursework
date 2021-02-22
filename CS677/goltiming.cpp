
#include "goltiming.h"

namespace gol {
void timing::start() {
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_start);
}

void timing::stop() {
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_stop);
}

float timing::ms() {
    long diff_nsec = m_stop.tv_nsec - m_start.tv_nsec;
    long d_sec;
    long d_nsec;

    if (diff_nsec < 0) { d_sec  = m_stop.tv_sec - m_start.tv_sec - 1; d_nsec = diff_nsec + 1000000000;}
    else               { d_sec  = m_stop.tv_sec - m_start.tv_sec;     d_nsec = diff_nsec; }

    return (d_sec * 1000.0f) + (d_nsec / 1000000.0f);
}
}
