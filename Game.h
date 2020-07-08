#pragma once

#define SPRITE_COUNT 5

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

    void HandleEnemySpawn();
    void HandleEnemyMasterSpawn();
    void spawnEnemies(sf::Time elapsedTime);

	void updateStatistics(sf::Time elapsedTime);
	void HandleTexts();
	void HandleCollisionEnemyMasterWeaponPlayer();
	void HanldeEnemyMasterWeaponMoves();
	void HandleEnemyMasterWeaponFiring();
	void HandleEnemyMasterMove();
	void HandleCollisionWeaponPlayer();
	void HanldeEnemyWeaponMoves();
	void HandleEnemyWeaponFiring();
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
    static float      TimeEnemyMasterSpawn;
    static float      TimeSpawnWave;
	static const sf::Time	TimePerFrame;

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
	sf::Sprite	_Enemy[SPRITE_COUNT];
	sf::Texture	_TextureWeapon;
	sf::Texture	_TextureWeaponEnemy;
	sf::Texture	_TextureWeaponEnemyMaster;
	sf::Sprite	_Weapon;
	sf::Texture	_TextureEnemyMaster;
	sf::Sprite	_EnemyMaster;
};

