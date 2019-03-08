#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
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

public slots:
    void onTimerup();

private:
    Ui::MainWindow *ui;
    bool m_bRepaintAll = true;
    QTimer m_timer;

    QPoint m_qpEyeLeftCenter;
    QPoint m_qpEyeRightCenter;
    QPoint m_qpRightPupilCenter;
    QPoint m_qpLeftPupilCenter;
    int m_iScleraRadius;
    int m_iPupilRadius;

    QRect* m_pEyeArea = nullptr;

    void calculatePosition(QPoint,QPoint,QPoint&);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
