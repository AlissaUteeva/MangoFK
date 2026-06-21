#include "audioprocessor.h"
#include <QDebug>
#include <cmath>

AudioProcessor::AudioProcessor(QObject *parent)
    : QObject(parent)
    , device(nullptr)
    , context(nullptr)
    , source(0)
    , buffer(0)
    , isInitialized(false)
{
}

AudioProcessor::~AudioProcessor()
{
    shutdownOpenAL();
}

bool AudioProcessor::initializeOpenAL()
{
    // Открываем устройство
    device = alcOpenDevice(nullptr);
    if (!device) {
        qDebug() << "Ошибка: Не удалось открыть OpenAL устройство";
        return false;
    }

    // Создаем контекст
    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) {
        qDebug() << "Ошибка: Не удалось создать контекст OpenAL";
        return false;
    }

    // Генерируем буфер и источник
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);

    // Проверяем ошибки
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        qDebug() << "Ошибка OpenAL:" << error;
        return false;
    }

    // Настройки по умолчанию
    setMasterVolume(0.8f);
    setListenerPosition(0.0f, 0.0f, 0.0f);
    setSourcePosition(0.0f, 0.0f, -1.0f);
    setPitch(1.0f);

    isInitialized = true;
    qDebug() << "OpenAL инициализирован успешно!";
    return true;
}

void AudioProcessor::shutdownOpenAL()
{
    if (isInitialized) {
        if (buffer) alDeleteBuffers(1, &buffer);
        if (source) alDeleteSources(1, &source);

        if (context) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
        }
        if (device) alcCloseDevice(device);

        isInitialized = false;
    }
}

void AudioProcessor::setMasterVolume(float volume)
{
    if (!isInitialized) return;
    volume = qBound(0.0f, volume, 1.0f);
    alListenerf(AL_GAIN, volume);
    emit volumeChanged(volume);
}

void AudioProcessor::setPitch(float pitch)
{
    if (!isInitialized) return;
    pitch = qBound(0.5f, pitch, 2.0f);
    alSourcef(source, AL_PITCH, pitch);
}

void AudioProcessor::setDopplerFactor(float factor)
{
    if (!isInitialized) return;
    factor = qBound(0.0f, factor, 1.0f);
    alDopplerFactor(factor);
}

void AudioProcessor::setSpeedOfSound(float speed)
{
    if (!isInitialized) return;
    speed = qBound(100.0f, speed, 1000.0f);
    alSpeedOfSound(speed);
}

void AudioProcessor::setListenerPosition(float x, float y, float z)
{
    if (!isInitialized) return;
    alListener3f(AL_POSITION, x, y, z);
}

void AudioProcessor::setListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ)
{
    if (!isInitialized) return;
    float orientation[] = {atX, atY, atZ, upX, upY, upZ};
    alListenerfv(AL_ORIENTATION, orientation);
}

void AudioProcessor::setSourcePosition(float x, float y, float z)
{
    if (!isInitialized) return;
    alSource3f(source, AL_POSITION, x, y, z);
}

void AudioProcessor::setSourceVelocity(float x, float y, float z)
{
    if (!isInitialized) return;
    alSource3f(source, AL_VELOCITY, x, y, z);
}

QVector<float> AudioProcessor::getFrequencySpectrum()
{
    QVector<float> spectrum;
    static float phase = 0;
    phase += 0.1f;

    for (int i = 0; i < 10; i++) {
        float value = (sin(phase + i * 0.5f) + 1.0f) / 2.0f;
        spectrum.append(value);
    }
    return spectrum;
}
