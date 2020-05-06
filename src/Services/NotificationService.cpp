#include "NotificationService.h"

Vector<const Notification*> NotificationService::notifications;
Mutex NotificationService::mutex;

const std::vector<const Notification*>& NotificationService::getNotifications(){
	return notifications;
}

void NotificationService::addNotification(const Notification* notification){
	for(const Notification* notif : notifications){
		/*if(false && notif->id == notification->id){
			delete notification;
			return;
		}*/
	}

	notifications.push_back(notification);
}

void NotificationService::removeNotification(const Notification* notification){
	int i = notifications.indexOf(notification);

	if(i != -1){
		notifications.remove(i);
	}

	delete notification;
}

Mutex& NotificationService::getMutex(){
	return mutex;
}
