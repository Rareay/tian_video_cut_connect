#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Initialize();


}

MainWindow::~MainWindow()
{
    m_video->release();
    delete ui;
}

void MainWindow::Initialize()
{
    m_video = new cv::VideoCapture();
    m_label_frame = new QLabel();
    m_start_or_end = START;
    m_current_frame = 0.;
    m_front_frame = 0.;
    m_auto_display = true;
    m_play = true;
    m_start_frame = 0;
    m_end_frame = 0;
    m_save_index = 0;
    m_connect_index = 0;

    ui->tab_1->setWindowTitle("1");

    connect(ui->slidingBar, SIGNAL(start_changed(float)), ui->progressBar, SLOT(set_location(float)));
    connect(ui->slidingBar, SIGNAL(end_changed(float)), ui->progressBar, SLOT(set_location(float)));

    connect(ui->actionCut, SIGNAL(triggered(bool)), this, SLOT(on_actionCut_changed()));
    connect(ui->actionConnect, SIGNAL(triggered(bool)), this, SLOT(on_actionConnect_triggered()));

    //qDebug() << ui->frame->x();
    //qDebug() << ui->frame->y();
    m_label_frame->setParent(ui->stackedWidget);
    m_label_frame->setMinimumSize(QSize(1, 1));
    m_label_frame->setMaximumSize(QSize(1, 1));
    m_label_frame->setStyleSheet("background-color: rgb(0, 0, 0);");
    //m_label->setStyleSheet("background-color: qlineargradient(spread:pad, "
    //                     "x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 0, 0, 0), "
    //                     "stop:1 rgba(255, 255, 255, 0));");
    m_label_frame->move(11, 11);
    //ui->gridLayout_2->addWidget(m_label_frame, 0, 0, 1, 2);

    QFrame * line = new QFrame();
    line->setHidden(true);
    ui->gridLayout->addWidget(line, 0, 0, 15, 1);


    m_fist_videolist = new VideoList();
    m_fist_videolist->m_checkbox->setText(QString(" "));
    m_fist_videolist->m_textbrowser->setText(QString(" "));
    m_fist_videolist->setMaximumHeight(45);
    ui->gridLayout->addWidget(m_fist_videolist, 0, 0, 1, 1);

    QFrame * line2 = new QFrame();
    line2->setHidden(true);
    ui->gridLayout_3->addWidget(line2, 0, 0, 15, 1);

    m_fist_connect_videolist = new VideoList();
    m_fist_connect_videolist ->m_checkbox->setText(QString(" "));
    m_fist_connect_videolist ->m_textbrowser->setText(QString(" "));
    m_fist_connect_videolist ->setMaximumHeight(45);
    ui->gridLayout_3->addWidget(m_fist_connect_videolist, 0, 0, 1, 1);
}

void MainWindow::show_frame(cv::Mat &img)
{
    int w = img.cols;
    int h = img.rows;
    // 改变图片的尺寸
    int u_w = ui->frame->width();
    int u_h = ui->frame->height();
    int i_w = w;
    int i_h = h;
    if (i_w > u_w || i_h > u_h) {
        if (u_w * i_h - u_h * i_w > 0) {
            int i_h_temp = i_h;
            i_h = u_h;
            i_w = u_h * i_w / i_h_temp;
        } else {
            int i_w_temp = i_w;
            i_w = u_w;
            i_h = u_w * i_h / i_w_temp;
        }
    }
    w = i_w;
    h = i_h;

    m_label_frame->setMinimumSize(QSize(w, h));
    m_label_frame->setMaximumSize(QSize(w, h));
    int x = ui->frame->x();
    int y = ui->frame->y();
    x += (u_w - w)/2;
    y += (u_h - h)/2;
    m_label_frame->move(x, y);

    cv::resize(img, img, cv::Size(w, h));
    cv::cvtColor(img, img, CV_BGR2RGB);
    QImage image((const uchar*)img.data, img.cols,
                 img.rows, img.cols*img.channels(),
                 QImage::Format_RGB888);
    m_label_frame->setPixmap(QPixmap::fromImage(image));
}

