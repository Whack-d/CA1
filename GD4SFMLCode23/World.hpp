#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Aircraft.hpp"
#include "Layers.hpp"
#include "NetworkNode.hpp"
#include "NetworkProtocol.hpp"
#include "PickupType.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>

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

	sf::FloatRect GetViewBounds() const;
	CommandQueue& GetCommandQueue();
	float GetWorldCountdown();

	Aircraft* AddAircraft(int identifier);
	void RemoveAircraft(int identifier);
	void SetCurrentBattleFieldPosition(float line_y);
	void SetWorldHeight(float height);
	bool HasAlivePlayer() const;

	Aircraft* GetAircraft(int identifier) const;
	sf::FloatRect GetBattlefieldBounds() const;
	bool PollGameAction(GameActions::Action& out);

private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerPosition();
	void AdaptPlayerVelocity();
	void AdaptPlayerRotation();

	void HandleCollisions();
	void UpdateText();


private:
	struct SpawnPoint
	{
		SpawnPoint(AircraftType type, float x, float y) :m_type(type), m_x(x), m_y(y)
		{

		}
		AircraftType m_type;
		float m_x;
		float m_y;
	};

private:
	sf::RenderWindow& m_window;
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	sf::View m_camera;
	TextureHolder m_textures;
	SoundPlayer& m_sounds;
	FontHolder& m_fonts;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;

	CommandQueue m_command_queue;

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;

	std::vector<Aircraft*> m_player_aircraft;

	bool m_networked_world;
	NetworkNode* m_network_node;
	SpriteNode* m_finish_sprite;
	Countdown* m_countdown;
};

