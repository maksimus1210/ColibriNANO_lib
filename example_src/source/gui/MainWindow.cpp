#include "MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
    setupUi(this);

    if (!m_loader.load(QString("colibrinano_lib.dll").toStdWString())) {
        QMessageBox::critical(this, windowTitle(), tr("Failed to load colibrinano_lib.dll!"));
        qApp->quit();
    }

    m_loader.initialize();

    m_spectrum.resize(DspCore::SpectrumSize);
    m_x.resize(DspCore::SpectrumSize);
    m_y.resize(DspCore::SpectrumSize);

    pPlotter = new QCustomPlot;
    verticalLayout->addWidget(pPlotter);

    pPlotter->addGraph();
    pPlotter->yAxis->setRange(-160, 0);

    //
    uint32_t t_devices = m_loader.devices();
    for (uint32_t i = 0; i < t_devices; ++i)
        cbDevices->addItem(QStringLiteral("ColibriNANO ") + QString::number(i + 1));

    connect(pbOpen , &QPushButton::toggled, this, &MainWindow::onOpen);
    connect(pbStart, &QPushButton::toggled, this, &MainWindow::onStart);
    connect(sbFrequency, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::onFrequency);
    connect(sbPreamp   , static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::onPreamp);
    connect(sbSampleRate, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onSampleRate);

    connect(&DspCore::instance(), &DspCore::readyRead, this, &MainWindow::onReadSpectrum);
    connect(&DspCore::instance(), &DspCore::adcOverloadChanged, adcOverload, &QRadioButton::setChecked);

    onSampleRate(sbSampleRate->currentIndex());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    m_loader.finalize();
}

void MainWindow::onOpen(bool state)
{
    if (state) {
        if (!m_loader.open(&m_deskriptor, cbDevices->currentIndex())) {
            QMessageBox::warning(this, windowTitle(), tr("Failed to open ColibriNANO!"));
            pbOpen->setChecked(false);
            return;
        }

        m_loader.setFrequency(m_deskriptor, 1000000*sbFrequency->value());
        m_loader.setPream(m_deskriptor, sbPreamp->value());
    }
    else {
        pbStart->setChecked(false);
        m_loader.stop(m_deskriptor);
        m_loader.close(m_deskriptor);

        m_deskriptor = nullptr;
    }
}

void MainWindow::onStart(bool state)
{
    if (state) {
        DspCore::instance().open();
        m_loader.start(m_deskriptor,
                       static_cast<SampleRateIndex>(sbSampleRate->currentIndex()),
                       DspCore::callbackRx,
                       &DspCore::instance());
    }
    else {
        DspCore::instance().close();
        m_loader.stop(m_deskriptor);
    }
}

void MainWindow::onFrequency(double value)
{
    m_loader.setFrequency(m_deskriptor, 1000000*value);
}

void MainWindow::onPreamp(double value)
{
    m_loader.setPream(m_deskriptor, value);
}

void MainWindow::onSampleRate(int index)
{
    int t_sampleRate = sampleRate(index);
    pPlotter->xAxis->setRange(-t_sampleRate/2, t_sampleRate/2);

    float t_step  = t_sampleRate / static_cast<double>(DspCore::SpectrumSize);
    float t_begin = -t_sampleRate/2;

    for (auto &value : m_x) {
        value = t_begin;
        t_begin += t_step;
    }
}

void MainWindow::onReadSpectrum()
{
    DspCore::instance().getSpectrum(m_spectrum);

    for (size_t i = 0; i < m_spectrum.size(); ++i)
        m_y[i] = m_spectrum[i];

    pPlotter->graph(0)->setData(m_x, m_y);
    pPlotter->replot();
}

int MainWindow::sampleRate(int index) const noexcept
{
    switch (index) {
        case Sr_48kHz: return 48000;
        case Sr_96kHz: return 96000;
        case Sr_192kHz: return 192000;
        case Sr_384kHz: return 384000;
        case Sr_768kHz: return 768000;
        case Sr_1536kHz: return 1536000;
        case Sr_1920kHz: return 1920000;
        case Sr_2560kHz: return 2560000;
        case Sr_3072kHz: return 3072000;

        default: break;
    }

    return 48000;
}
























