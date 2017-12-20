#ifndef DSPCORE_H
#define DSPCORE_H

#include <cstdint>
#include <mutex>
#include <atomic>

#include <QtCore>

#include "../LibLoader/common.h"
#include "fft.h"
#include "window.h"
#include "spectrumringbuffer.h"




class DspCore : public QThread
{
    Q_OBJECT

public:
    static constexpr size_t SpectrumSize = 4096;

public:
    static DspCore& instance();

    ~DspCore();

    void open();
    void close();
    bool isOpen() const;

    static bool callbackRx(Complex *pSrc, uint32_t len, bool adcOverload, void *pUserData);

    void getSpectrum(vector<Real> &data);

signals:
    void readyRead();
    void adcOverloadChanged(bool);

private slots:
    void process();

protected:
    void run() override;

private:
    explicit DspCore(QObject *parent = nullptr);
    Q_DISABLE_COPY(DspCore);

    void setAdcOverload(bool state);

private:
    bool        m_open { false };
    atomic_bool m_adcOverload { false };

    fft    m_fft;
    Window m_windows;
    SDR::SpectrumRingBuffer<Complex> m_iqSpectrumBuffer;

    std::mutex m_mutex;

    vector<Complex> m_signal;
    vector<Real>    m_spectrum;
};

#endif // DSPCORE_H
