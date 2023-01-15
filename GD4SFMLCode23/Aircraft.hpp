#pragma once
#include "Entity.hpp"
#include "AircraftType.hpp"
#include "ResourceIdentifiers.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "TextNode.hpp"

class Aircraft : public Entity
{
public:
	Aircraft(AircraftType type, const TextureHolder& textures, const FontHolder& fonts);
	unsigned int GetCategory() const override;

	void IncreaseFireRate();
	void IncreaseFireSpread();
	void UpdateTexts();
	void UpdateMovementPattern(sf::Time dt);
	float GetMaxSpeed() const;

private:
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	
private:
	AircraftType m_type;
	sf::Sprite m_sprite;

	unsigned int m_fire_rate;
	unsigned int m_spread_level;
	TextNode* m_score_display;
	float m_travelled_distance;
	int m_directions_index;
};

