#include "mainwindow.h"
#include "audioprocessor.h"
#include <QPushButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QFileInfo>
#include <QWidget>
#include <QUrl>
#include <QSlider>
#include <QMessageBox>

//TrackWidget
TrackWidget::TrackWidget(const QString &filePath, QWidget *parent)
    : QWidget(parent)
    , filePath(filePath)
    , isSliderPressed(false)
{
    // Создаем плеер для этого трека
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(filePath));

    // Создаем виджеты
    QFileInfo info(filePath);
    label = new QLabel(info.fileName());
    playButton = new QPushButton("▶");
    deleteButton = new QPushButton("DEL");
    playButton->setFixedSize(50, 30);
    deleteButton->setFixedSize(40, 30);
    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 0);

    // Верхняя строка (название + кнопки)
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(label, 1);
    topLayout->addWidget(playButton);
    topLayout->addWidget(deleteButton);

    // Главный вертикальный лейаут
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addStretch(1);
    mainLayout->addWidget(slider);

    // Стили
    setStyleSheet("TrackWidget { background-color: rgba(255, 148, 210, 200); border-radius: 10px; margin: 5px; padding: 8px; }");
    label->setStyleSheet("font-size: 13px; font-weight: bold; color: #4a0025; background-color: rgba(255,255,255,120); padding: 5px; border-radius: 5px;");
    playButton->setStyleSheet("background-color: #ff69b4; color: white; border: none; border-radius: 5px; font-size: 14px; font-weight: bold;");
    deleteButton->setStyleSheet("background-color: #ff4444; color: white; border: none; border-radius: 5px; font-size: 14px; font-weight: bold;");
    slider->setStyleSheet("QSlider::groove:horizontal { height: 4px; background: #ffc0cb; border-radius: 2px; }"
                          "QSlider::handle:horizontal { background: #ff1493; width: 12px; height: 12px; margin: -4px 0; border-radius: 6px; }"
                          "QSlider::sub-page:horizontal { background: #ff69b4; border-radius: 2px; }");

    // Подключаем сигналы
    connect(playButton, &QPushButton::clicked, this, &TrackWidget::onPlayClicked);
    connect(deleteButton, &QPushButton::clicked, this, &TrackWidget::onDeleteClicked);
    connect(player, &QMediaPlayer::durationChanged, this, &TrackWidget::setSliderRange);
    connect(player, &QMediaPlayer::positionChanged, this, &TrackWidget::updateSliderPosition);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &TrackWidget::onMediaStatusChanged);
    connect(slider, &QSlider::sliderPressed, this, &TrackWidget::onSliderPressed);
    connect(slider, &QSlider::sliderReleased, this, &TrackWidget::onSliderReleased);
}

void TrackWidget::onPlayClicked()
{
    emit playRequested(this);
}

void TrackWidget::updatePlayButton(bool isPlaying)
{
    if (isPlaying) {
        playButton->setText("ӀӀ");
        playButton->setStyleSheet("background-color: #ff1493; color: white; border: none; border-radius: 5px; font-size: 14px; font-weight: bold;");
    } else {
        playButton->setText("▶");
        playButton->setStyleSheet("background-color: #ff69b4; color: white; border: none; border-radius: 5px; font-size: 14px; font-weight: bold;");
    }
}

void TrackWidget::setSliderRange(qint64 duration)
{
    slider->setRange(0, static_cast<int>(duration));
}

void TrackWidget::updateSliderPosition(qint64 position)
{
    if (!isSliderPressed) {
        slider->setValue(static_cast<int>(position));
    }
}

void TrackWidget::onSliderPressed()
{
    isSliderPressed = true;
}

void TrackWidget::onSliderReleased()
{
    isSliderPressed = false;
    player->setPosition(slider->value());
}

void TrackWidget::onDeleteClicked()
{
    emit trackDeleted(this);
}

void TrackWidget::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        emit trackFinished(this);
    }
}

void TrackWidget::stopPlayback()
{
    player->stop();
}

void TrackWidget::startPlayback()
{
    player->play();
}

void TrackWidget::pausePlayback()
{
    player->pause();
}

// MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentTrack(nullptr)
{
    setWindowTitle("MangoFK 🎀");
    setMinimumSize(700, 650);

    // Розовый фон
    this->setStyleSheet("QMainWindow { background-color: #ffb6c1; }");

    // Центральный виджет
    QWidget *central = new QWidget(this);
    central->setStyleSheet("background-color: transparent;");
    setCentralWidget(central);

    // Главный лейаут
    QVBoxLayout *mainLay = new QVBoxLayout(central);

    //ВЕРХНЯЯ ПАНЕЛЬ (Предыдущий - Добавить - Следующий)
    QHBoxLayout *topBar = new QHBoxLayout();

    prevButton = new QPushButton("<- Предыдущий");
    prevButton->setMinimumHeight(45);
    prevButton->setMinimumWidth(140);
    prevButton->setStyleSheet("font-size: 14px; font-weight: bold; background-color: #ff69b4; color: white; border: none; border-radius: 10px; padding: 8px;");
    prevButton->setCursor(Qt::PointingHandCursor);

    addButton = new QPushButton("+ Добавить MP3");
    addButton->setMinimumHeight(45);
    addButton->setMinimumWidth(160);
    addButton->setStyleSheet("font-size: 14px; font-weight: bold; background-color: #ff1493; color: white; border: none; border-radius: 10px; padding: 8px;");
    addButton->setCursor(Qt::PointingHandCursor);

    nextButton = new QPushButton("Следующий ->");
    nextButton->setMinimumHeight(45);
    nextButton->setMinimumWidth(140);
    nextButton->setStyleSheet("font-size: 14px; font-weight: bold; background-color: #ff69b4; color: white; border: none; border-radius: 10px; padding: 8px;");
    nextButton->setCursor(Qt::PointingHandCursor);

    topBar->addWidget(prevButton);
    topBar->addStretch(1);
    topBar->addWidget(addButton);
    topBar->addStretch(1);
    topBar->addWidget(nextButton);

    //ПАНЕЛЬ ГРОМКОСТИ
    QHBoxLayout *volumeBar = new QHBoxLayout();

    QLabel *volumeLabel = new QLabel("Громкость:");
    volumeLabel->setStyleSheet("color: #ff1493; font-weight: bold; font-size: 12px;");

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(70);
    volumeSlider->setFixedWidth(150);
    volumeSlider->setStyleSheet("QSlider::groove:horizontal { height: 4px; background: #ffc0cb; border-radius: 2px; }"
                                "QSlider::handle:horizontal { background: #ff1493; width: 12px; height: 12px; margin: -4px 0; border-radius: 6px; }");

    volumeBar->addStretch(1);
    volumeBar->addWidget(volumeLabel);
    volumeBar->addWidget(volumeSlider);
    volumeBar->addStretch(1);

    //ПАНЕЛЬ ЭФФЕКТОВ OPENAL
    QHBoxLayout *openalBar = new QHBoxLayout();
    openalBar->setSpacing(15);

    pitchDownButton = new QPushButton("Понизить тон");
    pitchDownButton->setMinimumHeight(35);
    pitchDownButton->setStyleSheet("background-color: #ff69b4; color: white; border: none; border-radius: 8px; padding: 5px; font-weight: bold;");
    pitchDownButton->setCursor(Qt::PointingHandCursor);
    pitchDownButton->setToolTip("Понижает высоту звука (голос становится ниже)");

    pitchUpButton = new QPushButton("Повысить тон");
    pitchUpButton->setMinimumHeight(35);
    pitchUpButton->setStyleSheet("background-color: #ff69b4; color: white; border: none; border-radius: 8px; padding: 5px; font-weight: bold;");
    pitchUpButton->setCursor(Qt::PointingHandCursor);
    pitchUpButton->setToolTip("Повышает высоту звука (голос становится выше)");

    openalBar->addStretch(1);
    openalBar->addWidget(pitchDownButton);
    openalBar->addWidget(pitchUpButton);
    openalBar->addStretch(1);

    //ОБЛАСТЬ ПРОКРУТКИ ДЛЯ ТРЕКОВ
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    container = new QWidget();
    container->setStyleSheet("background-color: transparent;");
    layout = new QVBoxLayout(container);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(10);

    scrollArea->setWidget(container);

    //ДОБАВЛЯЕМ ВСЕ В ГЛАВНЫЙ ЛЕЙАУТ
    mainLay->addLayout(topBar);
    mainLay->addLayout(volumeBar);
    mainLay->addLayout(openalBar);
    mainLay->addWidget(scrollArea, 1);

    //ИНИЦИАЛИЗАЦИЯ OPENAL
    audioProcessor = new AudioProcessor(this);

    if (!audioProcessor->initializeOpenAL()) {
        QMessageBox::warning(this, "Предупреждение",
                             "OpenAL не удалось инициализировать.\n"
                             "Эффекты будут недоступны, но плеер продолжит работу.");
    } else {
        qDebug() << "OpenAL успешно инициализирован!";
    }

    //ПОДКЛЮЧАЕМ СИГНАЛЫ

    // Громкость
    connect(volumeSlider, &QSlider::valueChanged, [this](int value) {
        float volume = value / 100.0f;
        audioProcessor->setMasterVolume(volume);

        if (currentTrack) {
            QMediaPlayer *player = currentTrack->getPlayer();
            if (player && player->audioOutput()) {
                player->audioOutput()->setVolume(volume);
            }
        }
    });

    // Понижение тона
    connect(pitchDownButton, &QPushButton::clicked, [this]() {
        static float pitch = 1.0f;
        pitch -= 0.1f;
        if (pitch < 0.5f) pitch = 0.5f;
        audioProcessor->setPitch(pitch);
        QMessageBox::information(this, "Эффект OpenAL",
                                 QString("Тон понижен до %1\nЗвук стал ниже").arg(pitch));
    });

    //Повышение тона
    connect(pitchUpButton, &QPushButton::clicked, [this]() {
        static float pitch = 1.0f;
        pitch += 0.1f;
        if (pitch > 2.0f) pitch = 2.0f;
        audioProcessor->setPitch(pitch);
        QMessageBox::information(this, "Эффект OpenAL",
                                 QString("Тон повышен до %1\nЗвук стал выше").arg(pitch));
    });

    // Основные кнопки
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addFile);
    connect(prevButton, &QPushButton::clicked, this, &MainWindow::playPrevTrack);
    connect(nextButton, &QPushButton::clicked, this, &MainWindow::playNextTrack);

    // Устанавливаем начальную громкость
    volumeSlider->setValue(70);
}

