#include "Countdown.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


Countdown::Countdown(const FontHolder& font) : Entity(5)
{
	m_countdown = 100;
	std::string empty_string = "";
	std::unique_ptr<TextNode> score_display(new TextNode(font, empty_string));
	m_countdown_display = score_display.get();
	AttachChild(std::move(score_display));

	UpdateText();
}

int Countdown::GetCountdown()
{
	return m_countdown;
}

void Countdown::UpdateText() 
{
	m_countdown_display->SetString(std::to_string(GetCountdown()) + " Seconds Left!");

}

void Countdown::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	UpdateText();
	Entity::UpdateCurrent(dt, commands);
}
