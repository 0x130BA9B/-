#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QDebug>
#include <QMainWindow>
#include<QImage>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public slots:
   void xytimerUpDate();
    void openCamaraSlot();
       void readFrameSlot();
       void closeCamaraSlot();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QImage toGray( QImage image );
    QString type1;
    QString type2;
    QString type3;
    QString openfile;
    int number;
    QTimer *timer2;
   QTimer *timer3;
   QTimer *timer4;
   int pzcount;
   int tp;
   QImage AdjustContrast(QImage Img, int iContrastValue);
void LB2diaplay(QString road);
VideoCapture capture;
QImage imgg;
Mat frame;

private slots:


       void paizhaoT();
       void paizhaoC();
   void on_pushButton_clicked();

   void on_PB2_clicked();

   void on_PB3_clicked();

   void on_CB1_activated(const QString &arg1);

   void on_CB2_activated(const QString &arg1);

   void on_CB3_activated(const QString &arg1);

   void on_pushButton_2_clicked();

   void on_PB4_clicked();

   void on_PB5_clicked();

   void on_pushButton_3_clicked();

   void on_pushButton_4_clicked();

   void on_pushButton_5_clicked();

   void on_pushButton_6_clicked();

   void on_horizontalSlider_actionTriggered(int action);

   void on_horizontalSlider_2_actionTriggered(int action);

   void on_open_clicked();

private:
    Ui::MainWindow *ui;





};


#endif // MAINWINDOW_H

