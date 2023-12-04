//
// Created by 11312 on 2023/11/27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Widget.h" resolved

#include "widget.hpp"
#include "ui_Widget.h"

Widget::Widget(AudioPlayer* audioPlayer,QWidget *parent) :
        m_AudioPlayer(audioPlayer), QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->horizontalSlider, &ClickableQSlider::sliderPressed, this, &Widget::OnSlidePressed);
    connect(ui->horizontalSlider, &ClickableQSlider::sliderReleased, this, &Widget::OnSlideReleased);
    connect(ui->horizontalSlider, &ClickableQSlider::valueChanged, this, &Widget::OnSlideValueChanged);
    connect(ui->horizontalSlider, &ClickableQSlider::FreeClickedEvent, this, &Widget::OnSlideClicked);
    connect(ui->checkBox, &QCheckBox::clicked, [this](bool checked){this->m_AudioPlayer->m_AutoLoop=checked;});
    UpdateDeviceList();
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &Widget::ReopenDevice);
}

Widget::Widget(QWidget *parent) :
        QWidget(parent), ui(new Ui::Widget)
{
    m_AudioPlayer = new AudioPlayer();
    ui->setupUi(this);
    connect(ui->horizontalSlider, &ClickableQSlider::sliderPressed, this, &Widget::OnSlidePressed);
    connect(ui->horizontalSlider, &ClickableQSlider::sliderReleased, this, &Widget::OnSlideReleased);
    connect(ui->horizontalSlider, &ClickableQSlider::valueChanged, this, &Widget::OnSlideValueChanged);
    connect(ui->horizontalSlider, &ClickableQSlider::FreeClickedEvent, this, &Widget::OnSlideClicked);
    connect(ui->checkBox, &QCheckBox::clicked, [this](bool checked){this->m_AudioPlayer->m_AutoLoop=checked;});
    UpdateDeviceList();
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &Widget::ReopenDevice);
}

Widget::~Widget()
{
    m_AudioPlayer->Close();
    delete m_AudioPlayer;
    delete m_ProgressTimer;
    delete ui;
}

void Widget::Play()
{
    if(!m_AudioPlayer->Loaded()) return;
    if(m_AudioPlayer->IsEnd())
        ui->horizontalSlider->setValue(0);
    double second = ((double) ui->horizontalSlider->value() / 100) * m_AudioPlayer->GetDuration();
    m_AudioPlayer->Locate(second);
    m_AudioPlayer->Start();
    if(m_ProgressTimer != nullptr)
        m_ProgressTimer->start(100);
}


void Widget::Pause()
{
    if(!m_AudioPlayer->Stop()) return;
    if(m_ProgressTimer != nullptr)
        m_ProgressTimer->stop();
}

void Widget::Stop()
{
    if(!m_AudioPlayer->Close()) return;
    ui->label->setText("Playing: ");
    ui->label_3->setText("Duration: ");
    ui->label_4->setText("Current: ");
    m_ProgressTimer->stop();
    delete m_ProgressTimer;
    m_ProgressTimer = nullptr;
}

void Widget::Open()
{
    try
    {
        if (!m_AudioPlayer->Open(ui->lineEdit->text().toStdString(), 96000)) return;
    }catch (...)
    {
        Util::LOGEFMT("<Open> Cannot open url\n");
        return;
    }
    ui->label->setText(QString("Playing: ")+ui->lineEdit->text());

    int64_t dur = m_AudioPlayer->GetDuration()*1000;
    int64_t min = dur/60000, sec = (dur%60000)/1000, ms = (dur%60000)%1000;
    std::string durText = std::format("{}min {:2}sec {:3}ms", min, sec, ms);
    ui->label_3->setText(QString("Duration: ")+QString(durText.data()));

    if(m_ProgressTimer != nullptr) return;
    m_ProgressTimer = new QTimer(this);
    connect(m_ProgressTimer, &QTimer::timeout, this,&Widget::UpdateSliderValue);
}

void Widget::UpdateSliderValue()
{
    if(m_AudioPlayer->Tell() == NAN || m_AudioPlayer->GetDuration() == NAN) return;
    int progress = std::lround(m_AudioPlayer->Tell()*100/m_AudioPlayer->GetDuration());
    ui->horizontalSlider->setValue(progress);
    
    int64_t cur = std::lround(m_AudioPlayer->Tell()*1000);
    int64_t min = cur/60000, sec = (cur%60000)/1000, ms = (cur%60000)%1000;
    std::string durText = std::format("{}min {:2}sec {:3}ms: {:3}%", min, sec, ms, progress);
    ui->label_4->setText(QString("Current: ")+QString(durText.data()));
}

void Widget::OnSlidePressed()
{
    if(m_AudioPlayer->GetState() == RecurrentController::RUNNING) m_PlayingBeforeMove = true;
    else m_PlayingBeforeMove = false;
    if(m_ProgressTimer != nullptr)
        m_ProgressTimer->stop();
    ui->horizontalSlider->m_Pressed = true;
}

void Widget::OnSlideClicked()
{
    if(!ui->horizontalSlider->m_Pressed)
    {
        double second = ((double) ui->horizontalSlider->value() / 100) * m_AudioPlayer->GetDuration();
        m_AudioPlayer->Locate(second);
    }
}

void Widget::OnSlideReleased()
{
    if(m_PlayingBeforeMove)
    {
        if(!m_AudioPlayer->Loaded()) return;
        if(m_AudioPlayer->IsEnd())
            ui->horizontalSlider->setValue(0);
        double second = ((double) ui->horizontalSlider->value() / 100) * m_AudioPlayer->GetDuration();
        m_AudioPlayer->Locate(second);
        m_AudioPlayer->Start();
        if(m_ProgressTimer != nullptr)
            m_ProgressTimer->start(100);
    }
    ui->horizontalSlider->m_Pressed = false;
}

void Widget::ReopenDevice(const QString& qstr)
{
    auto& alctx = OpenALContext::GetInstance();
    alctx.ReopenDevice(qstr.toStdString());
}

void Widget::UpdateDeviceList()
{
    std::string *devList;
    int n = OpenALContext::ListDevice(&devList);

    bool isSame = true;
    if(n == m_DeviceListLength)
    {
        for(int i = 0; i < n; ++i)
        {
            if(devList[i] == m_DeviceList[i]) continue;
            isSame = false;
        }
    }
    else isSame = false;

    if(isSame)
    {
        delete[] devList;
        return;
    }
    ui->comboBox->clear();
    delete[] m_DeviceList;
    m_DeviceList = devList;
    m_DeviceListLength = n;

    for(int i = 0; i < n; ++i)
    {
        ui->comboBox->addItem(m_DeviceList[i].data());
    }

}

void Widget::OnSlideValueChanged(int progress)
{
    if(!m_AudioPlayer->Loaded()) return;
    int64_t cur = std::lround(progress*m_AudioPlayer->GetDuration()*10);
    int64_t min = cur/60000, sec = (cur%60000)/1000, ms = (cur%60000)%1000;
    std::string durText = std::format("{}min {:2}sec {:3}ms: {:3}%", min, sec, ms, progress);
    ui->label_4->setText(QString("Current: ")+QString(durText.data()));
}

