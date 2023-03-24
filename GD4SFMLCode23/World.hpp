#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Aircraft.hpp"
#include "Layers.hpp"
//#include "NetworkNode.hpp"
#include "NetworkProtocol.hpp"
#include "PickupType.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


#include <array>
#include "CommandQueue.hpp"

#include "BloomEffect.hpp"
#include "SoundPlayer.hpp"

#include "Countdown.hpp"



class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderWindow& window, FontHolder& font, SoundPlayer& sounds, bool networked = false);
	void Update(sf::Time dt);
	void Draw();
	CommandQueue& GetCommandQueue();
	float GetWorldCountdown();

private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerPosition(Aircraft* player);
	void AdaptPlayerVelocity(Aircraft* player);
	void UpdateText();


private:
	sf::RenderWindow& m_window;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;

	CommandQueue m_command_queue;

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;
	Aircraft* m_player_aircraft;
	Aircraft* m_player_aircraft2;
	Countdown* m_countdown;
};

