#include "DspCore.h"

DspCore& DspCore::instance()
{
    static DspCore INSTANCE;
    return INSTANCE;
}

DspCore::DspCore(QObject *parent) :
  QThread(parent)
{
    m_fft.setSize(SpectrumSize);
    m_windows.setParam(SpectrumSize);
    m_iqSpectrumBuffer.resize(SpectrumSize);
    m_spectrum.resize(SpectrumSize);
    m_signal.resize(SpectrumSize);
}

DspCore::~DspCore()
{
    close();
}

void DspCore::open()
{
    if (m_open)
        return;

    m_open = true;
    start();
}

void DspCore::close()
{
    if (!m_open)
        return;

    m_open = false;

    quit();
    wait();
}

bool DspCore::isOpen() const
{
    return m_open;
}

bool DspCore::callbackRx(Complex *pSrc, uint32_t len, bool adcOverload, void *pUserData)
{
    DspCore::instance().m_iqSpectrumBuffer.write(pSrc, len);
    DspCore::instance().setAdcOverload(adcOverload);

    return true;
}

void DspCore::run()
{
    QTimer t_timer;
    auto c = connect(&t_timer, &QTimer::timeout, this, &DspCore::process, Qt::DirectConnection);
    t_timer.start(50);

    exec();

    disconnect(c);
}

void DspCore::process()
{
    m_iqSpectrumBuffer.readAll(m_signal);

    m_windows.process(m_signal);
    m_fft.process(m_signal, true);
    rotate(m_signal.begin(), m_signal.begin() + SpectrumSize/2, m_signal.end());

    lock_guard<std::mutex> t_locker(m_mutex);

    for (size_t i = 0; i < m_signal.size(); ++i)
        m_spectrum[i] = 5*log(m_signal[i].re*m_signal[i].re + m_signal[i].im*m_signal[i].im);

    emit readyRead();
}

void DspCore::getSpectrum(vector<Real> &data)
{
    if (data.size() != SpectrumSize)
        data.resize(SpectrumSize);

    lock_guard<std::mutex> t_locker(m_mutex);

    std::copy(m_spectrum.begin(), m_spectrum.end(), data.begin());
}

void DspCore::setAdcOverload(bool state)
{
    if (m_adcOverload != state) {
        m_adcOverload = state;
        emit adcOverloadChanged(state);
    }
}









