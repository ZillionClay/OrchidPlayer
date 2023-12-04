//
// Created by 11312 on 2023/12/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ClickableQSlider.h" resolved

#include "ClickableQSlider.hpp"


ClickableQSlider::ClickableQSlider(QWidget *parent) :
        QSlider(parent)
{
}

void ClickableQSlider::mousePressEvent(QMouseEvent *ev)
{
    QSlider::mousePressEvent(ev);
    //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
    double epos = ev->pos().x() / (double)width();
    setValue(lround(epos * (maximum() - minimum()) + minimum()));
    //发送自定义的鼠标单击信号
    emit FreeClickedEvent();
}
