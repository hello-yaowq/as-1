/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2015  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "arvic.h"

/* ============================ [ MACROS    ] ====================================================== */
#define iX           0
#define iY           1
#define iOffset      2
#define iLength      3
#define iHeadWidth   4
#define iTailWidth   5
#define iColor       6
#define iStart       7     /* Start Degree */
#define iRange       8     /* Span Range Degree */

#define cMechanicalZero 1000
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static qint32 pointer_configuration[2][9] = {
    {850,285,0, 175,16,4,0x7453A2,330,240}, /* Speed */
    {473,157,20,70 ,10,4,0xD63441,322,250}, /* Tacho */
};
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
arVICPointer::arVICPointer(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setZValue(1);
}

QRectF arVICPointer::boundingRect() const
{
    qint32* cfg =  pointer_configuration[0];
    return QRectF(-cfg[iLength]-cfg[iOffset],-cfg[iLength]-cfg[iOffset],(cfg[iLength]+cfg[iOffset])*2,(cfg[iLength]+cfg[iOffset])*2);
}

void arVICPointer::setPosDegree(qint32 Degree)
{
    qint32* cfg =  pointer_configuration[0];
//    if((Degree <= (cfg[iRange]*100+cMechanicalZero)) && (Degree >= 0)) {
//        self.Degree = Degree;
//    }

    setRotation((Degree-cMechanicalZero)/100+cfg[iStart]);
}

void arVICPointer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)widget;
    qint32* cfg = pointer_configuration[0];
    quint32 radius,radius2;
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor((cfg[iColor]>>16)&0xFF,(cfg[iColor]>>8)&0xFF,(cfg[iColor]>>0)&0xFF)));
    QPolygon polygon(4);
    polygon.setPoint(0,QPoint(-cfg[iOffset],                 cfg[iHeadWidth]/2));
    polygon.setPoint(1,QPoint(-cfg[iOffset]-cfg[iLength],    cfg[iTailWidth]/2));
    polygon.setPoint(2,QPoint(-cfg[iOffset]-cfg[iLength],   -cfg[iTailWidth]/2));
    polygon.setPoint(3,QPoint(-cfg[iOffset],                -cfg[iHeadWidth]/2));
    painter->drawConvexPolygon(polygon);
    if(cfg[iOffset] < 0) {
        radius = -cfg[iOffset];
    }
    else if(cfg[iOffset] > 0) {
        radius = cfg[iOffset];
    }
    else {
        radius = cfg[iHeadWidth];
    }
    radius += 2;
    painter->drawEllipse(-radius,-radius,radius*2,radius*2);
    painter->setBrush(QBrush(QColor(0,0,0))); /* black */
    radius2 = radius*2/3;
    if(radius2 > cfg[iHeadWidth]) {
        radius2 = radius-cfg[iHeadWidth]*2/3;
    }
    painter->drawEllipse(-radius2,-radius2,radius2*2,radius2*2);
    setPos(cfg[iX],cfg[iY]);

    setPosDegree(cMechanicalZero);
}

arVICPointer::~arVICPointer(){}

arVICGraphicView::arVICGraphicView(QWidget *parent):QGraphicsView(parent)
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    setScene(scene);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);

    speed = new arVICPointer();
    scene->addItem(speed);
    startTimer(10);
}

void arVICGraphicView::drawBackground(QPainter *painter, const QRectF &rect)
{
    QImage bg(VIC_PICS_PATH"/bg.png");
    resize(bg.size());
    scene()->setSceneRect(0, 0, bg.size().width(), bg.size().height());
    painter->drawImage(0,0,bg);

    (void)rect;
}

void arVICGraphicView::timerEvent(QTimerEvent* e)
{
    (void)e;
}

arVICGraphicView::~arVICGraphicView()
{

}

arVIC::arVIC(QString name,QWidget *parent) : arDevice(name,parent)
{
    gview = new arVICGraphicView(this);
    setCentralWidget(gview);
    this->setGeometry(50,150,1200,500);
    setVisible(true);
}
arVIC::~arVIC()
{

}
