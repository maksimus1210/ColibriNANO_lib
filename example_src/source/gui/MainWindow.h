#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QtWidgets>

#include <QCustomPlot>
#include "../LibLoader/common.h"
#include "../LibLoader/LibLoader.h"
#include "../dsp/DspCore.h"
#include "ui_MainWindow.h"

using namespace std;


class MainWindow : public QMainWindow, protected Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onOpen(bool state);
    void onStart(bool state);
    void onFrequency(double value);
    void onPreamp(double value);
    void onSampleRate(int index);

    void onReadSpectrum();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    inline int sampleRate(int index) const noexcept;

private:
    QCustomPlot *pPlotter;

    Descriptor m_deskriptor { nullptr };
    LibLoader  m_loader;

    vector<Real> m_spectrum;
    QVector<double> m_x;
    QVector<double> m_y;
};

#endif // MAINWINDOW_H
