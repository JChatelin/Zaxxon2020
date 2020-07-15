#include "pch.h"
#include "StringHelpers.h"
#include "Game.h"
#include "EntityManager.h"

const float Game::PlayerSpeed = 200.f;
const float Game::WeaponSpeed = 1.5f;
float Game::TimeSpawnWave = 3.f;
float Game::TimeEnemyMasterSpawn = 10.f;
const double Game::PI = 2 * acos(0.0);
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: mWindow(sf::VideoMode(840, 600), "Zaxxon 2020", sf::Style::Close)
	, mTexture()
	, mPlayer()
	, mFont()
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	mWindow.setFramerateLimit(160);
	buffer.loadFromFile("../Media/03-Main_Music.wav");
	mainAudio.setBuffer(buffer);
	_TextureWeapon.loadFromFile("../Media/Textures/SI_WeaponGreen.png");
	_TextureWeaponEnemy.loadFromFile("../Media/Textures/SI_WeaponYellow.png");
	_TextureWeaponEnemyMaster.loadFromFile("../Media/Textures/SI_WeaponRed.png");
	mTexture.loadFromFile("../Media/Textures/ennemy_3.png");
	_TextureEnemyMaster.loadFromFile("../Media/Textures/ennemy_motherSS.png");
	_TextureEnemy.loadFromFile("../Media/Textures/ennemy_1.png");
	mFont.loadFromFile("../Media/Sansation.ttf");

	InitSprites();
}

void Game::ResetSprites()
{
	_IsGameOver = false;
	_IsEnemyWeaponFired = false;
	_IsPlayerWeaponFired = false;
	_IsEnemyMasterWeaponFired = false;

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		entity->m_enabled = true;
	}
}

void Game::InitSprites() {
    srand(time(NULL));
    sf::Clock clock; // starts the clock
    sf::Time spawnTimerEnnemy = clock.getElapsedTime();
    sf::Time spawnTimerMasterEnemy = clock.getElapsedTime();

    _lives = 3;
    _score = 0;
    _IsGameOver = false;
    _IsEnemyWeaponFired = false;
    _IsPlayerWeaponFired = false;
    _IsEnemyMasterWeaponFired = false;

    //
    // Player
    //

    mPlayer.setTexture(mTexture);
    mPlayer.setPosition(50.f, 300.f);
    mPlayer.setRotation(70.f);
    std::shared_ptr<Entity> player = std::make_shared<Entity>();
    player->m_sprite = mPlayer;
    player->m_type = EntityType::player;
    player->m_size = mTexture.getSize();
    player->m_position = mPlayer.getPosition();
    EntityManager::m_Entities.push_back(player);


	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);

	//
	// Lives
	//

	_LivesText.setFillColor(sf::Color::Green);
	_LivesText.setFont(mFont);
	_LivesText.setPosition(10.f, 50.f);
	_LivesText.setCharacterSize(20);
	_LivesText.setString(std::to_string(_lives));

	//
	// Text
	//

	_ScoreText.setFillColor(sf::Color::Green);
	_ScoreText.setFont(mFont);
	_ScoreText.setPosition(10.f, 100.f);
	_ScoreText.setCharacterSize(20);
	_ScoreText.setString(std::to_string(_score));
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	mainAudio.setLoop(true);
	mainAudio.play();
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
            spawnEnemies(elapsedTime);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update(sf::Time elapsedTime)
{
	sf::Vector2f movement(0.f, 0.f);
	if (mIsMovingUp)
		movement.y -= PlayerSpeed;
	if (mIsMovingDown)
		movement.y += PlayerSpeed;
	if (mIsMovingLeft)
		movement.x -= PlayerSpeed;
	if (mIsMovingRight)
		movement.x += PlayerSpeed;

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::player)
		{
			continue;
		}

		entity->m_sprite.move(movement * elapsedTime.asSeconds());
	}
}

