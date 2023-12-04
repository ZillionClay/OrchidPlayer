//
// Created by 11312 on 2023/11/27.
//

#ifndef ORCHIDPLAYER_WIDGET_HPP
#define ORCHIDPLAYER_WIDGET_HPP

#include <QWidget>
#include <QTimer>
#include "AudioPlayer.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
Q_OBJECT

public:
    AudioPlayer* m_AudioPlayer;
    QTimer* m_ProgressTimer = nullptr;

    explicit Widget(QWidget *parent = nullptr);
    explicit Widget(AudioPlayer* audioPlayer, QWidget *parent = nullptr);
    ~Widget() override;

public slots:
    void Open();
    void Play();
    void Pause();
    void Stop();
    void UpdateSliderValue();
    void OnSlideValueChanged(int progress);
    void OnSlidePressed();
    void OnSlideClicked();
    void OnSlideReleased();
    void ReopenDevice(const QString& qstr);
    void UpdateDeviceList();

signals:

private:
    Ui::Widget *ui;
    int m_DeviceListLength = 0;
    bool m_PlayingBeforeMove = false;
    std::string *m_DeviceList = nullptr;
};


#endif //ORCHIDPLAYER_WIDGET_HPP
