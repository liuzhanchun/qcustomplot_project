#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

#include <QMainWindow>
#include "../src/qcustomplot.h"
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void realtimeDataSlot();
    void timeToAddData();
    void timeToRePlot();
    void on_btn_start1_clicked();
    void on_btn_adddata_clicked();
    void on_btn_start2_clicked();

private:
    Ui::MainWindow *ui;
    void init_draw1();
    void init_darw2();
    QTimer rePlotTimer;
    QTimer dataTimer1;
    QTimer dataTimer2;
    bool  m_start1;
    bool  m_start2;
    int  m_countGraph;

};

#endif // MAINWINDOW_H
