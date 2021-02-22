
#include "cudagoltiming.h"

namespace cudagol {
timing::timing() {
    cudaEventCreate(&m_start);
    cudaEventCreate(&m_stop);
}

timing::~timing() {
    cudaEventDestroy(m_start);
    cudaEventDestroy(m_stop);
}

void timing::start() {
    cudaEventRecord(m_start);
    //cudaEventSynchronize(m_start);
}

void timing::stop() {
    cudaEventRecord(m_stop);
    cudaEventSynchronize(m_stop);
}

float timing::ms() {
    float ms;
    cudaEventElapsedTime(&ms, m_start, m_stop);
    return ms;
}

void timing::sync() {
    cudaDeviceSynchronize();
}
}
