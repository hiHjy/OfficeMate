#include "faceattendence.h"
#include "ui_faceattendence.h"

#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QByteArray>
#include "face_api.h"
#include <QMetaType>
Q_DECLARE_METATYPE(cv::Mat)

FaceAttendence* FaceAttendence::self = nullptr;

FaceAttendence::FaceAttendence(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FaceAttendence)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    // ----------------------------------------------------------------------
    qRegisterMetaType<cv::Mat>("cv::Mat");
    self = this;
    ui->widget_2->hide();
    old_x = ui->head_cap_img->x();
    old_y = ui->head_cap_img->y();

    DatabaseManager *db = DatabaseManager::getInstance();
    if (!db->initDataBase()) {
        qDebug() << "initDataBase error" << endl;
    }


    cap.open("/dev/video1");
    //startTimer(100);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &FaceAttendence::updateFrame);
    timer->start(100); // 大约 30 FPS


    cap >> frame;
    if (frame.empty()) {
        qDebug() << "等待有效帧" ;
        return;
    }

    cascade.load("./haarcascade_frontalface_default.xml");

    face_dect = new Work(this, &frame, &cascade);
    workThreadConnection();
    //face_dect->terminate();

    std::vector<uchar> buf;
    cv::imencode(".jpg", frame, buf);

    /* jpg转为base64 */

    QByteArray jpgData(reinterpret_cast<const char*>(buf.data()), buf.size());
    QString jpgBase64 = jpgData.toBase64();
    getAccessToken([this, jpgBase64](QString token){
        if(token.isEmpty()){
            qDebug() << "token 获取失败";
            return;
        }
        global_token = token;
        qDebug () << "token:\n" << token;
        face_dect->start();
        //faceSearch(jpgBase64, global_token);
    });

    connect(this, &FaceAttendence::sigFaceCrop, this, [this](QImage img ){

        int size =  ui->head_image->width();   // 圆形头像的直径
        QImage scaled = img.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        ui->head_image->setPixmap(QPixmap::fromImage(scaled));
        //        FaceAttendence::getInstance()->status = false;
        //        FaceAttendence::getInstance()->frame_success.data = 0x0;
        {
            QMutexLocker locker(&frameSuccessMutex);
            frame_success.release();
        }


        //        this->setStatus(false);

        qDebug() << "sigFaceCrop::被调用";

    });

    connect(this, &FaceAttendence::sigFaceVerified, this, [=](UserInfo user){
        //ui->widget_2->setText("认证成功：" + name);
        detectionSuccess = true;
        ui->widget_2->show();
        if (!user.valid) {
            qDebug() << "User not found. The user may not be registered" << endl;

        } else {


            stopCamera();
            ui->head_cap_img->hide();
            qDebug() << "识别成功：";
            qDebug() << "姓名:" << user.name;
            qDebug() << "工号:" << user.workId;
            qDebug() << "身份:" << user.identity;
            qDebug() << "学院"  << user.dept;
            ui->LE_Name->setText(user.name);
            ui->LE_Work_ID->setText(user.workId);
            ui->LE_identity->setText(user.identity);
            ui->LE_Dept->setText(user.dept);

            //识别成功后存入数据库，记录访客时间
            if (!DatabaseManager::getInstance()->insertVisitRecord(user.workId)) {

                QMessageBox::warning(this, "错误", "记录访客导数据库失败！");
                return;
            }

            QTimer::singleShot(5000, this, &FaceAttendence::startCamera);



        }

        //程序执行到这里检测完成了，这个时候需要更新状态让检测线程再次启动

    });
    connect(this, &FaceAttendence::sigCropReady, this, [=](){


        qDebug() << "切割调用";
        cv::Mat candidate;
        {
            QMutexLocker locker(&FaceAttendence::getInstance()->frameSuccessMutex);
            candidate = FaceAttendence::getInstance()->frame_success;
        }
        cv::Mat gray;
        std::vector<cv::Rect> faces;
        if (candidate.empty()) {
            qDebug() << "候选帧为空";
            return;
        }
        if (candidate.empty()) {

            qDebug() << "line:" << __LINE__
                     << "fun:"  << __FUNCTION__
                     <<"file:"  << __FILE__
                    << "candidate empty";
        }
        cv::cvtColor(candidate, gray, cv::COLOR_BGR2GRAY);
        cascade.detectMultiScale(
                    gray,
                    faces,
                    1.2,        // scaleFactor
                    5,          // minNeighbors
                    0,
                    cv::Size(60, 60),   // minSize
                    cv::Size()          // maxSize
                    );
        cv::Rect faceRect;
        if (faces.size() > 0) {
            faceRect = faces[0];

            for(cv::Rect &face : faces) {
                if (faceRect.area() < face.area()) {
                    faceRect = face;
                }
            }

            faceRect &= cv::Rect(0, 0, candidate.cols, candidate.rows);
            cv::Mat faceROI = candidate(faceRect).clone();

            cv::Mat rgb;
            if (faceROI.empty()) {

                qDebug() << "line:" << __LINE__
                         << "fun:"  << __FUNCTION__
                         <<"file:"  << __FILE__
                        << "faceROI empty";
            }
            cv::cvtColor(faceROI, rgb, cv::COLOR_BGR2RGB);
            QImage img((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
            emit sigFaceCrop(img);




            qDebug()<<"end1111";
        }


    });

}

void FaceAttendence::workThreadConnection()
{
    connect(face_dect, &Work::sigFaceReady, this, [=](QString base64, cv::Mat frame) {
        qDebug() << "connect faceSearch";
        {
            QMutexLocker loker(&this->frameSuccessMutex);
            this->frame_success = frame.clone();

        }
        faceSearch(base64, global_token);
        //        qDebug() << "frame：data： " << frame.data << endl;
        //        qDebug() << "frame_success：data： " << frame_success.data << endl;
        //        if (status == true) {
        //            frame_success = frame;
        //        }

    });

    connect(face_dect, &Work::sigFaceTrace, this, [=](int x, int y, bool status) {

        if (status)
            ui->head_cap_img->move(x, y);
        else {

            ui->head_cap_img->move(old_x, old_y);
        }

    });



    qDebug() << "连接信号和槽函数";


}

void FaceAttendence::updateFrame()
{
    cv::Mat tmp;
    cap >> frame;

    {
        QMutexLocker locker(&frameMutex);
        tmp = frame;
    }
    if (tmp.empty()) {

        qDebug() << "line:" << __LINE__
                 << "fun:"  << __FUNCTION__
                 <<"file:"  << __FILE__
                << "tmp empty";
    }
    cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);
    QImage image(tmp.data, tmp.cols, tmp.rows,  tmp.step1(),QImage::Format_RGB888);

    ui->videoLb->setPixmap(QPixmap::fromImage(image));
    //    ui->videoLb->setPixmap(QPixmap::fromImage(image).scaled(ui->videoLb->size(),
    //                                        Qt::KeepAspectRatio,
    //                                        Qt::SmoothTransformation));

}