void Game::render()
{
	mWindow.clear();

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		mWindow.draw(entity->m_sprite);
	}

	mWindow.draw(mStatisticsText);
	mWindow.draw(mText);
	mWindow.draw(_LivesText);
	mWindow.draw(_ScoreText);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}

	//
	// Handle collisions
	//


	if (mStatisticsUpdateTime >= sf::seconds(0.050f))
	{
		if (_IsGameOver == true)
			return;

		HandleTexts();
		HandleGameOver();
		HandleCollisionWeaponEnemy();
		HandleCollisionWeaponPlayer();
		HandleCollisionEnemyMasterWeaponPlayer();
		HandleCollisionWeaponEnemyMaster();
		HanldeWeaponMoves();
		HanldeEnemyWeaponMoves();
		HanldeEnemyMasterWeaponMoves();
		HandleEnemyMoves(elapsedTime);
		HandleEnemyMasterMove();
		HandleEnemyWeaponFiring();
		HandleEnemyMasterWeaponFiring();
	}
}

void Game::HandleTexts()
{
	std::string lives = "Lives: " + std::to_string(_lives);
	_LivesText.setString(lives);
	std::string score = "Score: " + std::to_string(_score);
	_ScoreText.setString(score);
	return;
}

void Game::HandleCollisionEnemyMasterWeaponPlayer()
{
	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyMasterWeapon)
		{
			continue;
		}

		sf::FloatRect boundWeapon;
		boundWeapon = weapon->m_sprite.getGlobalBounds();

		sf::FloatRect boundPlayer;
		boundPlayer = EntityManager::GetPlayer()->m_sprite.getGlobalBounds();

		if (boundWeapon.intersects(boundPlayer) == true)
		{
			weapon->m_enabled = false;
			_IsEnemyMasterWeaponFired = false; 
			_lives--;
			//break;
			goto end;
		}
	}

end:
	//nop
	return;
}

void Game::HanldeEnemyMasterWeaponMoves()
{
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyMasterWeapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		x -= WeaponSpeed * 2.f;
        y += WeaponSpeed / 2.f;

		if (x <= 0)
		{
			entity->m_enabled = false;
			_IsEnemyMasterWeaponFired = false;
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::HandleEnemyMasterWeaponFiring()
{
	if (_IsEnemyMasterWeaponFired == true)
		return;

	if (EntityManager::GetEnemyMaster()->m_enabled == false)
		return;

	// a little random...
	int r = rand() % 50;
	if (r != 10)
		return;

	float x, y;
	x = EntityManager::GetEnemyMaster()->m_sprite.getPosition().x;
	y = EntityManager::GetEnemyMaster()->m_sprite.getPosition().y;
	y -= 0.5f;

	std::shared_ptr<Entity> sw = std::make_shared<Entity>();
	sw->m_sprite.setTexture(_TextureWeaponEnemyMaster);
    sw->m_sprite.setRotation(70.f);
    sw->m_sprite.setScale(2.f, 2.f);
	sw->m_sprite.setPosition(
		x + _TextureEnemyMaster.getSize().x / 2.f,
		y + _TextureEnemyMaster.getSize().y / 2.f);
	sw->m_type = EntityType::enemyMasterWeapon;
	sw->m_size = _TextureWeaponEnemyMaster.getSize();
	EntityManager::m_Entities.push_back(sw);

	_IsEnemyMasterWeaponFired = true;
}

void Game::HandleEnemyMasterMove()
{
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyMaster)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;

        if (entity->m_bBackToFront == true)
            y += sin(numberOfTicks * 0.1f * PI);
        else
            y -= sin(numberOfTicks * 0.1f * PI);

        x -= 1.f;
		entity->m_times++;

		if (y >= 500 || y <= 100)
		{
			if (entity->m_bBackToFront == true)
			{
				entity->m_bBackToFront = false;
				entity->m_times = 0;
			}
			else
			{
				entity->m_bBackToFront = true;
				entity->m_times = 0;
			}
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::HandleCollisionWeaponPlayer()
{
	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		sf::FloatRect boundWeapon;
		boundWeapon = weapon->m_sprite.getGlobalBounds();

		sf::FloatRect boundPlayer;
		boundPlayer = EntityManager::GetPlayer()->m_sprite.getGlobalBounds();

		if (boundWeapon.intersects(boundPlayer) == true)
		{
			weapon->m_enabled = false;
			_IsEnemyWeaponFired = false;
			_lives--;
			goto end;
		}
	}

end:
	//nop
	return;
}

void Game::HanldeEnemyWeaponMoves()
{
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		x -= WeaponSpeed * 2.f;
		y += WeaponSpeed / 2.f;

		if (x <= 0)
		{
			entity->m_enabled = false;
			_IsEnemyWeaponFired = false;
		}
		else
		{
			entity->m_sprite.setPosition(x, y);
		}
	}
}

void Game::HandleEnemyWeaponFiring()
{
	if (_IsEnemyWeaponFired == true)
		return;

	std::vector<std::shared_ptr<Entity>>::reverse_iterator rit = EntityManager::m_Entities.rbegin();
	for (; rit != EntityManager::m_Entities.rend(); rit++)
	{
		std::shared_ptr<Entity> entity = *rit;

		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemy)
		{
			continue;
		}

		// a little random...
		int r = rand() % 20;
		if (r != 10)
			continue;

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y -= 0.5f;

		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sw->m_sprite.setTexture(_TextureWeaponEnemy);
		sw->m_sprite.setRotation(70.f);
        sw->m_sprite.setScale(2.f, 2.f);
		sw->m_sprite.setPosition(
			x + _TextureWeaponEnemy.getSize().x / 2.f,
			y + _TextureWeaponEnemy.getSize().y / 2.f);

		sw->m_type = EntityType::enemyWeapon;
		sw->m_size = _TextureWeaponEnemy.getSize();
		EntityManager::m_Entities.push_back(sw);

		_IsEnemyWeaponFired = true;
		break;
	}
}

