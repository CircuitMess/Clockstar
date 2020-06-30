#ifndef CIRCUITWATCH_NOTIFICATION_H
#define CIRCUITWATCH_NOTIFICATION_H

#include <Arduino.h>
#include <string>

struct Notification {
	std::string title;
	std::string body;
};

#endif //CIRCUITWATCH_NOTIFICATION_H
