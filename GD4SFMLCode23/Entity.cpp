#include "Entity.hpp"
#include <cassert>

Entity::Entity(int hitpoints)
    : m_score(hitpoints)
{
}

void Entity::SetVelocity(sf::Vector2f velocity)
{
    m_velocity = velocity;
}

void Entity::SetVelocity(float vx, float vy)
{
    m_velocity.x = vx;
    m_velocity.y = vy;
}

sf::Vector2f Entity::GetVelocity() const
{
    return m_velocity;
}

void Entity::Accelerate(sf::Vector2f velocity)
{
    m_velocity += velocity;
}

void Entity::Accelerate(float vx, float vy)
{
    m_velocity.x += vx;
    m_velocity.y += vy;
}

int Entity::GetScore() const
{
    return m_score;
}

void Entity::GainPoints(unsigned int points)
{
    assert(points > 0);
    //TODO Limit hitpoints
    m_score += points;
}

void Entity::LosePoints(unsigned int points)
{
    assert(points > 0);
    m_score -= points;
}

void Entity::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
    move(m_velocity * dt.asSeconds());
}
