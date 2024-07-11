// IconController.h
#ifndef ICONCONTROLLER_H
#define ICONCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QIcon>
#include <QLabel>
#include <QPushButton>

class QWidget;

enum class DeviceType {
    Light,
    Fan,
    Temperature,
    Humidity,
    Distance
};

class IconController : public QObject {
    Q_OBJECT

public:
    explicit IconController(QObject *parent = nullptr);
    ~IconController();

    void registerWidget(const QString &widgetId, QWidget *widget, const QMap<int, QIcon> &stateIcons);


    void triggerUpdate(const QString &widgetId, int newStateIndex);

    QMap<QString, QWidget*> widgets;
    QMap<QString, QMap<int, QIcon>> stateIcons;
    QMap<QString, int> states;

signals:
    void updateIcon(const QString &widgetId, QIcon icon);

private:

};
#endif // ICONCONTROLLER_H
