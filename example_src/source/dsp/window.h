#ifndef WINDOW_H
#define WINDOW_H

#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>

#include "../LibLoader/common.h"

using namespace std;

class Window
{
public:
    Window() = default;

    bool setParam(uint32_t t_size)
    {
        if (t_size == 0)
            return false;

        Real sn = t_size - 1;
        m_vector.resize(t_size);

        for (uint32_t i = 0; i < m_vector.size(); ++i)
            m_vector[i] = 0.3635819 - 0.4891775*cos(2*M_PI*i/sn) + 0.1365995*cos(4*M_PI*i/sn) - 0.0106411*cos(6.0*M_PI*i/sn);

        return true;
    }

    void process(vector<Complex> &srcDst)
    {
        if (srcDst.size() != m_vector.size())
            return;

        for (uint32_t i = 0; i < srcDst.size(); ++i) {
            srcDst[i].re *= m_vector[i];
            srcDst[i].im *= m_vector[i];
        }
    }

private:
    Window(const Window&) = delete;
    Window &operator=(const Window&) = delete;

private:
    std::vector<Real> m_vector;
};

#endif // WINDOW_H

