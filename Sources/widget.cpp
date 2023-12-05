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

    m_AudioPlayer->m_Listeners.Run.push_back(
            [this](AudioPlayer& player){
                emit Widget::UpdateCurrentProgressSignal(player.Tell() / player.GetDuration());
            });
    SetUpUi();
    ConnectSlots();
}

Widget::Widget(QWidget *parent) :
        QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_AudioPlayer = new AudioPlayer();
    m_AudioPlayer->m_Listeners.Run.push_back(
            [this](AudioPlayer& player){
                emit Widget::UpdateCurrentProgressSignal(player.Tell() / player.GetDuration());
            });

    SetUpUi();
    ConnectSlots();
}

Widget::~Widget()
{
    m_AudioPlayer->Close();
    delete m_AudioPlayer;
//    delete m_ProgressTimer;
    delete ui;
}

void Widget::Play()
{
    if(!m_AudioPlayer->Loaded()) return;
    if(m_AudioPlayer->IsEnd())
    {
        m_AudioPlayer->Locate(0);
        m_AudioPlayer->Start();
//        if (m_ProgressTimer != nullptr)
//            m_ProgressTimer->start(100);
    }
    if(m_AudioPlayer->GetState() != RecurrentController::RUNNING)
    {
        double second;
        if(m_StateRecord.current > 0)
        {
            second = m_StateRecord.current;
            m_StateRecord.current = -1;
        }
        else
        {
            second = ((double) ui->horizontalSlider->value() / 100) * m_AudioPlayer->GetDuration();
        }

        m_AudioPlayer->Locate(second);
        m_AudioPlayer->Start();
//        if (m_ProgressTimer != nullptr)
//            m_ProgressTimer->start(100);
    }
}


void Widget::Pause()
{
    if(!m_AudioPlayer->Stop()) return;
//    if(m_ProgressTimer != nullptr)
//        m_ProgressTimer->stop();
    m_StateRecord.current = m_AudioPlayer->Tell();
}

void Widget::Stop()
{
    if(!m_AudioPlayer->Close()) return;
    ui->label->setText("Playing: ");
    ui->label_3->setText("Duration: ");
    ui->label_4->setText("Current: ");
//    m_ProgressTimer->stop();
//    delete m_ProgressTimer;
//    m_ProgressTimer = nullptr;
    m_StateRecord.current = -1;
    m_StateRecord.isPlaying = false;
}

void Widget::Open()
{
    try
    {
        if (!m_AudioPlayer->Open(ui->lineEdit->text().toStdString())) return;
    }catch (...)
    {
        Util::LOGEFMT("<Open> Cannot open url\n");
        return;
    }
    ui->label->setText(QString("Playing: ")+ui->lineEdit->text());

    AVSampleFormat sampleFormat = AudioPlayer::NarrowFormat(m_AudioPlayer->GetFormat());
    int sampleRate = m_AudioPlayer->GetSampleRate();

    int sampleFormatBoxIndex = ui->BitFormatBox->findData(sampleFormat);
    ui->BitFormatBox->setCurrentIndex(sampleFormatBoxIndex);
    int sampleRateBoxIndex = ui->SampleRateBox->findData(sampleRate);
    ui->SampleRateBox->setCurrentIndex(sampleRateBoxIndex);

    int64_t dur = lround(m_AudioPlayer->GetDuration()*1000);
    int64_t min = dur/60000, sec = (dur%60000)/1000, ms = (dur%60000)%1000;
    std::string durText = std::format("{}min {:2}sec {:3}ms", min, sec, ms);
    ui->label_3->setText(QString("Duration: ")+QString(durText.data()));
    int64_t cur = lround((ui->horizontalSlider->value()/100.0) * m_AudioPlayer->GetDuration()*1000);
    min = cur/60000, sec = (cur%60000)/1000, ms = (cur%60000)%1000;
    std::string curText = std::format("{}min {:2}sec {:3}ms: {:3}%", min, sec, ms, ui->horizontalSlider->value());
    ui->label_4->setText(QString("Current: ")+QString(curText.data()));
//    if(m_ProgressTimer != nullptr) return;
//    m_ProgressTimer = new QTimer(this);
//    connect(m_ProgressTimer, &QTimer::timeout, this,&Widget::UpdateSliderValue);
}

void Widget::UpdateSliderValue()
{
    if(m_AudioPlayer->Tell() == NAN || m_AudioPlayer->GetDuration() == NAN) return;
    int progress = std::lround(m_AudioPlayer->Tell()*100/m_AudioPlayer->GetDuration());
    ui->horizontalSlider->setValue(progress);
    
    int64_t cur = std::lround(m_AudioPlayer->Tell()*1000);
    int64_t min = cur/60000, sec = (cur%60000)/1000, ms = (cur%60000)%1000;
    std::string curText = std::format("{}min {:2}sec {:3}ms: {:3}%", min, sec, ms, progress);
    ui->label_4->setText(QString("Current: ")+QString(curText.data()));
}

void Widget::OnSlidePressed()
{
    if(m_AudioPlayer->GetState() == RecurrentController::RUNNING) m_StateRecord.isPlaying = true;
    else m_StateRecord.isPlaying = false;
//    if(m_ProgressTimer != nullptr)
//        m_ProgressTimer->stop();
    ui->horizontalSlider->m_Pressed = true;
}

void Widget::OnSlideClicked()
{
    if(!ui->horizontalSlider->m_Pressed)
    {
        double second = ((double) ui->horizontalSlider->value() / 100) * m_AudioPlayer->GetDuration();
        m_AudioPlayer->Locate(second);
        m_StateRecord.current = -1;
    }
}