int MainWindow::do_cut(cv::VideoCapture *video, std::string parent_path,
                       std::string filename, int frame_start, int frame_end)
{
    std::string suffix = ".mp4";
    std::string::size_type idx;
    idx = filename.find(suffix);
    if (idx == std::string::npos) { // 不存在
        filename += suffix;
    }
    filename = parent_path + "/" + filename;
    //std::cout << filename << std::endl;

    if (video->isOpened()) {
        int total_frame = video->get(CV_CAP_PROP_FRAME_COUNT); //总帧数
        int width = video->get(CV_CAP_PROP_FRAME_WIDTH);  //帧宽度
        int height = video->get(CV_CAP_PROP_FRAME_HEIGHT); //帧高度
        int frameRate = video->get(CV_CAP_PROP_FPS);  // 每秒的帧数

        if (frame_start >= frame_end) return -1;
        if (frame_end >= total_frame) return -1;

        cv::VideoWriter save_video(filename, CV_FOURCC('D', 'I', 'V', 'X'), frameRate,
                                    cv::Size(width, height), true);

        video->set(CV_CAP_PROP_POS_FRAMES, frame_start);

        cv::Mat frame;
        for (int index = frame_start; index < frame_end; index++) {
            *video >> frame;
            if (frame.empty()) break;
            save_video.write(frame);
        }
        save_video.release();
    }

    return 0;
}

void MainWindow::write_video_to_video(cv::VideoCapture *capture, cv::VideoWriter *writer,
                                      int w, int h, float rate)
{
    cv::Mat frame;
    while (1) {
        *capture >> frame;
        if (frame.empty()) break;
        cv::resize(frame, frame, cv::Size(w, h));
        writer->write(frame); // 写视频
    }

}

void MainWindow::write_picture_to_video(cv::Mat img, cv::VideoWriter *writer,
                                        int w, int h, int fps, int t)
{
    if (img.empty()) return;

    cv::Mat image;
    cv::resize(img, image, cv::Size(w, h));
    for (int i = 0; i < fps * t; i++) {
        cv::Mat frame = image.clone();
        writer->write(frame);
    }
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    cv::Mat img = m_frame.clone();
    if (!img.empty())
        show_frame(m_frame);
}

void MainWindow::do_display()
{
    if(m_video->isOpened()) {
        if (m_auto_display == false)
            m_video->set(CV_CAP_PROP_POS_FRAMES, m_current_frame);
        *m_video >> m_frame;
        if (m_frame.empty()) return;

        if (m_auto_display == true) {
            m_current_frame ++;
            //qDebug() << (float)m_current_frame/m_video_frame_count;
            ui->progressBar->set_location((float)m_current_frame/m_video_frame_count);
        }
        cv::Mat temp_frame = m_frame.clone();
        show_frame(temp_frame);
        int cut_time = (m_end_frame - m_start_frame) * 1000 / m_video_fps;
        int all_time = m_video_frame_count * 1000 / m_video_fps;
        ui->textBrowser->setText(QString("start: %1    end: %2    time: %3ms    current: %4    fps: %5    size: %6x%7    all_time: %8ms").
                     arg(QString::number(m_start_frame)).
                     arg(QString::number(m_end_frame)).
                     arg(QString::number(cut_time)).
                     arg(QString::number(m_current_frame)).
                     arg(QString::number(m_video_fps)).
                     arg(QString::number(m_video_frame_width)).
                     arg(QString::number(m_video_frome_height)).
                     arg(QString::number(all_time)));

    }
}


void MainWindow::on_slidingBar_start_changed(float p)
{
    m_start_or_end = START;
    //qDebug() << p;
    m_current_frame = m_video_frame_count * p;
    m_start_frame = m_current_frame;
    m_auto_display = false;
    m_timer->stop();
    do_display();
    m_play = false;
    ui->pushButton_play->setText("Play");
}

void MainWindow::on_slidingBar_end_changed(float p)
{
    m_start_or_end = END;
    //qDebug() << p;
    m_current_frame = m_video_frame_count * p;
    m_end_frame = m_current_frame;
    m_auto_display = false;
    m_timer->stop();
    do_display();
    m_play = false;
    ui->pushButton_play->setText("Play");

}

void MainWindow::on_progressBar_location_changed(float p)
{
    //qDebug() << "... " << p;
    m_current_frame = m_video_frame_count * p;
    m_auto_display = false;
    m_timer->stop();
    do_display();
    m_play = false;
    ui->pushButton_play->setText("Play");
}

