#include "Aircraft.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include "ResourceHolder.hpp"
#include "ReceiverCategories.hpp"
#include "Texture.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"

namespace
{
	const std::vector<AircraftData> Table = InitializeAircraftData();
}

Texture ToTextureID(AircraftType type)
{
	switch (type)
	{
	case AircraftType::kEagle:
		return Texture::kEagle;
		break;
	case AircraftType::kRaptor:
		return Texture::kRaptor;
		break;
	case AircraftType::kCharacter:
		return Texture::kCharacter;
		break;
	case AircraftType::kCharacter2:
		return Texture::kCharacter2;
		break;
	}
	return Texture::kEagle;
}

Aircraft::Aircraft(AircraftType type, const TextureHolder& textures, const FontHolder& fonts) 
	: Entity(Table[static_cast<int>(type)].m_score)
	, m_type(type) 
	, m_sprite(textures.Get(ToTextureID(type)))
	, m_fire_rate(1)
	, m_spread_level(1)
	, m_score_display(nullptr)
	, m_travelled_distance(0.f)
	, m_directions_index(0)
{
	sf::FloatRect bounds = m_sprite.getLocalBounds();
	m_sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
	std::string empty_string = "";

	std::unique_ptr<TextNode> score_display(new TextNode(fonts, empty_string));
	m_score_display = score_display.get();
	AttachChild(std::move(score_display));

	UpdateTexts();

}

unsigned int Aircraft::GetCategory() const
{
	switch (m_type)
	{
	case AircraftType::kCharacter:
		return static_cast<unsigned int>(ReceiverCategories::kPlayerAircraft);
	default:
		return static_cast<unsigned int>(ReceiverCategories::kEnemyAircraft);

	}
}

int Aircraft::GetIdentifier()
{
	return m_identifier;
}

void Aircraft::SetIdentifier(int identifier)
{
	m_identifier = identifier;
}

void Aircraft::IncreaseFireRate()
{
	if (m_fire_rate < 5)
	{
		++m_fire_rate;
	}
}

void Aircraft::IncreaseFireSpread()
{
	if (m_spread_level < 3)
	{
		++m_spread_level;
	}
}

void Aircraft::UpdateTexts()
{
	m_score_display->SetString(std::to_string(GetScore()) + " POINTS");
	m_score_display->setPosition(0.f, 50.f);
	m_score_display->setRotation(-getRotation());
}

void Aircraft::UpdateMovementPattern(sf::Time dt)
{
	//Enemy AI
	const std::vector<Direction>& directions = Table[static_cast<int>(m_type)].m_directions;
	if (!directions.empty())
	{
		//Move along the current direction, then change direction
		if (m_travelled_distance > directions[m_directions_index].m_distance)
		{
			m_directions_index = (m_directions_index + 1) % directions.size();
			m_travelled_distance = 0;
		}

		//Compute velocity
		double radians = Utility::ToRadians(directions[m_directions_index].m_angle + 90.f);
		float vx = GetMaxSpeed() * std::cos(radians);
		float vy = GetMaxSpeed() * std::sin(radians);

		SetVelocity(vx, vy);
		m_travelled_distance += GetMaxSpeed() * dt.asSeconds();



	}
}

float Aircraft::FindMouse(sf::Vector2<int> mousePos, sf::RenderWindow& window)
{
	sf::Vector2<int> newMousePos = window.mapCoordsToPixel(getPosition());

	const float PI = 3.14159265;
	float dx = newMousePos.x - mousePos.x;
	float dy = newMousePos.y - mousePos.y;
	float rotation = atan2f(dx, dy) * 180 / PI;
	return -rotation;
}

void Aircraft::RotateSprite(float rotation)
{
	m_sprite.setRotation(rotation);
}

float Aircraft::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_speed;
}

void Aircraft::Fire()
{
	//Fire Code
}

void Aircraft::SetHitbox(sf::Vector2f position, sf::Vector2f size)
{
	hitBox = { position, size };
	hurtBox = { sf::Vector2f(position.x + 50, position.y), size};
}

void Aircraft::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

void Aircraft::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	UpdateTexts();
	Entity::UpdateCurrent(dt, commands);

	//Update hitbox and hurtbox
	getTransform().transformRect(hitBox);
}

sf::Sprite Aircraft::GetSprite()
{
	return m_sprite;
};
