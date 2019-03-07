#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QColor>
#include <QtDebug>
#include <QtMath>
#include <QTimer>

/*
 *  左右眼中心位置按照宽度计算
 *  巩膜及虹膜半径长度按照高度计算
 */
#define EYE_REFRUSH_TIME            10

#define EYE_RIGHTCENTER_X_RATIO     0.75        //右眼中心位置即总宽度*RATIO eg:800*0.75 = 600
#define EYE_LEFTCENTER_X_RATIO      0.25        //左眼中心位置即总宽度*RATIO eg:800*0.25 = 200
#define EYE_RIGHTCENTER_Y_RATIO     0.5
#define EYE_LEFTCENTER_Y_RATIO      0.5

#define EYE_SCLERA_RATIO        0.39        //巩膜半径长度即宽高最小者*RATIO eg:460*0.39 = 180

#define EYE_PUPIL_RATIO         0.17       //瞳孔半径长度即巩膜半径*RATIO eg:180*0.065 = 30

QPoint g_qpRightPupilCenter;
QPoint g_qpLeftPupilCenter;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->resizeEvent(nullptr);
    m_timer.start(EYE_REFRUSH_TIME);
    connect(&m_timer,&QTimer::timeout,this,&MainWindow::onTimerup);

    //draw background
    this->setAutoFillBackground(true);
    QPalette _pal = palette();
    _pal.setBrush(QPalette::Background,Qt::black);
    this->setPalette(_pal);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::calculatePosition(QPoint inputPoint,QPoint offsetPoint,QPoint &outputPoint)
{
    //calculate center distence
    int _distenceX = inputPoint.x() - offsetPoint.x();
    int _distenceY = inputPoint.y() - offsetPoint.y();
    double _centerDistence = sqrt(pow(double(_distenceX),2.0) + pow(double(_distenceY),2.0));

    if(_centerDistence <= (m_iScleraRadius - m_iPupilRadius))
    {
        outputPoint.setX(inputPoint.x());
        outputPoint.setY(inputPoint.y());
    }
    else
    {
        /*
         *  1.求出鼠标与眼中心的方向角的弧度
         *  2.绘制瞳孔的中心距离为巩膜半径-瞳孔半径
         *  3.坐标(a,b) a=R*cos  b=R*sin
         */
        double _targetRadian = atan2(double(_distenceY),double(_distenceX));
        _centerDistence = m_iScleraRadius - m_iPupilRadius;
        outputPoint.setX(int(_centerDistence * cos(_targetRadian)) + offsetPoint.x());
        outputPoint.setY(int(_centerDistence * sin(_targetRadian)) + offsetPoint.y());
    }

}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter _painter(this);

    //redraw eye base
    _painter.setBrush(QBrush(Qt::white));
    _painter.drawEllipse(m_qpEyeLeftCenter,m_iScleraRadius,m_iScleraRadius);
    _painter.drawEllipse(m_qpEyeRightCenter,m_iScleraRadius,m_iScleraRadius);

    this->m_bRepaintAll = false;

    //draw pupli
    _painter.setBrush(QBrush(Qt::black));
    _painter.drawEllipse(g_qpLeftPupilCenter,m_iPupilRadius,m_iPupilRadius);
    _painter.drawEllipse(g_qpRightPupilCenter,m_iPupilRadius,m_iPupilRadius);

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    m_qpEyeLeftCenter = QPoint(int(this->geometry().width() * EYE_LEFTCENTER_X_RATIO)
                               ,int(this->geometry().height() * EYE_LEFTCENTER_Y_RATIO));
    m_qpEyeRightCenter = QPoint(int((this->geometry().width()) * EYE_RIGHTCENTER_X_RATIO)
                                ,int((this->geometry().height()) * EYE_RIGHTCENTER_Y_RATIO));

    m_iScleraRadius = int(this->geometry().height() * EYE_SCLERA_RATIO);    //给定一个初始值

    //避免重叠
    if(m_iScleraRadius*4 > this->geometry().width())
        m_iScleraRadius = this->geometry().width() / 4;
    if(m_iScleraRadius*2 > this->geometry().height())
        m_iScleraRadius = this->geometry().height() / 2;

    m_iPupilRadius = int(m_iScleraRadius * EYE_PUPIL_RATIO);

    //计算双眼覆盖矩形范围
    QPoint lt(m_qpEyeLeftCenter.x() - m_iScleraRadius
              ,m_qpEyeLeftCenter.y() - m_iScleraRadius);
    QPoint rb(m_qpEyeRightCenter.x() + m_iScleraRadius
              ,m_qpEyeRightCenter.y() + m_iScleraRadius);
    m_pEyeArea = new QRect(lt,rb);

    g_qpLeftPupilCenter = m_qpEyeLeftCenter;
    g_qpRightPupilCenter = m_qpEyeRightCenter;
}

void MainWindow::onTimerup()
{
    QPoint _mousePos = this->mapFromGlobal(QCursor::pos());

    this->calculatePosition(_mousePos,m_qpEyeLeftCenter,g_qpLeftPupilCenter);
    this->calculatePosition(_mousePos,m_qpEyeRightCenter,g_qpRightPupilCenter);

    this->update(*m_pEyeArea);
}
