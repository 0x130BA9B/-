#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QtCore>
#include<qtimer.h>
#include<QImage>
#include<QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include<tools.h>
#include<QMouseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    openfile="";
    pzcount=0;
    tp=0;
    ui->setupUi(this);


 this->setStyleSheet("background-color:rgb(152,245,255)");
    QImage in(":/images/2.PNG");
    ui->LB2->setPixmap(QPixmap::fromImage(in));

    setWindowTitle("Digital Camera~");  //tittle

    QTimer *timer = new QTimer(this);   //新建定时器
        connect(timer,SIGNAL(timeout()),this,SLOT(xytimerUpDate()));//关联定时器计满信号和相应的槽函数
        timer->start(1000);//定时器开始计时，其中1000表示1000ms即1秒

        timer2 = new QTimer(this);   //新建定时器
        connect(timer2,SIGNAL(timeout()),this,SLOT(paizhaoT()));//关联定时器计满信号和相应的槽函数
        timer3 = new QTimer(this);   //新建定时器
        connect(timer3,SIGNAL(timeout()),this,SLOT(paizhaoC()));//关联定时器计满信号和相应的槽函数

        timer4 = new QTimer(this);   //新建定时器
        connect(timer4,SIGNAL(timeout()),this,SLOT(readFrameSlot()));
        connect(ui->open,SIGNAL(clicked()),this,SLOT(openCamaraSlot()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::openCamaraSlot()
{
    capture.open(-1);
    if( !capture.isOpened())
    {
       qDebug()<< "open camera error"<<endl;
       this->close();
    }
    else
    {
        timer4->start(33);
    }
}
void MainWindow::readFrameSlot()
{
    capture >> frame;
    if(!frame.empty())
    {
        imgg = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,
                     QImage::Format_RGB888).rgbSwapped();//
        imgg = imgg.scaled(
                            ui->LB1->width(),
                            ui->LB1->height(),
                            Qt::IgnoreAspectRatio,
                            Qt::FastTransformation
                        );//调整图片大小，使图片和显示的label控件一样大

        ui->LB1->setPixmap(QPixmap::fromImage(imgg));


    }
}

void MainWindow::closeCamaraSlot()
{
    timer4->stop();
    capture.release();
    ui->LB1->setPixmap(QPixmap());
}

void MainWindow::xytimerUpDate()
  {
   QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
   QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");//设置系统时间显示格式
   ui->timelabel->setText(strTime);//在标签上显示时间
  }

void MainWindow::on_pushButton_clicked()
{
    //QImage in(":/images/1.PNG");//test
    //ui->LB1->setPixmap(QPixmap::fromImage(in));//test
    closeCamaraSlot();
    tp++;
    QString typegg;
    QString tps=QString("%1").arg(tp);
    typegg="raspistill -ex "+type1+" -awb "+type2+" -ifx "+type3+" -o image"+tps+".jpg -t 100";
        if(type1==NULL||type2==NULL||type3==NULL)
            typegg="raspistill -ex auto -awb auto -ifx none -o image"+tps+".jpg -t 100";
    QProcess::execute(typegg);
    QString filename="/home/pi/build-v1-Unnamed-Debug/image"+tps+".jpg";

     LB2diaplay(filename);
    QMessageBox d;
    d.information(this,"tips","achieve!!");
    openCamaraSlot();

}

void MainWindow::on_PB2_clicked()
{
    closeCamaraSlot();

    QString strFileNameSrc=QFileDialog::getOpenFileName(this,tr("open file"),"",tr("Images (*.PNG *.png *.xpm *.jpg);;All files(*.*)"));
    openfile =strFileNameSrc;
    //ui->LB2->setText(strFileNameSrc);
   // QImage open(strFileNameSrc);
    QImage* img=new QImage,* scaledimg=new QImage;//分别保存原图和缩放之后的图片
    if(! ( img->load(strFileNameSrc) ) ) //加载图像
    {
        QMessageBox::information(this,
                                 tr("打开图像失败"),
                                 tr("打开图像失败!"));
        delete img;
        return;
    }
    int Owidth=img->width(),Oheight=img->height();
    int Fwidth,Fheight;       //缩放后的图片大小
    ui->LB1->setGeometry(110,0,491,451);
    int Mul;            //记录图片与label大小的比例，用于缩放图片
    if(Owidth/491>=Oheight/451)
        Mul=Owidth/491;
    else
        Mul=Oheight/451;
    Fwidth=Owidth/Mul;
    Fheight=Oheight/Mul;
    *scaledimg=img->scaled(Fwidth,Fheight,Qt::KeepAspectRatio);
    ui->LB1->setPixmap(QPixmap::fromImage(*scaledimg));
   // ui->LB1->setPixmap(QPixmap::fromImage(open));
}

void MainWindow::on_PB3_clicked()
{
    closeCamaraSlot();
    QString strFileNameSrc=QFileDialog::getOpenFileName(this,tr("open file"),"",tr("Images (*PNG .png *.xpm *.jpg);;All files(*.*)"));
    QString strFileNameDem=QFileDialog::getSaveFileName(this,tr("save file"),tr("/images/unknow.jpg"),"");
    QString strSrc=strFileNameSrc;
    QString strDem=strFileNameDem;
    QMessageBox msgBox;
    QFile file(strSrc);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        msgBox.information(this,tr("tips"),tr("open false"));
    if(!file.copy(strSrc,strDem))
        msgBox.information(this,tr("tips"),tr("copy false"));
        else
        msgBox.information(this,tr("tips"),tr("copy achieve!"));
     file.close();

    openCamaraSlot();
}

void MainWindow::on_CB1_activated(const QString &arg1)
{
    type1=ui->CB1->currentText();
}

void MainWindow::on_CB2_activated(const QString &arg1)
{
    type2=ui->CB2->currentText();
}

void MainWindow::on_CB3_activated(const QString &arg1)
{
    type3=ui->CB3->currentText();
}

void MainWindow::on_pushButton_2_clicked()
{

    QImage in(openfile);
    QImage out=toGray(in);
    QImage* img=&out;

    QImage* scaledimg=new QImage;//分别保存原图和缩放之后的图片

    int Owidth=img->width(),Oheight=img->height();
    int Fwidth,Fheight;       //缩放后的图片大小
    ui->LB1->setGeometry(110,0,491,451);
    int Mul;            //记录图片与label大小的比例，用于缩放图片
    if(Owidth/491>=Oheight/451)
        Mul=Owidth/491;
    else
        Mul=Oheight/451;
    Fwidth=Owidth/Mul;
    Fheight=Oheight/Mul;
    *scaledimg=img->scaled(Fwidth,Fheight,Qt::KeepAspectRatio);
    ui->LB1->setPixmap(QPixmap::fromImage(*scaledimg));

    //ui->LB1->setPixmap(QPixmap::fromImage(out));

    tp++;
   QString tps=QString("%1").arg(tp);
    QString filesave="/home/pi/build-v1-Unnamed-Debug/image"+tps+".jpg";
    scaledimg->save(filesave);


    LB2diaplay(filesave);
}

QImage MainWindow::toGray( QImage image )
{
    int height = image.height();
    int width = image.width();
    QImage ret(width, height, QImage::Format_Indexed8);
    ret.setColorCount(256);
    for(int i = 0; i < 256; i++)
    {
        ret.setColor(i, qRgb(i, i, i));
    }
    switch(image.format())
    {
    case QImage::Format_Indexed8:
        for(int i = 0; i < height; i ++)
        {
            const uchar *pSrc = (uchar *)image.constScanLine(i);
            uchar *pDest = (uchar *)ret.scanLine(i);
            memcpy(pDest, pSrc, width);
        }
        break;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        for(int i = 0; i < height; i ++)
        {
            const QRgb *pSrc = (QRgb *)image.constScanLine(i);
            uchar *pDest = (uchar *)ret.scanLine(i);

            for( int j = 0; j < width; j ++)
            {
                 pDest[j] = qGray(pSrc[j]);
            }
        }
        break;
    }
    return ret;
}

void MainWindow::on_PB4_clicked()//冷色
{
    QImage in(openfile);
    QImage out=Tools::Cool(60,in);
    QImage* img=&out;

    QImage* scaledimg=new QImage;//分别保存原图和缩放之后的图片

    int Owidth=img->width(),Oheight=img->height();
    int Fwidth,Fheight;       //缩放后的图片大小
    ui->LB1->setGeometry(110,0,491,451);
    int Mul;            //记录图片与label大小的比例，用于缩放图片
    if(Owidth/491>=Oheight/451)
        Mul=Owidth/491;
    else
        Mul=Oheight/451;
    Fwidth=Owidth/Mul;
    Fheight=Oheight/Mul;
    *scaledimg=img->scaled(Fwidth,Fheight,Qt::KeepAspectRatio);
    ui->LB1->setPixmap(QPixmap::fromImage(*scaledimg));
   // ui->LB1->setPixmap(QPixmap::fromImage(out));
    tp++;
   QString tps=QString("%1").arg(tp);
    QString filesave="/home/pi/build-v1-Unnamed-Debug/image"+tps+".jpg";
    scaledimg->save(filesave);


    LB2diaplay(filesave);

}


QImage Tools::Cool(int delta,QImage origin)
{
    QImage *newImage=new QImage(origin.width(),origin.height(),QImage::Format_ARGB32);
    QColor oldColor;
    int r,g,b;
    for(int x=0;x<newImage->width();x++)
    {
        for(int y=0;y<newImage->height();y++)
        {
            oldColor=QColor(origin.pixel(x,y));

            r=oldColor.red();
            g=oldColor.green();
            b=oldColor.blue()+delta;

            r=qBound(0,r,255);
            g=qBound(0,g,255);

            newImage->setPixel(x,y,qRgb(r,g,b));
        }
    }
    return *newImage;
}

void MainWindow::on_PB5_clicked()//暖色
{
    QImage in(openfile);
    QImage out=Tools::Warm(60,in);
    QImage* img=&out;

    QImage* scaledimg=new QImage;//分别保存原图和缩放之后的图片

    int Owidth=img->width(),Oheight=img->height();
    int Fwidth,Fheight;       //缩放后的图片大小
    ui->LB1->setGeometry(110,0,491,451);
    int Mul;            //记录图片与label大小的比例，用于缩放图片
    if(Owidth/491>=Oheight/451)
        Mul=Owidth/491;
    else
        Mul=Oheight/451;
    Fwidth=Owidth/Mul;
    Fheight=Oheight/Mul;
    *scaledimg=img->scaled(Fwidth,Fheight,Qt::KeepAspectRatio);
    ui->LB1->setPixmap(QPixmap::fromImage(*scaledimg));
    //ui->LB1->setPixmap(QPixmap::fromImage(out));
    tp++;
   QString tps=QString("%1").arg(tp);
    QString filesave="/home/pi/build-v1-Unnamed-Debug/image"+tps+".jpg";
    scaledimg->save(filesave);


    LB2diaplay(filesave);

}


QImage Tools::Warm(int delta,QImage origin)
{
    QImage *newImage=new QImage(origin.width(),origin.height(),QImage::Format_ARGB32);
    QColor oldColor;
    int r,g,b;
    for(int x=0;x<newImage->width();x++)
    {
        for(int y=0;y<newImage->height();y++)
        {
            oldColor=QColor(origin.pixel(x,y));

            r=oldColor.red()+delta;
            g=oldColor.green()+delta;
            b=oldColor.blue();

            r=qBound(0,r,255);
            g=qBound(0,g,255);

            newImage->setPixel(x,y,qRgb(r,g,b));
        }
    }
    return *newImage;
}


QImage Tools::Brightness(int delta,QImage origin)
{
    QImage *newImage=new QImage(origin.width(),origin.height(),QImage::Format_ARGB32);
    QColor oldColor;
    int r,g,b;
    for(int x=0;x<newImage->width();x++)
    {
        for(int y=0;y<newImage->height();y++)
        {
            oldColor=QColor(origin.pixel(x,y));

            r=oldColor.red()+delta;
            g=oldColor.green()+delta;
            b=oldColor.blue()+delta;

            r=qBound(0,r,255);
            b=qBound(0,b,255);
            g=qBound(0,g,255);

            newImage->setPixel(x,y,qRgb(r,g,b));
        }
    }
    return *newImage;
}

void MainWindow::on_pushButton_3_clicked()
{
  timer2->start(3000);//定时器开始计时
}

void MainWindow::paizhaoT()//time-lapse photography
{
   // QImage in(":/images/1.PNG");//test
   // ui->LB1->setPixmap(QPixmap::fromImage(in));//test
    closeCamaraSlot();
    tp++;
    QString typegg;
    QString tps=QString("%1").arg(tp);
    typegg="raspistill  -ex "+type1+" -awb "+type2+" -ifx "+type3+" -o image"+tps+".jpg -t 100";
    if(type1==NULL|type2==NULL|type3==NULL)
        typegg="raspistill -ex auto -awb auto -ifx none -o image"+tps+".jpg -t 100";
    QProcess::execute(typegg);
    QString filename="/home/pi/build-v1-Unnamed-Debug/image"+tps+".jpg";
        LB2diaplay(filename);

    QMessageBox d;
    d.information(this,"tips","achieve!!");
    timer2->stop();
    openCamaraSlot();
}

void MainWindow::on_pushButton_4_clicked()
{
    QMessageBox mode;
    mode.information(this,"tips","auto - 自动曝光模式"
                  "night - 夜间拍摄模式"
                  "nightpreview - 夜间预览拍摄模式"
                  "backlight - 逆光拍摄模式"
                  "spotlight - 聚光灯拍摄模式"
                  "sports - 运动拍摄模式"
                  "snow - 雪景拍摄模式"
                  "beach - 海滩拍摄模式"
                  "verylong - 长时间曝光拍摄模式"
                  "fixedfps - 帧约束拍摄模式"
                  "antishake - 防抖模式"
                  "fireworks - 烟火拍摄模式"
                     "off - 关闭白平衡测算"
                     "auto - 自动模式（默认）"
                     "sun - 日光模式"
                     "cloud - 多云模式"
                     "shade - 阴影模式"
                     "tungsten - 钨灯模式"
                     "fluorescent - 荧光灯模式"
                     "incandescent - 白炽灯模式"
                     "flash - 闪光模式"
                     "horizon - 地平线模式"
                     "none - 无特效（默认）"
                     "negative - 反色图像"
                     "solarise - 曝光过度图像"
                     "posterize - 色调图像"
                     "whiteboard - 白板特效"
                     "blackboard - 黑板特效"
                     "sketch - 素描风格特效"
                     "denoise - 降噪图像"
                     "emboss - 浮雕图像"
                     "oilpaint - 油画风格特效"
                     "hatch - 草图特效"
                     "gpen - 马克笔特效"
                     "pastel - 柔化风格特效"
                     "watercolour - 水彩风格特效"
                     "film - 胶片颗粒风格特效"
                     "blur - 模糊图像"
                     "saturation - 色彩饱和图像");



}


void MainWindow::on_pushButton_5_clicked()
{

    timer3->start(500);//定时器开始计时
}
void MainWindow::paizhaoC()//continue photography
{
    //QImage in(":/images/1.PNG");//test
   // ui->LB1->setPixmap(QPixmap::fromImage(in));//test
    closeCamaraSlot();

    tp++;
    QString typegg;
    QString tps=QString("%1").arg(tp);
    typegg="raspistill -ex "+type1+" -awb "+type2+" -ifx "+type3+" -o image"+tps+".jpg -t 100";
    if(type1==NULL|type2==NULL|type3==NULL)
        typegg="raspistill -ex auto -awb auto -ifx none -o image"+tps+".jpg -t 100";
    QProcess::execute(typegg);
    QString filename="/home/pi/build-v1-Unnamed-Debug/image"+tps+".jpg";
    //QImage out3(outf3);
      LB2diaplay(filename);


    //ui->LB2->setPixmap(QPixmap::fromImage(*imgScaled));//test
    QMessageBox d;

    pzcount++;
    if(pzcount==3)
    {
        timer3->stop();
        d.information(this,"tips","achieve!!");
    }
    openCamaraSlot();

}

void MainWindow::on_pushButton_6_clicked()
{
    closeCamaraSlot();
     tp++;
    QString typegg;
    QString tps=QString("%1").arg(tp);
    typegg="raspivid -t 5000 -o video"+tps+".h264 -ex "+type1+" -awb "+type2+" -ifx "+type3;
    if(type1==NULL|type2==NULL|type3==NULL)
        typegg="raspivid -t 5000 -o video"+tps+".h264 -ex auto -awb auto -ifx none";
     QProcess::execute(typegg);
     QMessageBox d;
     d.information(this,"tips","achieve!!");
     openCamaraSlot();
}

void MainWindow::on_horizontalSlider_actionTriggered(int action)
{
    ui->horizontalSlider->setRange(-100,100);
    QImage in(openfile);
    number=ui->horizontalSlider->value();
    QImage out=Tools::Brightness(number,in);
    QImage* img=&out;

    QImage* scaledimg=new QImage;//分别保存原图和缩放之后的图片

    int Owidth=img->width(),Oheight=img->height();
    int Fwidth,Fheight;       //缩放后的图片大小
    ui->LB1->setGeometry(110,0,491,451);
    int Mul;            //记录图片与label大小的比例，用于缩放图片
    if(Owidth/491>=Oheight/451)
        Mul=Owidth/491;
    else
        Mul=Oheight/451;
    Fwidth=Owidth/Mul;
    Fheight=Oheight/Mul;
    *scaledimg=img->scaled(Fwidth,Fheight,Qt::KeepAspectRatio);
    ui->LB1->setPixmap(QPixmap::fromImage(*scaledimg));

    tp++;
   QString tps=QString("%1").arg(tp);
    QString filesave="/home/pi/build-v1-Unnamed-Debug/image"+tps+".jpg";
    scaledimg->save(filesave);


    LB2diaplay(filesave);
}

QImage MainWindow::AdjustContrast(QImage Img, int iContrastValue)
{
    int pixels = Img.width() * Img.height();
    unsigned int *data = (unsigned int *)Img.bits();

    int red, green, blue, nRed, nGreen, nBlue;

    if (iContrastValue > 0 && iContrastValue < 100)
    {
        float param = 1 / (1 - iContrastValue / 100.0) - 1;

        for (int i = 0; i < pixels; ++i)
        {
            nRed = qRed(data[i]);
            nGreen = qGreen(data[i]);
            nBlue = qBlue(data[i]);

            red = nRed + (nRed - 127) * param;
            red = (red < 0x00) ? 0x00 : (red > 0xff) ? 0xff : red;
            green = nGreen + (nGreen - 127) * param;
            green = (green < 0x00) ? 0x00 : (green > 0xff) ? 0xff : green;
            blue = nBlue + (nBlue - 127) * param;
            blue = (blue < 0x00) ? 0x00 : (blue > 0xff) ? 0xff : blue;

            data[i] = qRgba(red, green, blue, qAlpha(data[i]));
        }
    }
    else
    {
        for (int i = 0; i < pixels; ++i)
        {
            nRed = qRed(data[i]);
            nGreen = qGreen(data[i]);
            nBlue = qBlue(data[i]);

            red = nRed + (nRed - 127) * iContrastValue / 100.0;
            red = (red < 0x00) ? 0x00 : (red > 0xff) ? 0xff : red;
            green = nGreen + (nGreen - 127) * iContrastValue / 100.0;
            green = (green < 0x00) ? 0x00 : (green > 0xff) ? 0xff : green;
            blue = nBlue + (nBlue - 127) * iContrastValue / 100.0;
            blue = (blue < 0x00) ? 0x00 : (blue > 0xff) ? 0xff : blue;

            data[i] = qRgba(red, green, blue, qAlpha(data[i]));
        }
    }

    return Img;
}


void MainWindow::on_horizontalSlider_2_actionTriggered(int action)
{
   ui->horizontalSlider_2->setRange(-100,100);

        int dbd=ui->horizontalSlider_2->value();
    QImage in(openfile);
     QImage out=AdjustContrast(in,dbd);
    QImage* img=&out;

    QImage* scaledimg=new QImage;//分别保存原图和缩放之后的图片

    int Owidth=img->width(),Oheight=img->height();
    int Fwidth,Fheight;       //缩放后的图片大小
    ui->LB1->setGeometry(110,0,491,451);
    int Mul;            //记录图片与label大小的比例，用于缩放图片
    if(Owidth/491>=Oheight/451)
        Mul=Owidth/491;
    else
        Mul=Oheight/451;
    Fwidth=Owidth/Mul;
    Fheight=Oheight/Mul;
    *scaledimg=img->scaled(Fwidth,Fheight,Qt::KeepAspectRatio);
    ui->LB1->setPixmap(QPixmap::fromImage(*scaledimg));

    tp++;
   QString tps=QString("%1").arg(tp);
    QString filesave="/home/pi/build-v1-Unnamed-Debug/image"+tps+".jpg";
    scaledimg->save(filesave);


    LB2diaplay(filesave);

}


void MainWindow::LB2diaplay(QString road)
{
        QString filel=road;
    QImage* img2=new QImage,* scaledimg2=new QImage;//分别保存原图和缩放之后的图片
    if(! ( img2->load(filel) ) ) //加载图像
    {
        QMessageBox::information(this,
                                 tr("打开图像失败"),
                                 tr("打开图像失败!"));
        delete img2;
        return;
    }
    int Owidth2=img2->width(),Oheight2=img2->height();
    int Fwidth2,Fheight2;       //缩放后的图片大小
    ui->LB2->setGeometry(611,39,181,141);
    int Mul2;            //记录图片与label大小的比例，用于缩放图片
    if(Owidth2/181>=Oheight2/141)
        Mul2=Owidth2/181;
    else
        Mul2=Oheight2/141;
    Fwidth2=Owidth2/Mul2;
    Fheight2=Oheight2/Mul2;
    *scaledimg2=img2->scaled(Fwidth2,Fheight2,Qt::KeepAspectRatio);
    ui->LB2->setPixmap(QPixmap::fromImage(*scaledimg2));

}

void MainWindow::on_open_clicked()
{

}
