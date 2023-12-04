//
// Created by 11312 on 2023/11/27.
//

#ifndef ORCHIDPLAYER_WIDGET_HPP
#define ORCHIDPLAYER_WIDGET_HPP

#include <QWidget>
#include <QTimer>
#include <QFileDialog>
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
    // QTimer* m_ProgressTimer = nullptr;
    QFileDialog* m_QFileDialog = nullptr;

    explicit Widget(QWidget *parent = nullptr);
    explicit Widget(AudioPlayer* audioPlayer, QWidget *parent = nullptr);
    ~Widget() override;

public slots:
    void Open();
    void Play();
    void Pause();
    void Stop();
    void UpdateSliderValue();
    void OnSlideValueChanged(int precentage);
    void OnSlidePressed();
    void OnSlideClicked();
    void OnSlideReleased();
    void ReopenDevice(const QString& qstr);
    void UpdateDeviceList();
    void OpenFileDialog();
    void OnFileSelected();
    void OnUpdateCurrentProgress(double progress);

signals:
    void UpdateCurrentProgressSignal(double progress);

private:
    Ui::Widget *ui;

    struct PlayStateRecord
    {
        bool isPlaying = false;
        double current = -1;
    };

    PlayStateRecord m_StateRecord;

    void SetUpUi();
    void ConnectSlots();
    int m_DeviceListLength = 0;
    std::string *m_DeviceList = nullptr;
};


#endif //ORCHIDPLAYER_WIDGET_HPP
