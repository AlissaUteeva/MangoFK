#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QObject>
#include <QVector>
#include <AL/al.h>
#include <AL/alc.h>

class AudioProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AudioProcessor(QObject *parent = nullptr);
    ~AudioProcessor();

    // Основные функции
    bool initializeOpenAL();
    void shutdownOpenAL();
    void setMasterVolume(float volume);
    void setPitch(float pitch);
    void setDopplerFactor(float factor);
    void setSpeedOfSound(float speed);

    // 3D позиционирование
    void setListenerPosition(float x, float y, float z);
    void setListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ);
    void setSourcePosition(float x, float y, float z);
    void setSourceVelocity(float x, float y, float z);

    // Визуализация
    QVector<float> getFrequencySpectrum();

signals:
    void volumeChanged(float volume);

private:
    ALCdevice *device;
    ALCcontext *context;
    ALuint source;
    ALuint buffer;
    bool isInitialized;
};

#endif
