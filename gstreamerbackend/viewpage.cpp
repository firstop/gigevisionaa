/***************************************************************************
 *   Copyright (C) 2014-2015 by Cyril BALETAUD                                  *
 *   cyril.baletaud@gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "viewpage.h"
#include "viewpage_p.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QSlider>
#include <QDesktopWidget>
#include <QApplication>
#include <QToolButton>
#include <QAction>
#include <QCheckBox>
#include <QDialog>


ViewPagePrivate::ViewPagePrivate()
    : visibility(NULL)
{}


ViewPage::ViewPage(QSharedPointer<IView> view, QWidget *parent)
    : QWidget(parent),
      d_ptr(new ViewPagePrivate)
{
    setWindowTitle(trUtf8("Vue Vidéo"));
    createWidgets(view);
}

ViewPage::~ViewPage()
{}

QAction *ViewPage::visibilityAction()
{
    Q_D(ViewPage);
    if (d->visibility == NULL) {
        d->visibility = new QAction(QIcon("://ressources/icons/video-off.png"), trUtf8("Vidéo locale"), this);
        d->visibility->setCheckable(true);
        connect(d->visibility, SIGNAL(toggled(bool)), this, SLOT(setVisible(bool)));
    }
    return d->visibility;
}

void ViewPage::createWidgets(QSharedPointer<IView> client)
{
    Q_D(ViewPage);
    QPushButton *button = new QPushButton;
    button->setFixedSize(QSize(64,64));
    button->setIcon(QIcon("://ressources/icons/video-off.png"));
    button->setIconSize(button->size() * 0.6);
    connect(button, SIGNAL(clicked()), client->qObject(), SLOT(toggleView()));


    QComboBox *screens = new QComboBox;
    screens->addItem(trUtf8("Intégrée"));
    const QDesktopWidget * dw = QApplication::desktop();
    for (int i=0; i<dw->screenCount(); ++i) {
        QRect rect = dw->screenGeometry(i);
        screens->addItem(trUtf8("Écran %0 %1x%2").arg(i+1).arg(rect.width()).arg(rect.height()), rect);
    }

    QSpinBox *framerate = new QSpinBox;
    framerate->setRange(0, 100);
    framerate->setValue(30);
    framerate->setSuffix(trUtf8("   i/s"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(trUtf8("Sortie vidéo")));
    layout->addWidget(screens);
    layout->addWidget(new QLabel(trUtf8("Framerate (0: inactif)")));
    layout->addWidget(framerate);

    QGroupBox * group = new QGroupBox(trUtf8("Configuration"));
    group->setLayout(layout);
    QCheckBox *enableIncrust = new QCheckBox(trUtf8("Activer l'incrustation"));
    enableIncrust->setChecked(false);
    QPushButton *adjustIncrust = new QPushButton(trUtf8("Ajuster l'incrustation"));
    adjustIncrust->setEnabled(false);
    connect(enableIncrust, SIGNAL(toggled(bool)),adjustIncrust,SLOT(setEnabled(bool)));
    connect(enableIncrust, SIGNAL(toggled(bool)),client->qObject(),SLOT(enableIncrust(bool)));
    connect(adjustIncrust, SIGNAL(clicked()), this, SLOT(adjustIncrust()));

    QVBoxLayout *incrustLayout = new QVBoxLayout;
    incrustLayout->addWidget(enableIncrust);
    incrustLayout->addWidget(adjustIncrust);
    QGroupBox *incrust = new QGroupBox(trUtf8("Incrutation"));
    incrust->setLayout(incrustLayout);

    QVBoxLayout *main = new QVBoxLayout;
    main->addWidget(button, 0, Qt::AlignCenter);
    main->addWidget(new QLabel(trUtf8("Start/Stop")), 0, Qt::AlignCenter);
    main->addWidget(group);
    main->addWidget(incrust);
    main->addStretch();
    setLayout(main);

    connect(client->qObject(), SIGNAL(viewStateChanged(bool)), this, SLOT(clientActived(bool)));
    connect(this, SIGNAL(changeIncrustPosition(int,int,int,int)), client->qObject(), SLOT(changeIncrustPosition(int,int,int,int)));

}

void ViewPage::clientActived(bool actived)
{
    Q_D(ViewPage);
    d->visibility->setIcon(actived ? QIcon("://ressources/icons/video-on.png") : QIcon("://ressources/icons/video-off.png"));
}

void ViewPage::videoWindowChanged(bool fullscreen)
{
    if (!fullscreen) {
        m_screenMagement->blockSignals(true);
        m_screenMagement->setCurrentIndex(0);
        m_screenMagement->blockSignals(false);
    }
}

void ViewPage::onScreenIndexChanged(int index)
{
    QVariant variant = m_screenMagement->itemData(index);
    if (variant.isValid()) {
        QRect rect = variant.toRect();
        emit setFullscreen(true, QPoint(rect.left(), rect.top()));

    } else {
        emit setFullscreen(false, QPoint());
    }
}

void ViewPage::adjustIncrust()
{
    Q_D(ViewPage);
    d->xpos = new QSlider(Qt::Horizontal);
    d->xpos->setRange(0, 100);
    QSpinBox *xspin = new QSpinBox;
    xspin->setRange(0, 100);
    connect(d->xpos, SIGNAL(valueChanged(int)),xspin,SLOT(setValue(int)));
    connect(xspin, SIGNAL(valueChanged(int)),d->xpos,SLOT(setValue(int)));
    d->xpos->setValue(0);

    d->ypos = new QSlider(Qt::Horizontal);
    d->ypos->setRange(0, 100);
    QSpinBox *yspin = new QSpinBox;
    yspin->setRange(0, 100);
    connect(d->ypos, SIGNAL(valueChanged(int)),yspin,SLOT(setValue(int)));
    connect(yspin, SIGNAL(valueChanged(int)),d->ypos,SLOT(setValue(int)));
    d->ypos->setValue(0);

    d->fontSize = new QSlider(Qt::Horizontal);
    d->fontSize->setRange(3, 100);
    QSpinBox *fontspin = new QSpinBox;
    fontspin->setRange(3, 100);
    connect(d->fontSize, SIGNAL(valueChanged(int)),fontspin,SLOT(setValue(int)));
    connect(fontspin, SIGNAL(valueChanged(int)),d->fontSize,SLOT(setValue(int)));
    d->fontSize->setValue(3);

    d->opacity = new QSlider(Qt::Horizontal);
    d->opacity->setRange(0, 100);
    QSpinBox *opacityspin = new QSpinBox;
    opacityspin->setRange(0, 100);
    connect(d->opacity, SIGNAL(valueChanged(int)),opacityspin,SLOT(setValue(int)));
    connect(opacityspin, SIGNAL(valueChanged(int)),d->opacity,SLOT(setValue(int)));
    d->opacity->setValue(0);

    connect(d->xpos, SIGNAL(valueChanged(int)), this, SLOT(onIncrustPosChanged(int)));
    connect(d->ypos, SIGNAL(valueChanged(int)), this, SLOT(onIncrustPosChanged(int)));
    connect(d->fontSize, SIGNAL(valueChanged(int)), this, SLOT(onIncrustPosChanged(int)));
    connect(d->opacity, SIGNAL(valueChanged(int)), this, SLOT(onIncrustPosChanged(int)));


    QVBoxLayout *incrustLayout = new QVBoxLayout;
    incrustLayout->addWidget(new QLabel(trUtf8("Position horizontale")));
    incrustLayout->addWidget(xspin);
    incrustLayout->addWidget(d->xpos);
    incrustLayout->addWidget(new QLabel(trUtf8("Position verticale")));
    incrustLayout->addWidget(yspin);
    incrustLayout->addWidget(d->ypos);
    incrustLayout->addWidget(new QLabel(trUtf8("Taille de la police")));
    incrustLayout->addWidget(fontspin);
    incrustLayout->addWidget(d->fontSize);
    incrustLayout->addWidget(new QLabel(trUtf8("Opacité du fond")));
    incrustLayout->addWidget(opacityspin);
    incrustLayout->addWidget(d->opacity);


    QDialog popup(this);
    popup.setLayout(incrustLayout);
    popup.exec();

}

void ViewPage::onIncrustPosChanged(int)
{
    Q_D(ViewPage);
    emit changeIncrustPosition(d->xpos->value(), d->ypos->value(), d->fontSize->value(), d->opacity->value());
}



