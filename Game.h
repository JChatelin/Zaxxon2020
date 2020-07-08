#pragma once
#include "Weapon.h"

#define SPRITE_COUNT_Y 5

class Game
{
public:
	Game();
	~Game() { };
	void run();

private:
	void processEvents();
	void update(sf::Time elapsedTime);
	void render();

	void InitSprites();
	void ResetSprites();

	void spawnEnemies();

	void updateStatistics(sf::Time elapsedTime);
	void HandleTexts();
	void HandleCollisionEnemyMasterWeaponPlayer();
	void HanldeEnemyMasterWeaponMoves();
	void HandleEnemyMasterWeaponFiring();
	void HandleEnemyMasterSpawn();
	void HandleEnemyMasterMove();
	void HandleCollisionWeaponPlayer();
	void HanldeEnemyWeaponMoves();
	void HandleEnemyWeaponFiring();
	void HandleEnemySpawn();
	void HandleEnemyMoves();
	void HanldeWeaponMoves();
	void HandleCollisionWeaponEnemy();
	void HandleCollisionWeaponEnemyMaster();
	void HandleGameOver();
	void DisplayGameOver();
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

private:
	static const float		PlayerSpeed;
	static const float      WeaponSpeed;
	static const sf::Time	TimePerFrame;
	static const sf::Time   TimeEnemySpawn;
	static const sf::Time   TimeEnemyMasterSpawn;
	static const sf::Time   TimeSpawnWave;

	sf::RenderWindow		mWindow;
	sf::Texture	mTexture;
	sf::Sprite	mPlayer;
	sf::Font	mFont;
	sf::Text	mStatisticsText;
	sf::Time	mStatisticsUpdateTime;
	sf::Text	mText;
	sf::Text	_LivesText;
	int _lives = 3;
	sf::Text	_ScoreText;
	int _score = 0;

	std::size_t	mStatisticsNumFrames;
	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingRight;
	bool mIsMovingLeft;

	bool _IsGameOver = false;
	bool _IsEnemyWeaponFired = false;
	bool _IsPlayerWeaponFired = false;
	bool _IsEnemyMasterWeaponFired = false;

	sf::Texture	_TextureEnemy;
	sf::Sprite	_Enemy[SPRITE_COUNT_Y];
	sf::Texture	_TextureWeapon;
	sf::Texture	_TextureWeaponEnemy;
	sf::Texture	_TextureWeaponEnemyMaster;
	sf::Sprite	_Weapon;
	sf::Texture	_TextureEnemyMaster;
	sf::Sprite	_EnemyMaster;
};

