#pragma once
#include "Entity.hpp"
#include "TextNode.hpp"

class Countdown : public Entity 
{
public:
	int GetCountdown();
	void UpdateText();
	Countdown(const FontHolder& font);
private:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	int m_countdown;
	TextNode* m_countdown_display;
};
