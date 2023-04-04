#include "World.hpp"
#include "Utility.hpp"
#include "SoundNode.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <limits>

World::World(sf::RenderWindow& window, FontHolder& font, SoundPlayer& sounds, bool networked)
	:m_window(window)
	,m_target(window)
	,m_camera(window.getDefaultView())
	,m_textures()
	,m_fonts(font)
	,m_sounds(sounds)
	,m_scenegraph()
	,m_scene_layers()
	,m_world_bounds(0.f, 0.f, m_camera.getSize().x, 5000.f)
	,m_spawn_position(m_camera.getSize().x/2.f, m_world_bounds.height - m_camera.getSize().y/2.f)
	,m_player_aircraft()
	,m_networked_world(networked)
	,m_network_node(nullptr)
	,m_finish_sprite(nullptr)
	,m_countdown(nullptr)

{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);
	LoadTextures();
	BuildScene();

	m_camera.setCenter(m_spawn_position);
}

void World::Update(sf::Time dt)
{
	for (Aircraft* a : m_player_aircraft)
	{
		a->SetVelocity(0.f, 0.f);
	}

	//Forward the commands to the scenegraph, sort out velocity
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}
	AdaptPlayerVelocity();

	HandleCollisions();

	//RemoveWrecks() only destroys the entities, not the pointers in m_player_aircraft
	auto first_to_remove = std::remove_if(m_player_aircraft.begin(), m_player_aircraft.end(), std::mem_fn(&Aircraft::IsMarkedForRemoval));
	m_player_aircraft.erase(first_to_remove, m_player_aircraft.end());

	//Aplly Movement
	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition();
	AdaptPlayerRotation();
}

void World::Draw()
{
	m_target.setView(m_camera);
	m_target.draw(m_scenegraph);
}

sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
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
		ReceiverCategories category = (i == static_cast<int>(Layers::kAir)) ? ReceiverCategories::kScene : ReceiverCategories::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}

	//Prepare the background
	sf::Texture& texture = m_textures.Get(Texture::kDesert);
	//sf::IntRect textureRect(m_world_bounds);
	texture.setRepeated(true); 
	
	float view_height = m_camera.getSize().y;
	sf::IntRect texture_rect(m_world_bounds);
	texture_rect.height += static_cast<int>(view_height);

	//Add the background sprite to the world
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, texture_rect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top - view_height);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(background_sprite));

	//DEPRECATED
	/*
	//Add player's aircraft
	std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::kCharacter, m_textures, m_fonts));
	m_player_aircraft = leader.get();
	m_player_aircraft->setPosition(m_spawn_position);

	m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(leader));

	//Add their hitboxes
	m_player_aircraft->SetHitbox(sf::Vector2f(100,100), sf::Vector2f(20, 20));
	*/

	if (m_networked_world)
	{
		std::unique_ptr<NetworkNode> network_node(new NetworkNode());
		m_network_node = network_node.get();
		m_scenegraph.AttachChild(std::move(network_node));
	}
	
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

void World::AdaptPlayerPosition()
{
	//Keep the player on the sceen 
	sf::FloatRect view_bounds = GetViewBounds();
	const float border_distance = 10.f;

	for (Aircraft* aircraft : m_player_aircraft)
	{
		sf::Vector2f position = aircraft->getPosition();
		position.x = std::max(position.x, view_bounds.left + border_distance);
		position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
		position.y = std::max(position.y, view_bounds.top + border_distance);
		position.y = std::min(position.y, view_bounds.top + view_bounds.height - border_distance);
		aircraft->setPosition(position);
	}

}

void World::AdaptPlayerVelocity()
{
	for (Aircraft* aircraft : m_player_aircraft)
	{
		sf::Vector2f velocity = aircraft->GetVelocity();

		//If moving diagonally, reduce velocity (to have always same velocity)
		if (velocity.x != 0.f && velocity.y != 0.f)
		{
			aircraft->SetVelocity(velocity / std::sqrt(2.f));
		}
	}


}

void World::AdaptPlayerRotation()
{
	for (Aircraft* aircraft : m_player_aircraft)
	{
		if (aircraft == m_player_aircraft[0]) 
		{
			//TODO fix mouse if getPosition doesn't work
			sf::Vector2<int> mousePos = sf::Mouse::getPosition(m_window);
			//std::cout << aircraft->FindMouse(mousePos, m_window) << std::endl;
			//std::cout << aircraft->FindMouse(mousePos, m_window) << std::endl;
			aircraft->RotateSprite(aircraft->FindMouse(mousePos, m_window));
			//std::cout << aircraft->GetSprite().getRotation() << std::endl;
		}
	}
}

void World::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);
	for (SceneNode::Pair pair : collision_pairs)
	{
		/*if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kEnemyAircraft))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& enemy = static_cast<Aircraft&>(*pair.second);
			//Collision Response
			player.Damage(enemy.GetHitPoints());
			enemy.Destroy();
		}
		else if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kPickup))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			//Collision Response
			pickup.Apply(player);
			pickup.Destroy();
			player.PlayLocalSound(m_command_queue, SoundEffect::kCollectPickup);
		}
		else if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kEnemyProjectile) || MatchesCategories(pair, ReceiverCategories::kEnemyAircraft, ReceiverCategories::kAlliedProjectile))
		{
			auto& aircraft = static_cast<Aircraft&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			//Collision Response
			aircraft.Damage(projectile.GetDamage());
			projectile.Destroy();
		}*/
	}

		//TODO ADD COLLISION
}

bool MatchesCategories(SceneNode::Pair& colliders, ReceiverCategories type1, ReceiverCategories type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();
	if (static_cast<int>(type1) & category1 && static_cast<int>(type2) & category2)
	{
		return true;
	}
	else if (static_cast<int>(type1) & category2 && static_cast<int>(type2) & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

float World::GetWorldCountdown() {
	return m_countdown->GetCountdown();
}

Aircraft* World::AddAircraft(int identifier)
{
	std::unique_ptr<Aircraft> player(new Aircraft(AircraftType::kCharacter, m_textures, m_fonts));
	player->setPosition(m_camera.getCenter());
	player->SetIdentifier(identifier);

	m_player_aircraft.emplace_back(player.get());
	m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(player));
	return m_player_aircraft.back();
}

void World::RemoveAircraft(int identifier)
{
	Aircraft* aircraft = GetAircraft(identifier);
	if (aircraft)
	{
		aircraft->Destroy();
		std::cout << "Aircraft Destroyed" << std::endl;
		m_player_aircraft.erase(std::find(m_player_aircraft.begin(), m_player_aircraft.end(), aircraft));
	}
}

void World::SetCurrentBattleFieldPosition(float line_y)
{
	m_camera.setCenter(m_camera.getCenter().x, line_y - m_camera.getSize().y / 2);
	m_spawn_position.y = m_world_bounds.height;
}

void World::SetWorldHeight(float height)
{
	m_world_bounds.height = height;
}

bool World::HasAlivePlayer() const
{
	return !m_player_aircraft.empty();
}

Aircraft* World::GetAircraft(int identifier) const
{
	for (Aircraft* a : m_player_aircraft)
	{
		if (a->GetIdentifier() == identifier)
		{
			return a;
		}
	}
	return nullptr;
}

sf::FloatRect World::GetBattlefieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn offscreen
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}

bool World::PollGameAction(GameActions::Action& out)
{
	return m_network_node->PollGameAction(out);
}
