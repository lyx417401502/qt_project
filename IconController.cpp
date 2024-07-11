// IconController.cpp
#include "IconController.h"

IconController::IconController(QObject *parent) : QObject(parent) {

}

IconController::~IconController() {

}

void IconController::registerWidget(const QString &widgetId, QWidget *widget, const QMap<int, QIcon> &stateIcons) {
    widgets[widgetId] = widget;
    this->stateIcons[widgetId] = stateIcons;
    states[widgetId] = 0;
}

void IconController::triggerUpdate(const QString &widgetId, int newStateIndex) {
    if (widgets.contains(widgetId) && stateIcons.contains(widgetId) && states[widgetId] != newStateIndex) {
        states[widgetId] = newStateIndex;
        QIcon icon = stateIcons[widgetId][newStateIndex];
        emit updateIcon(widgetId, icon);
    }
}
