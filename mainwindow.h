#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>

QT_BEGIN_NAMESPACE
class QPushButton;
class QScrollArea;
class QVBoxLayout;
class QWidget;
class QLabel;
class QSlider;
QT_END_NAMESPACE

class AudioProcessor;

class TrackWidget : public QWidget
{
    Q_OBJECT
public:
    TrackWidget(const QString &filePath, QWidget *parent = nullptr);
    void stopPlayback();
    void startPlayback();
    void pausePlayback();
    void updatePlayButton(bool isPlaying);
    QString getFilePath() const { return filePath; }
    QMediaPlayer* getPlayer() const { return player; }

signals:
    void trackDeleted(TrackWidget*);
    void playRequested(TrackWidget*);
    void trackFinished(TrackWidget*);

private slots:
    void onPlayClicked();
    void setSliderRange(qint64 duration);
    void updateSliderPosition(qint64 position);
    void onSliderPressed();
    void onSliderReleased();
    void onDeleteClicked();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QPushButton *playButton;
    QPushButton *deleteButton;
    QLabel *label;
    QSlider *slider;
    QString filePath;
    bool isSliderPressed;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addFile();
    void playNextTrack();
    void playPrevTrack();
    void onTrackDeleted(TrackWidget *track);
    void onTrackPlayRequested(TrackWidget *track);
    void onTrackFinished(TrackWidget *track);

private:
    void playTrack(TrackWidget *track);
    void stopCurrentTrack();
    void updateAllPlayButtons();

    QPushButton *addButton;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QPushButton *pitchUpButton;
    QPushButton *pitchDownButton;
    QSlider *volumeSlider;
    QScrollArea *scrollArea;
    QWidget *container;
    QVBoxLayout *layout;
    QList<TrackWidget*> tracks;
    TrackWidget *currentTrack;
    AudioProcessor *audioProcessor;
};

#endif