void Game::spawnEnemies(sf::Time elapsedTime) {
    TimeEnemyMasterSpawn += elapsedTime.asSeconds();
    TimeSpawnWave += elapsedTime.asSeconds();
    if (EntityManager::GetEnemyMaster() == nullptr) {
        HandleEnemyMasterSpawn();
    } else {
        if(TimeEnemyMasterSpawn >= 3.f) {
            HandleEnemyMasterSpawn();
            TimeEnemyMasterSpawn = 0.f;
        }
    }
    if (TimeSpawnWave >= 2.f) {
        HandleEnemySpawn();
        TimeSpawnWave = 0.f;
    }
}

void Game::HandleEnemyMasterSpawn() {
    srand(time(NULL));

    float yMasterPosition = (float(rand()) / float((RAND_MAX)) * 500);
    _EnemyMaster.setTexture(_TextureEnemyMaster);
    _EnemyMaster.setPosition(860.f, yMasterPosition);
    _EnemyMaster.setRotation(70.f);

    std::shared_ptr<Entity> sem = std::make_shared<Entity>();
    sem->m_sprite = _EnemyMaster;
    sem->m_type = EntityType::enemyMaster;
    sem->m_size = _TextureEnemyMaster.getSize();
    sem->m_position = _EnemyMaster.getPosition();
    EntityManager::m_Entities.push_back(sem);
}

void Game::HandleEnemySpawn() {
    srand(time(NULL));

    for (int i = 0; i < SPRITE_COUNT; i++) {
        float yPosition = (float(rand()) / float((RAND_MAX)) * 500);

        _Enemy[i].setTexture(_TextureEnemy);
        _Enemy[i].setRotation(245.f);
        _Enemy[i].setPosition(860.f + 50.f * (i + 1), yPosition);

        std::shared_ptr<Entity> se = std::make_shared<Entity>();
        se->m_sprite = _Enemy[i];
        se->m_type = EntityType::enemy;
        se->m_size = _TextureEnemy.getSize();
        se->m_position = _Enemy[i].getPosition();
        EntityManager::m_Entities.push_back(se);
    }
}

