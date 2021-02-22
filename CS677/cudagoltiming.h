
#ifndef CUDAGOLTIMING_H
#define CUDAGOLTIMING_H

#include <cuda_runtime.h>

namespace cudagol {
class timing {
private:
    cudaEvent_t m_start;
    cudaEvent_t m_stop;

public:
    timing();
    ~timing();

    void start();
    void stop();
    float ms();
    void sync();
};
}

#endif // CUDAGOLTIMING_H