FaceAttendence *FaceAttendence::getInstance()
{
    return self;
}

void FaceAttendence::startCamera()
{

    ui->head_cap_img->show();
    ui->head_cap_img->move(old_x, old_y);
    ui->widget_2->hide();
    ui->head_image->clear();
    ui->LE_Dept->clear();
    ui->LE_Name->clear();
    ui->LE_Work_ID->clear();
    ui->LE_identity->clear();
    if (!cap.isOpened()) {
        cap.open("/dev/video1");
    }
    //    if (cap.isOpened()) {
    //       qDebug() << "摄像头打开失败" << "file:" << __FILE__ << " line:" << __LINE__ << endl;
    //       return;
    //    }
    if (!face_dect) {
        face_dect = new Work(this, &frame, &cascade);
        workThreadConnection();
        face_dect->start(); //启动线程

    }
    timer->start(33);
}

void FaceAttendence::stopCamera()
{
    if (face_dect) {
        face_dect->requestInterruption();
        face_dect->wait();
        delete face_dect;
        face_dect = nullptr;
    }

    timer->stop();
    while(cap.isOpened())
        cap.release();

    qDebug() << "摄像头已经关闭" << endl;



}





Work::Work(QWidget *parent, cv::Mat *frame, cv::CascadeClassifier *cascade, QMutex *frameMutex)
    :
      QThread(parent),
      frame(frame),
      cascade(cascade),
      frameMutex(frameMutex)
{
    // 可以在这里进行初始化
}


FaceAttendence::~FaceAttendence()
{
    if (face_dect) {
        face_dect->requestInterruption();
        face_dect->wait(); // 等待线程结束
        delete face_dect;
        face_dect = nullptr;
    }
    cap.release();
    delete ui;

}

