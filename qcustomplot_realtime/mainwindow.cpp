#include "mainwindow.h"
#include "ui_mainwindow.h"

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_draw1();//图表1初始化
    init_darw2();//图表2初始化
}

MainWindow::~MainWindow()
{
    delete ui;

}

//图表1初始化
void MainWindow::init_draw1()
{
    QCustomPlot* customPlot = ui->customPlot1;
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));//曲线1蓝色
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));//曲线2红色
    //坐标轴使用时间刻度
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    customPlot->xAxis->setTicker(timeTicker);
    //四边安上坐标轴
    customPlot->axisRect()->setupFullAxesBox();
    //设置y轴范围
    customPlot->yAxis->setRange(-1.2, 1.2);
    // 使上下轴、左右轴范围同步
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    //定时器连接槽函数realtimeDataSlot
    connect(&dataTimer1, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer1.start(0); // 间隔时间 0ms表示尽可能快的触发
    m_start1 = true;
}

void MainWindow::realtimeDataSlot()
{
    QCustomPlot* customPlot = ui->customPlot1;
    static QTime time(QTime::currentTime());

    double key = time.elapsed()/1000.0; // 开始到现在的时间，单位秒
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.002) // 大约2ms添加一次数据
    {
        // 添加数据到graph
        customPlot->graph(0)->addData(key, qSin(key)+qrand()/(double)RAND_MAX*1*qSin(key/0.3843));
        customPlot->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
        //记录当前时刻
        lastPointKey = key;
    }
    // 曲线能动起来的关键在这里，设定x轴范围为最近8个时刻
    customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    //绘图
    customPlot->replot();
    //计算帧数
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // 每2秒求一次平均值
    {
        //状态栏显示帧数和数据总数
        ui->statusBar->showMessage(
                    QString("%1 FPS, Total Data points: %2")
                    .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
                    .arg(customPlot->graph(0)->data()->size()+customPlot->graph(1)->data()->size())
                    , 0);
        lastFpsKey = key;
        frameCount = 0;
    }
}

void MainWindow::on_btn_start1_clicked()
{
    if(!m_start1)
    {
        dataTimer1.start(0);
        m_start1=true;
        ui->btn_start1->setText("停止");
    }
    else
    {
        dataTimer1.stop();
        m_start1=false;
        ui->btn_start1->setText("开始");
    }
}

//图表2初始化
void MainWindow::init_darw2()
{
    QCustomPlot* customPlot = ui->customPlot2;

    //四边安上坐标轴
    customPlot->axisRect()->setupFullAxesBox();
    // 使上下轴、左右轴范围同步
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    //定时器连接槽函数
    connect(&dataTimer2, SIGNAL(timeout()), this, SLOT(timeToAddData()));
    connect(&rePlotTimer, SIGNAL(timeout()), this, SLOT(timeToRePlot()));
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_start2 = false;
}

void MainWindow::timeToRePlot()
{
    static QTime time(QTime::currentTime());
       double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds

       QCustomPlot* customPlot = ui->customPlot2;
       int size = ui->spinBox2->value();

       customPlot->xAxis->setRange(customPlot->graph(0)->dataCount(), size, Qt::AlignRight);
       //重绘
       customPlot->replot(QCustomPlot::rpQueuedReplot);
       //customPlot->replot();
       static double lastFpsKey;
       static int frameCount;
       ++frameCount;
       if (key-lastFpsKey > 1) // 每1s求一次值
       {
           int displaySize = ui->spinBox2->value();//单条显示点数
           int datasize = customPlot->graph(0)->data()->size();//单条数据总数
           int graphCount= m_countGraph;//曲线条数
           long displaytotal=displaySize*graphCount;//显示总数
           long datatotal=datasize*graphCount;//数据总数
           QString str_displaytotal;
           QString str_datatotal;
           //显示的总数不会超过实际点数
           if(displaytotal > datatotal)
           {
               displaytotal = datatotal;
           }
           //显示点数加单位更直观
           if(displaytotal>=1e6)//百万级
           {
              str_displaytotal = QString("%1百万").arg(displaytotal/1000000.0,0,'f',2);
           }
           else if(displaytotal>=1e4)//万级
           {
               str_displaytotal = QString("%1万").arg(displaytotal/10000.0,0,'f',2);
           }
           else
           {
             str_displaytotal=QString("%1").arg(displaytotal);
           }
           //总数加单位更直观
           if(datatotal>=1e6)//百万级
           {
              str_datatotal = QString("%1百万").arg(datatotal/1000000.0,0,'f',2);
           }
           else if(datatotal>=1e4)//万级
           {
               str_datatotal = QString("%1万").arg(datatotal/10000.0,0,'f',2);
           }
           else
           {
             str_datatotal=QString("%1").arg(datatotal);
           }
           ui->statusBar->showMessage(
                       QString("FPS:%1，显示总数:%2*%3=%4，数据总数:%5*%6=%7")
                       .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
                       .arg(displaySize)
                       .arg(graphCount)
                       .arg(str_displaytotal)
                       .arg(datasize)
                       .arg(graphCount)
                       .arg(str_datatotal)
                       , 0);
           lastFpsKey = key;
           frameCount = 0;
       }
}

void MainWindow::timeToAddData()
{
    QCustomPlot* customPlot = ui->customPlot2;

      QTime time= QTime::currentTime();
      qsrand(time.msec()+time.second()*1000);//随机数种子
      for(int i=0;i<customPlot->graphCount();i++)
      {
          for(int j=0;j<10;j++)//每次添加10个数据
          {
              customPlot->graph(i)->addData(customPlot->graph(i)->dataCount(),qrand()/(double)RAND_MAX + i*2);
          }
      }
}

void MainWindow::on_btn_adddata_clicked()
{
    QCustomPlot* customPlot = ui->customPlot2;
   //x轴范围点数
   int size = ui->spinBox2->value();
   for(int i=0;i<customPlot->graphCount();i++)//添加x轴范围的点数，使波形占满画面
   {
       for(int j=0;j<size;j++)
       {
           customPlot->graph(i)->addData(customPlot->graph(i)->dataCount(),qrand()/(double)RAND_MAX + i*2);
       }
   }
}

void MainWindow::on_btn_start2_clicked()
{
    QCustomPlot* customPlot = ui->customPlot2;

       if(!m_start2)
       {
           customPlot->clearGraphs();//清除所有graph
           m_countGraph = ui->spinBox1->value();
           QPen pen;
           pen.setWidth(1);
           //添加曲线
           for(int i=0;i<m_countGraph;i++)
           {
               customPlot->addGraph();
               //颜色随机
               pen.setColor(QColor(qrand()%150+100,qrand()%150+100,qrand()%150+100));
               customPlot->graph(i)->setPen(pen);//
           }
           //设置y轴范围
           customPlot->yAxis->setRange(-1, m_countGraph*2);
           //设置x轴范围
           int size = ui->spinBox2->value();
           customPlot->xAxis->setRange(0, size, Qt::AlignRight);
           //定时添加和刷新数据
           rePlotTimer.start(16); // 间隔16ms刷新一次
           dataTimer2.start(5); // 间隔时间5ms添加一次数据
           m_start2 = true;
           ui->btn_start2->setText("停止");
       }
       else
       {
           rePlotTimer.stop(); // 停止重绘
           dataTimer2.stop(); // 停止添加数据
           m_start2 = false;
           ui->btn_start2->setText("开始");
       }
}
