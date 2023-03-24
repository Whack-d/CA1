#include "World.hpp"


World::World(sf::RenderWindow& window, FontHolder& font)
	:m_window(window)
	,m_camera(window.getDefaultView())
	,m_textures()
	,m_fonts(font)
	,m_scenegraph()
	,m_scene_layers()
	,m_world_bounds(0.f, 0.f, m_camera.getSize().x, 2000.f)
	,m_spawn_position(m_camera.getSize().x/2.f, m_world_bounds.height - m_camera.getSize().y/2.f)
	,m_player_aircraft(nullptr)
	,m_countdown(nullptr)

{

	LoadTextures();
	BuildScene();

	m_camera.setCenter(m_spawn_position);
}

void World::Update(sf::Time dt)
{
	//TODO fix velocity
	m_player_aircraft->SetVelocity(0.f, 0.f);


	//Forward the commands to the scenegraph, sort out velocity
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}
	AdaptPlayerVelocity(m_player_aircraft);

	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition(m_player_aircraft);
}

void World::Draw()
{
	m_window.setView(m_camera);
	m_window.draw(m_scenegraph);
}

CommandQueue& World::GetCommandQueue()
{
	return m_command_queue;
}

void World::LoadTextures()
{
	m_textures.Load(Texture::kEagle, "Media/Textures/Eagle.png");
	m_textures.Load(Texture::kRaptor, "Media/Textures/Raptor.png");
	m_textures.Load(Texture::kDesert, "Media/Textures/Map.png");
	m_textures.Load(Texture::kCharacter, "Media/Textures/Character.png");
	m_textures.Load(Texture::kCharacter2, "Media/Textures/Character2.png");
}

void World::BuildScene()
{
	//Initialize the different layers
	for(std::size_t i=0; i < static_cast<int>(Layers::kLayerCount); ++i)
	{ 
		SceneNode::Ptr layer(new SceneNode());
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}

	//Prepare the background
	sf::Texture& texture = m_textures.Get(Texture::kDesert);
	sf::IntRect textureRect(m_world_bounds);
	texture.setRepeated(true);

	//Add the background sprite to the world
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, textureRect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(background_sprite));

	//Add player's aircraft
	std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::kCharacter, m_textures, m_fonts));
	m_player_aircraft = leader.get();
	m_player_aircraft->setPosition(m_spawn_position);

	m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(leader));

	//Add their hitboxes
	m_player_aircraft->SetHitbox(sf::Vector2f(100,100), sf::Vector2f(20, 20));
	
	//Add countdown
	std::unique_ptr<Countdown> countdown(new Countdown(m_fonts));
	m_countdown = countdown.get();
	m_countdown->setPosition(sf::Vector2f(m_spawn_position.x, m_spawn_position.y - 300));

	m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(countdown));

	/*std::unique_ptr<Aircraft> left_escort(new Aircraft(AircraftType::kRaptor, m_textures, m_fonts));
	left_escort->setPosition(-80.f, 50.f);
	m_player_aircraft->AttachChild(std::move(left_escort));

	std::unique_ptr<Aircraft> right_escort(new Aircraft(AircraftType::kRaptor, m_textures, m_fonts));
	right_escort->setPosition(80.f, 50.f);
	m_player_aircraft->AttachChild(std::move(right_escort));*/


}

void World::AdaptPlayerPosition(Aircraft* player)
{
	//Keep the player on the sceen 
	sf::FloatRect view_bounds(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
	const float border_distance = 10.f;

	sf::Vector2f position = player->getPosition();
	position.x = std::max(position.x, view_bounds.left + border_distance);
	position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
	position.y = std::max(position.y, view_bounds.top + border_distance);
	position.y = std::min(position.y, view_bounds.top + view_bounds.height - border_distance);
	player->setPosition(position);

}

void World::AdaptPlayerVelocity(Aircraft* player)
{
	sf::Vector2f velocity = player->GetVelocity();

	//If they are moving diagonally divide by root 2
	if (velocity.x != 0.f && velocity.y != 0.f)
	{
		player->SetVelocity(velocity / std::sqrt(2.f));
	}
}

float World::GetWorldCountdown() {
	return m_countdown->GetCountdown();
}