//void FaceAttendence::setStatus(bool v)
//{


//    status.store(v);
//    qDebug() << "setStatus: "  << v << "当前线程:" << QThread::currentThread();

//}

void Work::run()
{
    std::cout << "work start" << std::endl;
    cv::Mat gray;
    cv::Mat work_frame;
    while (!isInterruptionRequested()) {
        {
            QMutexLocker locker(frameMutex);
            work_frame = *frame;

        }


        if (work_frame.empty()) {

            qDebug() << "line:" << __LINE__
                     << "fun:"  << __FUNCTION__
                     <<"file:"  << __FILE__
                    << "work_frame empty";
        }
        quint64 t1 = QDateTime::currentMSecsSinceEpoch();
        cv::cvtColor(work_frame, gray, cv::COLOR_BGR2GRAY);
        qDebug() << "cvtColor time:" << QDateTime::currentDateTime() - t1;


        // --- 2) Haar 人脸检测 ---
        std::vector<cv::Rect> faces;
        t1 = QDateTime::currentMSecsSinceEpoch();
        this->cascade->detectMultiScale(gray,
                                        faces,
                                        1.2,        // scaleFactor
                                        5,          // minNeighbors
                                        0,
                                        cv::Size(60, 60),   // minSize
                                        cv::Size()          // maxSize
                                        );
       qDebug() << "detectMultiScale time:" << QDateTime::currentDateTime() - t1;

        Rect rect;
        if (faces.empty() ) {

            emit sigFaceTrace(0, 0, false);
            usleep(100000);
            continue;
        }

        qDebug("检测到人脸");

        rect = faces[0];

        for (size_t i = 1; i < faces.size(); ++i) {
            if (faces[i].area() > rect.area()) {
                rect = faces[i];
            }
        }




        //cv::rectangle(*this->frame, face, cv::Scalar(0, 255, 0), 2);
        emit sigFaceTrace(rect.x, rect.y, true);
        //qDebug() << "status：" << FaceAttendence::getInstance()->status.load() << "frame empty:" << FaceAttendence::getInstance()->frame_success.empty() << endl;



        //        cv::Rect faceRect = rect;

        //        faceRect &= cv::Rect(0, 0, work_frame.cols, work_frame.rows);
        //        cv::Mat faceROI = work_frame(faceRect).clone();


        //        cv::Mat rgb;
        //        cv::cvtColor(faceROI, rgb, cv::COLOR_BGR2RGB);
        //        QImage img((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
        //        emit sigFaceCrop(img);
        //        qDebug()<<"end";



        //static qint64 last = 0;
//        qint64 now = QDateTime::currentMSecsSinceEpoch();
//        if (now - last < 4000) {  // 2000 ms = 2秒
//            continue;
//        }
//        last = now;
//        static int counter = 0;
//        counter++;

        std::vector<uchar> buf;
        t1 = QDateTime::currentMSecsSinceEpoch();
        cv::imencode(".jpg", work_frame, buf);
        qDebug() << "imencode time:" << QDateTime::currentDateTime() - t1;


        /* jpg转为base64 */

        QByteArray jpgData(reinterpret_cast<const char*>(buf.data()), buf.size());
        t1 = QDateTime::currentMSecsSinceEpoch();
        QString jpgBase64 = jpgData.toBase64();
        qDebug() << "imencode time:" << QDateTime::currentDateTime() - t1;

        qDebug() << "toBase64 长度：" << jpgBase64.length();

        //faceSearch(jpgBase64, global_token);
        emit sigFaceReady(jpgBase64, work_frame);
        FaceAttendence::getInstance()->detectionSuccess = false;

        /* ================================嵌入式极致优化=========================*/

        //防止重复检测，另外加超时机制防止卡死
        quint64 start = QDateTime::currentMSecsSinceEpoch();
        while (!FaceAttendence::getInstance()->detectionSuccess && !isInterruptionRequested()) {
            if (QDateTime::currentMSecsSinceEpoch() - start > 15000) {
                qDebug("超时！可能网络错误或token过期");
                FaceAttendence::getInstance()->detectionSuccess = true;
                break;
            }
            usleep(100000); //降低cpu占用，检测线程睡眠500ms
        }
        qDebug() << "单次检测完成，开始新的一轮检测";
        // qDebug() << jpgBase64;


    }

}


Work::~Work()
{
    std::cout << "work over" << std::endl;
}



