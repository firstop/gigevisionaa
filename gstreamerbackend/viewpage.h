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

#ifndef VIEWPAGE_H
#define VIEWPAGE_H

#include <QWidget>
#include <QScopedPointer>

class QComboBox;
class IView;
template <class T>
class QSharedPointer;

class IView
{
    QObject *m_parent;

public:
    explicit IView(QObject *parent) : m_parent(parent) {}
    virtual ~IView(){}
    QObject *qObject() const {return m_parent;}

    virtual void viewStateChanged(bool playing) = 0;
    virtual void toggleView() = 0;
    virtual void setRenderFramerate(double framerate) = 0;
    virtual void setRenderOutput(bool fullscreen, const QPoint &origin) = 0;
    virtual void enableIncrust(bool enable) = 0;
    virtual void changeIncrustPosition(int x, int y, int size, int opacity) = 0;
};

class ViewPagePrivate;
class ViewPage : public QWidget
{
    Q_OBJECT
    QComboBox *m_screenMagement;

public:
    ViewPage(QSharedPointer<IView> view, QWidget *parent = 0);
    virtual ~ViewPage();
    QAction *visibilityAction();


signals:
    void setFullscreen(bool fullscreen, const QPoint &origin);
    void changeIncrustPosition(int x, int y, int size, int opacity);

public slots:
    void clientActived(bool actived);
    void videoWindowChanged(bool fullscreen);

private slots:
    void onScreenIndexChanged(int index);
    void adjustIncrust();
    void onIncrustPosChanged(int);


protected:
    const QScopedPointer<ViewPagePrivate> d_ptr;

private:
    void createWidgets(QSharedPointer<IView> client);

    Q_DISABLE_COPY(ViewPage)
    Q_DECLARE_PRIVATE(ViewPage)

};

#endif // VIEWPAGE_H