void MainWindow::on_slidingMicroBar_location_changed(int state, int distence)
{
    //qDebug() << state << " " << distence;
    if (state == 1) {
        m_timer->stop();
        if (m_start_or_end == START) {
            m_current_frame = m_video_frame_count * ui->slidingBar->m_start;
            m_start_frame = m_current_frame;
        } else {
            m_current_frame = m_video_frame_count * ui->slidingBar->m_end;
            m_end_frame = m_current_frame;
        }
        m_front_frame = m_current_frame;
    }
    else {
        int f = m_front_frame + distence / 5;
        if (f >= 0 && f < m_video_frame_count) {
            m_current_frame = f;
            if (m_start_or_end == START) {
                ui->slidingBar->set_start((float)m_current_frame / (float)m_video_frame_count);
                m_start_frame = m_current_frame;
            } else {
                ui->slidingBar->set_end((float)m_current_frame / (float)m_video_frame_count);
                m_end_frame = m_current_frame;
            }
            m_auto_display = false;
            m_timer->stop();
            do_display();
            m_play = false;
            ui->pushButton_play->setText("Play");
        }
    }
}

void MainWindow::on_actionConnect_triggered()
{
    //qDebug() << "hello";
    ui->stackedWidget->setCurrentWidget(ui->page_2);
}

void MainWindow::on_actionCut_triggered()
{
    //qDebug() << "hello";
    ui->stackedWidget->setCurrentWidget(ui->page);
    m_label_frame->raise();
}


void MainWindow::on_pushButton_choose_clicked()
{
    QString DataPath;
    DataPath = QFileDialog::getOpenFileName(this, "选择视频", "/home/tianru");
    std::string path = DataPath.toStdString();
    if (path == "")
        return;
    m_video->open(path);
    if(m_video->isOpened()) {
        m_video_frame_width = m_video->get(CV_CAP_PROP_FRAME_WIDTH);  //帧宽度
        m_video_frome_height = m_video->get(CV_CAP_PROP_FRAME_HEIGHT); //帧高度
        m_video_fps =  m_video->get(CV_CAP_PROP_FPS);  //每秒的帧数
        m_video_frame_count = m_video->get(CV_CAP_PROP_FRAME_COUNT); //总帧数
        //std::cout << m_video_frame_width << ", " << m_video_frome_height
        //          << ", " << m_video_fps << ", " << m_video_frame_count << "\n ";

        m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(do_display()));
        m_timer->start(1000/m_video_fps);//开启定时器，周期500ms
        m_play = true;
        ui->pushButton_play->setText("Pause");
    }
}

void MainWindow::on_pushButton_play_clicked()
{
    if (m_play == false) {
        m_play = true;
        m_auto_display = true;
        ui->pushButton_play->setText("Pause");
        m_timer->start(1000/m_video_fps);//开启定时器，周期500ms
    } else {
        m_play = false;
        ui->pushButton_play->setText("Play");
        m_timer->stop();
    }

}

void MainWindow::on_pushButton_add_clicked()
{
    if (m_save_index >= 15) return;

    if (m_start_frame < 0 || m_end_frame < 0) return;
    if (m_end_frame <= m_start_frame) return;

    if (m_save_index == 0) {
        m_fist_videolist->m_checkbox->setText(QString::number(m_save_index));
        m_fist_videolist->m_textbrowser->setText(QString("%1 - %2").
                                           arg(QString::number(m_start_frame)).
                                           arg(QString::number(m_end_frame)));
    } else {
        VideoList * videolist = new VideoList();
        videolist->m_checkbox->setText(QString::number(m_save_index));
        videolist->m_textbrowser->setText(QString("%1 - %2").
                                           arg(QString::number(m_start_frame)).
                                           arg(QString::number(m_end_frame)));
        videolist->setMaximumHeight(45);
        ui->gridLayout->addWidget(videolist, m_save_index, 0, 1, 1);
    }
    m_save_index ++;

}

