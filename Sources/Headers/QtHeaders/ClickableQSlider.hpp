//
// Created by 11312 on 2023/12/2.
//

#ifndef ORCHIDPLAYER_CLICKABLEQSLIDER_HPP
#define ORCHIDPLAYER_CLICKABLEQSLIDER_HPP

#include <QSlider>
#include <QMouseEvent>
#include <QWidget>

class ClickableQSlider : public QSlider
{
Q_OBJECT

public:
    explicit ClickableQSlider(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *ev) override;
    bool m_Pressed = false;

signals:
    void FreeClickedEvent();

};


#endif //ORCHIDPLAYER_CLICKABLEQSLIDER_HPP