void Game::HandleEnemyMoves(sf::Time elapsedTime)
{
	//é
	// Handle Enemy moves
	//
	numberOfTicks += elapsedTime.asSeconds();
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemy)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;

		if (entity->m_bBackToFront == true)
		    y += sin(numberOfTicks * 0.3f * PI);
		else
			y -= sin(numberOfTicks * 0.3f * PI);

		x -= 1.f;
		entity->m_times++;

		if (y >= 500 || y <= 50) //0)
		{
			if (entity->m_bBackToFront == true)
			{
				entity->m_bBackToFront = false;
				entity->m_times = 0;
			}
			else
			{
				entity->m_bBackToFront = true;
				entity->m_times = 0;
			}
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::HanldeWeaponMoves()
{
	//
	// Handle Weapon moves
	//
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::weapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
        x += WeaponSpeed * 2.f;
        y -= WeaponSpeed / 2.f;

		if (x >= 840)
		{
			entity->m_enabled = false;
			_IsPlayerWeaponFired = false;
		}

		entity->m_sprite.setPosition(x, y);
	}
}

void Game::HandleCollisionWeaponEnemy()
{
	// Handle collision weapon enemies

	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::weapon)
		{
			continue;
		}

		for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
		{
			if (enemy->m_type != EntityType::enemy)
			{
				continue;
			}

			if (enemy->m_enabled == false)
			{
				continue;
			}

			sf::FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			sf::FloatRect boundEnemy;
			boundEnemy = enemy->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundEnemy) == true)
			{
				enemy->m_enabled = false;
				weapon->m_enabled = false;
				_IsPlayerWeaponFired = false;
				_score += 10;
				//break;
				goto end;
			}
		}
	}

end:
	//nop
	return;
}

void Game::HandleCollisionWeaponEnemyMaster()
{
	// Handle collision weapon master enemy

	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::weapon)
		{
			continue;
		}

		for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
		{
			if (enemy->m_type != EntityType::enemyMaster)
			{
				continue;
			}

			if (enemy->m_enabled == false)
			{
				continue;
			}

			sf::FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			sf::FloatRect boundEnemy;
			boundEnemy = enemy->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundEnemy) == true)
			{
				enemy->m_enabled = false;
				weapon->m_enabled = false;
				_IsPlayerWeaponFired = false;
				_score += 100;
				//break;
				goto end;
			}
		}
	}

end:
	//nop
	return;
}

void Game::HandleGameOver()
{
	if (EntityManager::GetPlayer()->m_enabled == false)
	{
		DisplayGameOver();
	}

	if (_lives == 0)
	{
		DisplayGameOver();
	}
}

void Game::DisplayGameOver()
{
	if (_lives == 0)
	{
		mText.setFillColor(sf::Color::Green);
		mText.setFont(mFont);
		mText.setPosition(200.f, 200.f);
		mText.setCharacterSize(80);

		mText.setString("GAME OVER");

		_IsGameOver = true;
		mainAudio.stop();
	}
	else
	{
		ResetSprites();
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::Up)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::Down)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::Left)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::Right)
		mIsMovingRight = isPressed;

	if (key == sf::Keyboard::Space)
	{
		if (isPressed == false)
		{
			return;
		}

		if (_IsPlayerWeaponFired == true)
		{
			return;
		}

		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sw->m_sprite.setTexture(_TextureWeapon);
		sw->m_sprite.setRotation(70.f);
        sw->m_sprite.setScale(2.f, 2.f);
		sw->m_sprite.setPosition(
			EntityManager::GetPlayer()->m_sprite.getPosition().x +
			EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x / 2.f,
			EntityManager::GetPlayer()->m_sprite.getPosition().y +
			EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y / 2.f);
		sw->m_type = EntityType::weapon;
		sw->m_size = _TextureWeapon.getSize();
		EntityManager::m_Entities.push_back(sw);

		_IsPlayerWeaponFired = true;
	}
}