MainWindow::~MainWindow()
{
    if (audioProcessor) {
        audioProcessor->shutdownOpenAL();
        delete audioProcessor;
    }
}

void MainWindow::addFile()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Выберите MP3 файлы",
        QString(),
        "MP3 файлы (*.mp3)"
        );

    for (const QString &file : files) {
        TrackWidget *track = new TrackWidget(file);
        layout->addWidget(track);
        tracks.append(track);
        connect(track, &TrackWidget::trackDeleted, this, &MainWindow::onTrackDeleted);
        connect(track, &TrackWidget::playRequested, this, &MainWindow::onTrackPlayRequested);
        connect(track, &TrackWidget::trackFinished, this, &MainWindow::onTrackFinished);
    }
}

void MainWindow::playTrack(TrackWidget *track)
{
    if (!track) return;

    if (currentTrack && currentTrack != track) {
        currentTrack->stopPlayback();
        currentTrack->updatePlayButton(false);
    }

    currentTrack = track;
    currentTrack->startPlayback();
    currentTrack->updatePlayButton(true);

    float volume = volumeSlider->value() / 100.0f;
    audioProcessor->setMasterVolume(volume);

    QMediaPlayer *player = currentTrack->getPlayer();
    if (player && player->audioOutput()) {
        player->audioOutput()->setVolume(volume);
    }

    updateAllPlayButtons();
}

void MainWindow::stopCurrentTrack()
{
    if (currentTrack) {
        currentTrack->stopPlayback();
        currentTrack->updatePlayButton(false);
    }
}

void MainWindow::updateAllPlayButtons()
{
    for (TrackWidget *track : tracks) {
        bool isCurrent = (track == currentTrack);

        if (isCurrent && currentTrack) {
            QMediaPlayer *player = currentTrack->getPlayer();
            bool isPlaying = (player->playbackState() == QMediaPlayer::PlayingState);
            track->updatePlayButton(isPlaying);
        } else {
            track->updatePlayButton(false);
        }

        if (isCurrent) {
            track->setStyleSheet("TrackWidget { background-color: rgba(255, 105, 180, 220); border: 2px solid #ff1493; border-radius: 10px; margin: 5px; padding: 8px; }");
        } else {
            track->setStyleSheet("TrackWidget { background-color: rgba(255, 148, 210, 200); border-radius: 10px; margin: 5px; padding: 8px; }");
        }
    }
}

void MainWindow::onTrackPlayRequested(TrackWidget *track)
{
    if (currentTrack == track) {
        QMediaPlayer *player = currentTrack->getPlayer();
        if (player->playbackState() == QMediaPlayer::PlayingState) {
            currentTrack->pausePlayback();
            currentTrack->updatePlayButton(false);
            audioProcessor->setMasterVolume(0.0f);
        } else {
            currentTrack->startPlayback();
            currentTrack->updatePlayButton(true);
            float volume = volumeSlider->value() / 100.0f;
            audioProcessor->setMasterVolume(volume);
        }
    } else {
        playTrack(track);
    }
}

void MainWindow::onTrackFinished(TrackWidget *track)
{
    if (currentTrack == track) {
        playNextTrack();
    }
}

void MainWindow::playNextTrack()
{
    if (tracks.isEmpty()) return;

    int currentIndex = tracks.indexOf(currentTrack);
    int nextIndex = (currentIndex + 1) % tracks.size();
    playTrack(tracks[nextIndex]);
}

void MainWindow::playPrevTrack()
{
    if (tracks.isEmpty()) return;

    int currentIndex = tracks.indexOf(currentTrack);
    int prevIndex = (currentIndex - 1 + tracks.size()) % tracks.size();
    playTrack(tracks[prevIndex]);
}

void MainWindow::onTrackDeleted(TrackWidget *track)
{
    int index = tracks.indexOf(track);
    if (index == -1) return;

    if (currentTrack == track) {
        currentTrack->stopPlayback();
        currentTrack = nullptr;

        layout->removeWidget(track);
        tracks.removeAt(index);
        track->deleteLater();

        if (!tracks.isEmpty()) {
            playTrack(tracks.first());
        }
    } else {
        layout->removeWidget(track);
        tracks.removeAt(index);
        track->deleteLater();

        if (currentTrack && tracks.indexOf(currentTrack) == -1) {
            if (!tracks.isEmpty()) {
                playTrack(tracks.first());
            }
        }
    }
}