void Widget::OnSlideReleased()
{
    if(m_StateRecord.isPlaying)
    {
        if(!m_AudioPlayer->Loaded()) return;
        if(m_AudioPlayer->IsEnd())
            ui->horizontalSlider->setValue(0);
        double second = ((double) ui->horizontalSlider->value() / 100) * m_AudioPlayer->GetDuration();
        m_AudioPlayer->Locate(second);
        m_AudioPlayer->Start();
//        if(m_ProgressTimer != nullptr)
//            m_ProgressTimer->start(100);
    }
    ui->horizontalSlider->m_Pressed = false;
    m_StateRecord.current = -1;
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

void Widget::OnSlideValueChanged(int precentage)
{
    if(!m_AudioPlayer->Loaded()) return;
    int64_t cur = std::lround(precentage * m_AudioPlayer->GetDuration() * 10);
    int64_t min = cur/60000, sec = (cur%60000)/1000, ms = (cur%60000)%1000;
    std::string durText = std::format("{}min {:2}sec {:3}ms: {:3}%", min, sec, ms, precentage);
    ui->label_4->setText(QString("Current: ")+QString(durText.data()));
}

void Widget::SetUpUi()
{
    QFont font;
    font.setFamilies({QString::fromUtf8("Consolas")});
    font.setPointSize(12);
    if(m_QFileDialog == nullptr)
    {
        m_QFileDialog = new QFileDialog(this);
        m_QFileDialog->setFont(font);
        m_QFileDialog->setGeometry(300, 300, 800, 600);
    }

    ui->SampleRateBox->addItem("11025Hz",  11025);
    ui->SampleRateBox->addItem("12000Hz",  12000);
    ui->SampleRateBox->addItem("22050Hz",  22050);
    ui->SampleRateBox->addItem("24000Hz",  24000);
    ui->SampleRateBox->addItem("44100Hz",  44100);
    ui->SampleRateBox->addItem("48000Hz",  48000);
    ui->SampleRateBox->addItem("88200Hz",  88200);
    ui->SampleRateBox->addItem("96000Hz",  96000);
    ui->SampleRateBox->addItem("176400Hz", 176400);
    ui->SampleRateBox->addItem("192000Hz", 192000);
    ui->SampleRateBox->addItem("352800Hz", 352800);
    ui->SampleRateBox->addItem("384000Hz", 384000);


    ui->BitFormatBox->addItem("8bit", AVSampleFormat::AV_SAMPLE_FMT_U8);
    ui->BitFormatBox->addItem("16bit", AVSampleFormat::AV_SAMPLE_FMT_S16);
    ui->BitFormatBox->addItem("32bit(float)", AVSampleFormat::AV_SAMPLE_FMT_FLT);
    ui->BitFormatBox->addItem("64bit(double)", AVSampleFormat::AV_SAMPLE_FMT_DBL);
}

void Widget::OpenFileDialog()
{
    if(m_QFileDialog != nullptr)
        m_QFileDialog->show();
    else SetUpUi();
}

void Widget::ConnectSlots()
{
    connect(ui->horizontalSlider, &ClickableQSlider::sliderPressed, this, &Widget::OnSlidePressed);
    connect(ui->horizontalSlider, &ClickableQSlider::sliderReleased, this, &Widget::OnSlideReleased);
    connect(ui->horizontalSlider, &ClickableQSlider::valueChanged, this, &Widget::OnSlideValueChanged);
    connect(ui->horizontalSlider, &ClickableQSlider::FreeClickedEvent, this, &Widget::OnSlideClicked);
    connect(ui->checkBox, &QCheckBox::clicked,
            [this](bool checked)
            {
                this->m_AudioPlayer->m_AutoLoop=checked;
            });
    UpdateDeviceList();
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &Widget::ReopenDevice);
    connect(ui->BrowseButton, &QPushButton::clicked, this, &Widget::OpenFileDialog);
    connect(m_QFileDialog, &QFileDialog::fileSelected, this, &Widget::OnFileSelected);
    connect(ui->VolumeSlider, &QSlider::valueChanged,
            [this](int value)
            {
                this->m_AudioPlayer->SetVolume(static_cast<float>(value)/100.0f);
            });
    connect(ui->SampleRateBox, &QComboBox::currentIndexChanged,
            [this](int index)
            {
                int sampleRate = this->ui->SampleRateBox->currentData().toInt();
                this->m_AudioPlayer->SetSampleRate(sampleRate);
            });
    connect(ui->BitFormatBox, &QComboBox::currentIndexChanged,
            [this](int index)
            {
                AVSampleFormat format = static_cast<AVSampleFormat>(this->ui->BitFormatBox->currentData().toInt());
                this->m_AudioPlayer->SetFormat(format);
            });
    connect(this, &Widget::UpdateCurrentProgressSignal, this, &Widget::OnUpdateCurrentProgress);
}

void Widget::OnFileSelected()
{
    if(m_QFileDialog)
    {
        QString fileUrl = m_QFileDialog->selectedFiles().first();
        ui->lineEdit->setText(fileUrl);
    }
}

void Widget::OnUpdateCurrentProgress(double progress)
{
    if(!m_AudioPlayer->Loaded() || !(progress == progress) || ui->horizontalSlider->m_Pressed) return;
    ui->horizontalSlider->setValue(lround(progress*100));
    int64_t cur = std::lround(progress*m_AudioPlayer->GetDuration()*1000);
    int64_t min = cur/60000, sec = (cur%60000)/1000, ms = (cur%60000)%1000;
    std::string durText = std::format("{}min {:2}sec {:3}ms: {:3}%", min, sec, ms, lround(progress*100));
    ui->label_4->setText(QString("Current: ")+QString(durText.data()));
}

