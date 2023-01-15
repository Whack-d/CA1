#pragma once
#include "SceneNode.hpp"
#include "CommandQueue.hpp"

class Entity : public SceneNode
{
public:
	Entity(int hitpoints);
	void SetVelocity(sf::Vector2f velocity);
	void SetVelocity(float vx, float vy);
	sf::Vector2f GetVelocity() const;
	void Accelerate(sf::Vector2f velocity);
	void Accelerate(float vx, float vy);

	int GetScore() const;
	void GainPoints(unsigned int points);
	void LosePoints(unsigned int points);


protected:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);

private:
	sf::Vector2f m_velocity;
	unsigned int m_score;
};

