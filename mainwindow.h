#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QTimer>
#include <QLabel>
#include <videolist.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/types_c.h>

#define START 0
#define END   1


struct FrameSize
{
    int h;
    int w;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    cv::VideoCapture * m_video;
    int m_video_frame_width;
    int m_video_frome_height;
    int m_video_fps;
    int m_video_frame_count;
    int m_current_frame;
    int m_front_frame;
    int m_start_frame;
    int m_end_frame;
    bool m_auto_display;
    bool m_play;
    int m_save_index;
    int m_start_or_end;
    std::string m_save_path;
    std::string m_video_path;
    FrameSize m_video_size_ori;
    int m_video_fps_ori;
    std::string m_picture_path;
    FrameSize m_picture_size_ori;
    int m_connect_index;
    int m_picture_time;

    cv::VideoCapture * m_read_connect_video;
    cv::VideoWriter * m_write_connect_video;

    QTimer * m_timer;
    cv::Mat m_frame;
    QLabel * m_label_frame;
    VideoList * m_fist_videolist;
    VideoList * m_fist_connect_videolist;

    void Initialize();
    void show_frame(cv::Mat &img);
    int do_cut(cv::VideoCapture *video, std::string parent_path,
                       std::string filename, int frame_start, int frame_end);
    void write_video_to_video(cv::VideoCapture * capture, cv::VideoWriter * writer, int w, int h, float rate);
    void write_picture_to_video(cv::Mat img, cv::VideoWriter * writer,
                                int w, int h, int fps, int t);

    virtual void resizeEvent(QResizeEvent *event) override;

public slots:
    void do_display();

private slots:
    void on_slidingBar_start_changed(float p);

    void on_slidingBar_end_changed(float p);

    void on_progressBar_location_changed(float p);

    void on_slidingMicroBar_location_changed(int state, int distence);

    void on_actionConnect_triggered();
    void on_actionCut_triggered();

    void on_pushButton_choose_clicked();

    void on_pushButton_play_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_cut_clicked();

    void on_pushButton_path1_clicked();

    void on_pushButton_add_2_clicked();

    void on_pushButton_add_1_clicked();

    void on_pushButton_path_2_clicked();

    void on_pushButton_crate_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
