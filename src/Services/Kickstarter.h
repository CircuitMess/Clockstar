#ifndef CIRCUITWATCH_KICKSTARTER_H
#define CIRCUITWATCH_KICKSTARTER_H

#include <Arduino.h>

struct CampaignData {
	double moneys;
	uint backers;
	uint comments;
};

class Kickstarter {
public:
	Kickstarter();

	void update();
	const CampaignData& getData() const;

private:
	CampaignData data = { 0, 0, 0 };

};


#endif //CIRCUITWATCH_KICKSTARTER_H