void MainWindow::on_pushButton_cut_clicked()
{
    // 禁止播放视频
    m_play = false;
    ui->pushButton_play->setText("Play");
    m_timer->stop();
    ui->pushButton_play->setDisabled(true);

    if (m_save_path == "") {
        QString Path;
        Path = QFileDialog::getExistingDirectory(this, "Path:", "/home/tianru");
        m_save_path = Path.toStdString();
        if (m_save_path == "") {
            m_video->set(CV_CAP_PROP_POS_FRAMES, m_current_frame);
            ui->pushButton_play->setCheckable(true);
            return;
        }
    }

    QLayoutItem * item;
    for (int cc = ui->gridLayout->count()-1; cc >= 0; --cc)
    {
        QLayoutItem *it = ui->gridLayout->itemAt(cc);
        VideoList *l = qobject_cast<VideoList *>(it->widget());
        if (l != 0)
        {
            std::string filname = l->m_lineedit->text().toStdString();
            if (l->m_checkbox->isChecked() && filname != "") {
                QString info = l->m_textbrowser->document()->toPlainText();
                QStringList list = info.split(" - ");
                QString start = list[0];
                QString end = list[1];
                int frame_start = atoi(start.toStdString().c_str());
                int frame_end = atoi(end.toStdString().c_str());
                //qDebug() << " " << frame_start << " " << frame_end;
                do_cut(m_video, m_save_path, filname, frame_start, frame_end);
            }
        }
    }

    // 允许播放
    m_video->set(CV_CAP_PROP_POS_FRAMES, m_current_frame);
    ui->pushButton_play->setDisabled(false);
}

void MainWindow::on_pushButton_path1_clicked()
{
    QString DataPath;
    DataPath = QFileDialog::getOpenFileName(this, "选择视频", "/home/tianru");
    std::string path = DataPath.toStdString();
    if (path == "")
        return;
    m_video_path = path;
    ui->textEdit_path1->setText(QString::fromLocal8Bit(path.c_str()));
    cv::VideoCapture * video = new cv::VideoCapture();
    video->open(path);
    if(video->isOpened()) {
        m_video_size_ori.w = video->get(CV_CAP_PROP_FRAME_WIDTH);  //帧宽度
        m_video_size_ori.h = video->get(CV_CAP_PROP_FRAME_HEIGHT); //帧高度
        m_video_fps_ori =  video->get(CV_CAP_PROP_FPS);  //每秒的帧数
        ui->textBrowser_size->setText(QString("%1x%2 (ori)").arg(QString::number(m_video_size_ori.w)).arg(QString::number(m_video_size_ori.h)));
        ui->textBrowser_fps->setText(QString("%1fps (ori)").arg(QString::number(m_video_fps_ori)));
    }
    video->release();
}

void MainWindow::on_pushButton_add_1_clicked()
{
    if (m_connect_index >= 15) return;

    if (m_video_path == "") return;

    if (m_connect_index == 0) {
        m_fist_connect_videolist->m_checkbox->setText(QString::number(m_connect_index));
        m_fist_connect_videolist->m_lineedit->setText(QString::fromLocal8Bit(m_video_path.c_str()));
        m_fist_connect_videolist->m_textbrowser->setText(QString("video , %1x%2 , %3fps").
                                                         arg(QString::number(m_video_size_ori.w)).
                                                         arg(QString::number(m_video_size_ori.h)).
                                                         arg(QString::number(m_video_fps_ori))
                                                         );
        //m_fist_connect_videolist->setDisabled(true);
        m_fist_connect_videolist->m_lineedit->setDisabled(true);
    } else {
        VideoList * videolist = new VideoList();
        videolist->m_checkbox->setText(QString::number(m_connect_index));
        videolist->m_lineedit->setText(QString::fromLocal8Bit(m_video_path.c_str()));
        videolist->m_textbrowser->setText(QString("video , %1x%2 , %3fps").
                                                   arg(QString::number(m_video_size_ori.w)).
                                                   arg(QString::number(m_video_size_ori.h)).
                                                   arg(QString::number(m_video_fps_ori))
                                                   );
        //videolist->setDisabled(true);
        videolist->m_lineedit->setDisabled(true);
        videolist->setMaximumHeight(45);
        ui->gridLayout_3->addWidget(videolist, m_connect_index, 0, 1, 1);
    }
    m_connect_index ++;

}


void MainWindow::on_pushButton_path_2_clicked()
{
    QString DataPath;
    DataPath = QFileDialog::getOpenFileName(this, "选择图片", "/home/tianru");
    std::string path = DataPath.toStdString();
    if (path == "")
        return;
    m_picture_path = path;
    ui->textEdit_path2->setText(QString::fromLocal8Bit(path.c_str()));

    cv::Mat img = cv::imread(path, 1);
    m_picture_size_ori.w = img.cols;
    m_picture_size_ori.h = img.rows;
}

