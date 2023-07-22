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

#include "recordpage.h"
#include "recordpage_p.h"

#include <QAction>
#include <QLabel>
#include <QGroupBox>

#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>


RecordPagePrivate::RecordPagePrivate()
    : visibility(NULL),
      button(NULL),
      configuration(NULL),
      directory(NULL),
      fileName(NULL)
{}

RecordPagePrivate::~RecordPagePrivate()
{}

RecordPage::RecordPage(QSharedPointer<IRecord> record, QWidget *parent)
    : QWidget(parent),
      d_ptr(new RecordPagePrivate)
{
    setWindowTitle(trUtf8("Enregistrement"));
    createWidgets(record);
}

RecordPage::~RecordPage()
{}

QAction *RecordPage::visibilityAction()
{
    Q_D(RecordPage);
    if (d->visibility == NULL) {
        d->visibility = new QAction(QIcon("://ressources/icons/record-off.png"), trUtf8("Enregistrement local"), this);
        d->visibility->setCheckable(true);
        connect(d->visibility, SIGNAL(toggled(bool)), this, SLOT(setVisible(bool)));
    }
    return d->visibility;
}

void RecordPage::clientActived(bool actived)
{
    Q_D(RecordPage);
    d->visibility->setIcon(actived
                           ? QIcon("://ressources/icons/record-on.png")
                           : QIcon("://ressources/icons/record-off.png"));
    d->button->setIcon(actived
                       ? QIcon("://ressources/icons/record-on.png")
                       : QIcon("://ressources/icons/record-off.png"));
    d->configuration->setDisabled(actived);
}

void RecordPage::onChangeDirectory()
{
    Q_D(RecordPage);
    QString directory = QFileDialog::getExistingDirectory(this, trUtf8("Sélectionner le dossier des enregistrements")
                                                          , QString("%0/%1").arg(d->filePath).arg(d->directory->text()));

    if (!directory.isEmpty() && QFileInfo(directory).isWritable()) {
        d->filePath = QString("%0/").arg(directory);
        d->directory->setText(directory.remove(QString("%0/").arg(QDir::homePath())));
    }
}

void RecordPage::onRecordClicked()
{
    Q_D(RecordPage);
    // détermination de l'index du nom de fichier
    int index = 0;
    QDirIterator it(d->filePath, QStringList() << QString("%0_????.mp4").arg(d->fileName->text()));
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        int i = fi.baseName().remove(0, 10).toInt();
        if (i>index) {
            index = i;
        }
    }

    emit startRecording(QString("%0%1_%2.mp4")
                        .arg(d->filePath)
                        .arg(d->fileName->text())
                        .arg(++index, 4, 10, QLatin1Char('0')));
}

void RecordPage::createWidgets(QSharedPointer<IRecord> client)
{
    Q_D(RecordPage);
    d->button = new QPushButton;
    d->button->setFixedSize(QSize(64,64));
    d->button->setIcon(QIcon("://ressources/icons/record-off.png"));
    d->button->setIconSize(d->button->size() * 0.6);
    connect(d->button, SIGNAL(clicked()), this, SLOT(onRecordClicked()));

    d->directory = new QLineEdit;
    d->directory->setReadOnly(true);
    d->filePath = QString("%0/").arg(QDir::homePath());
    QPushButton *changeDirectory = new QPushButton(trUtf8("Changer"));
    connect(changeDirectory, SIGNAL(clicked()),
            this, SLOT(onChangeDirectory()));

    d->fileName = new QLineEdit(trUtf8("jiguiviou"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(trUtf8("Dossier des enregistrements")));
    layout->addWidget(d->directory);
    layout->addWidget(new QLabel(trUtf8("Changer le dossier")));
    layout->addWidget(changeDirectory);
    layout->addWidget(new QLabel(trUtf8("Préfixe du fichier")));
    layout->addWidget(d->fileName);


    d->configuration = new QGroupBox(trUtf8("Configuration"));
    d->configuration->setLayout(layout);

    QVBoxLayout *main = new QVBoxLayout;
    main->addWidget(d->button, 0, Qt::AlignCenter);
    main->addWidget(new QLabel(trUtf8("Start/Stop")), 0, Qt::AlignCenter);
    main->addWidget(d->configuration);
    main->addStretch();
    setLayout(main);

    connect(client->qObject(), SIGNAL(isRecording(bool)), this, SLOT(clientActived(bool)));
    connect(this, SIGNAL(startRecording(QString)), client->qObject(), SLOT(toggleRecording(QString)));
}






