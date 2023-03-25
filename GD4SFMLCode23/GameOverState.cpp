#include "GameOverState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "Button.hpp"

GameOverState::GameOverState(StateStack& stack, Context context)
    :State(stack, context)
{
    sf::Texture& texture = context.textures->Get(Texture::kGameOver);

    m_background_sprite.setTexture(texture);

    auto menu_button = std::make_shared<GUI::Button>(context);
    menu_button->setPosition(400, 330);
    menu_button->SetText("Main Menu");
    menu_button->SetCallback([this]()
        {
            RequestStackClear();
            RequestStackPush(StateID::kMenu);
        });

    auto exit_button = std::make_shared<GUI::Button>(context);
    exit_button->setPosition(400, 420);
    exit_button->SetText("Exit");
    exit_button->SetCallback([this]()
        {
            RequestStackClear();
        });

    m_gui_container.Pack(menu_button);
    m_gui_container.Pack(exit_button);
}

void GameOverState::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());
    window.draw(m_background_sprite);
    window.draw(m_gui_container);
}

bool GameOverState::Update(sf::Time dt)
{
    return true;
}

bool GameOverState::HandleEvent(const sf::Event& event)
{
    m_gui_container.HandleEvent(event);
    return false;
}

