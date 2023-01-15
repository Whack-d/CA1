#pragma once
#include "Entity.hpp"
#include "TextNode.hpp"

class Countdown : public Entity 
{
public:
	float GetCountdown();
	void UpdateText();
	Countdown(const FontHolder& font);
	void UpdateCountdown(sf::Time dt);
private:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	float m_countdown;
	TextNode* m_countdown_display;
};