void MainWindow::on_pushButton_add_2_clicked()
{
    if (m_connect_index >= 15) return;

    if (m_picture_path == "") return;


    QString time = ui->textEdit_time->document()->toPlainText();
    if (time == "") return;
    m_picture_time = atoi(time.toStdString().c_str());

    if (m_connect_index == 0) {
        m_fist_connect_videolist->m_checkbox->setText(QString::number(m_connect_index));
        m_fist_connect_videolist->m_lineedit->setText(QString::fromLocal8Bit(m_picture_path.c_str()));
        m_fist_connect_videolist->m_textbrowser->setText(QString("picture , %1x%2 , %3s").
                                                         arg(QString::number(m_picture_size_ori.w)).
                                                         arg(QString::number(m_picture_size_ori.h)).
                                                         arg(QString::number(m_picture_time))
                                                         );
        //m_fist_connect_videolist->setDisabled(true);
        m_fist_connect_videolist->m_lineedit->setDisabled(true);
    } else {
        VideoList * videolist = new VideoList();
        videolist->m_checkbox->setText(QString::number(m_connect_index));
        videolist->m_lineedit->setText(QString::fromLocal8Bit(m_picture_path.c_str()));
        videolist->m_textbrowser->setText(QString("picture , %1x%2 , %3s").
                                                         arg(QString::number(m_picture_size_ori.w)).
                                                         arg(QString::number(m_picture_size_ori.h)).
                                                         arg(QString::number(m_picture_time))
                                                         );
        //videolist->setDisabled(true);
        videolist->m_lineedit->setDisabled(true);
        videolist->setMaximumHeight(45);
        ui->gridLayout_3->addWidget(videolist, m_connect_index, 0, 1, 1);
    }
    m_connect_index ++;

}

void MainWindow::on_pushButton_crate_clicked()
{
    QLayoutItem * item;

    if (m_save_path == "") {
        QString Path;
        Path = QFileDialog::getExistingDirectory(this, "Path:", "/home/tianru");
        m_save_path = Path.toStdString();
        if (m_save_path == "") {
            m_video->set(CV_CAP_PROP_POS_FRAMES, m_current_frame);
            ui->pushButton_play->setCheckable(true);
            return;
        }
    }

    QString size = ui->textEdit_crate_size->document()->toPlainText();
    if (size == "") return;
    QStringList l = size.split("x");
    if (l.length() < 2) return;
    int width = atoi(l[0].toStdString().c_str());
    int height = atoi(l[1].toStdString().c_str());

    QString fps = ui->textEdit_crate_fps->document()->toPlainText();
    if (fps == "") return;
    int frameRate = atoi(fps.toStdString().c_str());

    std::string connect_video_path = m_save_path + "/new.mp4";
    m_write_connect_video = new cv::VideoWriter(connect_video_path,
                                                CV_FOURCC('D', 'I', 'V', 'X'),
                                                frameRate,
                                                cv::Size(width, height), true);

    for (int cc = ui->gridLayout_3->count()-1; cc >= 0; --cc)
    {
        QLayoutItem *it = ui->gridLayout_3->itemAt(cc);
        VideoList *l = qobject_cast<VideoList *>(it->widget());
        if (l != 0)
        {
            std::string filename = l->m_lineedit->text().toStdString();
            if (l->m_checkbox->isChecked() && filename != "") {
                // 打开视频文件

                QString info = l->m_textbrowser->document()->toPlainText();
                QStringList list = info.split(" , ");
                QString type = list[0];
                if (type == QString("video")) {
                    m_read_connect_video = new cv::VideoCapture(filename);
                    write_video_to_video(m_read_connect_video, m_write_connect_video,
                                         width, height, 0.);
                    m_read_connect_video->release();
                } else if (type == QString("picture")) {
                    QStringList time = list[2].split("s");
                    int t = atoi(time[0].toStdString().c_str());
                    cv::Mat frame = cv::imread(filename, 1);
                    write_picture_to_video(frame, m_write_connect_video, width, height, frameRate, t);
                }

            }
        }
    }
    m_write_connect_video->release();
}
