#ifndef CIRCUITWATCH_NOTIFICATIONSERVICE_H
#define CIRCUITWATCH_NOTIFICATIONSERVICE_H


#include <Util/Vector.h>
#include <Sync/Mutex.h>
#include "Notification.h"

class NotificationService {
public:
	static const std::vector<const Notification*>& getNotifications();
	static void addNotification(const Notification* notification);
	static void removeNotification(const Notification* notification);

	static Mutex& getMutex();


private:
	static Vector<const Notification*> notifications;
	static Mutex mutex;

};


#endif //CIRCUITWATCH_NOTIFICATIONSERVICE_H
