#ifndef FFT_H
#define FFT_H

#define _USE_MATH_DEFINES

#include <vector>
#include <cmath>
#include <cstdint>

#include "../LibLoader/common.h"


using namespace std;

/**
 * \class fft
 * \brief класс преобразования Фурье.
 */
class fft
{
    inline uint32_t powToNext(uint32_t d) noexcept
    {
        --d;
        d |= d >> 1;
        d |= d >> 2;
        d |= d >> 4;
        d |= d >> 8;
        d |= d >> 16;
        ++d;
        return d;
    }

public:
    /**
     * \brief Конструктор класса по умолчанию.
     * \param t_size - размер БПФ.
     */
    explicit fft(uint32_t t_size = 16) :
    m_size(0),
    m_logN(0),
    m_k(1)
  {
      m_size = powToNext(t_size);
      init();
  }

    bool setSize(uint32_t t_size)
    {
        if (t_size == 0u)
            return false;

        if (m_size == t_size)
            return true;

        m_size = powToNext(t_size);
        init();

        return true;
    }

    uint32_t size() const noexcept
    {
        return m_size;
    }

    bool process(vector<Complex> &srcDst, bool fwd) noexcept
    {
        if ((srcDst.size() != m_size) || (m_size == 0))
            return false;

        if (fwd) {
            forward(srcDst);

            for (auto &sample : srcDst) {
                sample.re *= m_k;
                sample.im *= m_k;
            }
        }
        else {
            backward(srcDst);
        }

        return true;
    }

private:
    fft(const fft &) = delete;
    fft &operator=(const fft &) = delete;

    void init()
    {
        m_k = 1.0/m_size;
        m_logN = round(log10(m_size)/log10(2.0));
        m_coeff.resize(m_logN);

        // вычисление коэффициентов
        for (uint32_t i = 2, j = 0; i <= m_size; i <<= 1, ++j) {
            Real ang = 2.0*M_PI/i;
            m_coeff[j].re = cos(ang);
            m_coeff[j].im = sin(ang);
        }
    }

    void forward(vector<Complex> &srcDst) noexcept
    {
        uint32_t i, j, k, io, ie, in, nn;
        Real ru, iu, rtp, itp, rtq, itq, rw, iw, sr;

        nn = m_size >> 1;
        ie = m_size;

        for (uint32_t n = 1; n <= m_logN; ++n) {
            rw = m_coeff[m_logN - n].re;
            iw = m_coeff[m_logN - n].im;

            in = ie >> 1;
            ru = 1;
            iu = 0;

             for (j = 0; j < in; ++j) {
                 for (i = j; i < m_size; i += ie) {
                     io  = i + in;
                     rtp = srcDst[i].re + srcDst[io].re;
                     itp = srcDst[i].im + srcDst[io].im;
                     rtq = srcDst[i].re - srcDst[io].re;
                     itq = srcDst[i].im - srcDst[io].im;
                     srcDst[io].re = rtq*ru - itq*iu;
                     srcDst[io].im = itq*ru + rtq*iu;
                     srcDst[i].re  = rtp;
                     srcDst[i].im  = itp;
                 }
                 sr = ru;
                 ru = ru*rw - iu*iw;
                 iu = iu*rw + sr*iw;
             }
             ie >>= 1;
         }

         for (j = i = 1; i < m_size; ++i) {
             if (i < j) {
                 io  = i - 1;
                 in  = j - 1;
                 rtp = srcDst[in].re;
                 itp = srcDst[in].im;
                 srcDst[in].re = srcDst[io].re;
                 srcDst[in].im = srcDst[io].im;
                 srcDst[io].re = rtp;
                 srcDst[io].im = itp;
             }
             k = nn;
             while (k < j) {
                 j = j - k;
                 k >>= 1;
             }
             j = j + k;
         }
    }

    void backward(vector<Complex> &srcDst) noexcept
    {
        uint32_t i, j, n, k, io, ie, in, nn;
        Real ru, iu, rtp, itp, rtq, itq, rw, iw, sr;

        nn = m_size >> 1;
        ie = m_size;

        for (n = 1; n <= m_logN; ++n) {
            rw =  m_coeff[m_logN - n].re;
            iw = -m_coeff[m_logN - n].im;

            in = ie >> 1;
            ru = 1;
            iu = 0;

             for (j = 0; j < in; ++j) {
                 for (i = j; i < m_size; i += ie) {
                     io  = i + in;
                     rtp = srcDst[i].re + srcDst[io].re;
                     itp = srcDst[i].im + srcDst[io].im;
                     rtq = srcDst[i].re - srcDst[io].re;
                     itq = srcDst[i].im - srcDst[io].im;
                     srcDst[io].re = rtq*ru - itq*iu;
                     srcDst[io].im = itq*ru + rtq*iu;
                     srcDst[i].re  = rtp;
                     srcDst[i].im  = itp;
                 }
                 sr = ru;
                 ru = ru*rw - iu*iw;
                 iu = iu*rw + sr*iw;
             }
             ie >>= 1;
         }

         for (j = i = 1; i < m_size; ++i) {
             if (i < j) {
                 io  = i - 1;
                 in  = j - 1;
                 rtp = srcDst[in].re;
                 itp = srcDst[in].im;
                 srcDst[in].re = srcDst[io].re;
                 srcDst[in].im = srcDst[io].im;
                 srcDst[io].re = rtp;
                 srcDst[io].im = itp;
             }
             k = nn;
             while (k < j) {
                 j = j - k;
                 k >>= 1;
             }
             j = j + k;
         }
    }

private:
    uint32_t m_size;
    uint32_t m_logN;
    Real     m_k;

    vector<Complex> m_coeff;
};


#endif // FFT_H
