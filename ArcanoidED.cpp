

#include "Framework.h"
#include "Objects.h"
#include <vector>
#include <algorithm>

/* Test Framework realization */
class GameMode : public Framework {

public:

	///VARIABLES

	Rectangle player = Rectangle::Rectangle("data/Player.png", 400, 550, 100, 30, 0);
	Vector playerPosition = { 300 ,550 };
	Circle ball; Vector ballPosition = { 350 ,530 };

	std::vector <Rectangle> breakableBlocksRed, breakableBlocksGreen, breakableBlocksOrange;
	std::vector <Rectangle> walls;
	std::vector <Rectangle> fallingBlocks;
	std::vector <Rectangle> menuPics;

	int cupWidth = 8, brickWidth = 100, brickHeight = 30, cupStartDrawPoint = 0 ;
	int screenWidth = 800, screenHeight = 600;
	int attempts = 5; 
	bool startGame = false, gameOver = false, timerRunning = false;

	int countTime, setTime, boostLimit;


	Vector mouseClickXY;

	bool	movePlayerLeft = false, movePlayerRight = false, 
			moveCircle = false,  spawnBoost = false;
	int mouseCursorX = 0, mouseCursorY = 0;


	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = screenWidth;
		height = screenHeight;
		fullscreen = false;
	}

	virtual bool Init() {


		MenuPicsList();
		player.SetSettings("data/Player.png", 300, 550, brickWidth, brickHeight, 0);

		InitializeBlocks();
		ball.SetCircle("data/63-Breakout-Tiles.png", 350, 530);

		
		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick() {
		drawTestBackground();
		
		if (timerRunning) {
			if (CompareTime()) {
				player.SetSettings("data/Player.png", player.position.x, player.position.y, brickWidth, brickHeight, 0);
				boostLimit = 0;
			}
		}


		if (startGame) 
		{
			MovePlayer();
			MovingBall();
			//DrawingWall();
			BallVsBreakableBlocksCollision(breakableBlocksGreen);
			BallVsBreakableBlocksCollision(breakableBlocksOrange);
			BoostDrop();

			// PlayerVSBallCollision
			if (player.CheckPoint(ball.center)) {
				ball.turnAway = true;
			}
		}

		if(!startGame)
		{
			if (!menuPics.empty()) {
				menuPics.at(attempts).Stop();
			}
		}
		GameEnd();
		return false;
	}


	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		mouseCursorX = x;
		mouseCursorY = y;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		switch (button) {
		case FRMouseButton::LEFT:
			if (startGame) {
				
				ball.Addvector ({(float)mouseCursorX, (float)mouseCursorY});
				moveCircle = true;
			}
			else
			{
				if (gameOver) {
					attempts = 5;
					InitializeBlocks();	
					gameOver = false;//std::exit(EXIT_SUCCESS);
				}
				startGame = true;
			}
			break;
		}
	}

	virtual void onKeyPressed(FRKey k) {

		switch (k) {
		case FRKey::LEFT:
			movePlayerLeft = true;
			break;
		case FRKey::RIGHT:
			movePlayerRight = true;

			break;
		}
	}

	virtual void onKeyReleased(FRKey k) {

		switch (k) {
		case FRKey::LEFT:
			movePlayerLeft = false;
			break;
		case FRKey::RIGHT:
			movePlayerRight = false;
			break;
		}
	}

	virtual const char* GetTitle() override
	{
		return "Arcanoid";
	}

	///////////
	/// CUSTOM FUNCTIONS
	//////////


	void InitializeBlocks() {
		for (int i = 50; i < 700; i += 50) {
			breakableBlocksGreen.push_back(Rectangle("data/03-Breakout-Tiles.png", i, 100, 50, 50, 1));
		}
		for (int i = 50; i < 700; i += 50) {
			breakableBlocksOrange.push_back(Rectangle("data/09-Breakout-Tiles.png", i, 150, 50, 50, 2));
		}

	}


	void GameEnd() {
		if (breakableBlocksGreen.empty()&& breakableBlocksOrange.empty()) {
			attempts = 6;
			gameOver = true;
			startGame = false;
		}
	}

	int SetTimer(int delay) {
		setTime  = getTickCount() + delay;
		timerRunning = true;
		return setTime;
	}

	bool CompareTime()
	{
			if (setTime > getTickCount()) {
				return false;
			}
			else 
			{
				timerRunning = false;
				return true;
			}

	}

	void MovingBall()
	{
		if (moveCircle) {
			if (!ball.MoveCircle())
			{
				ball.Stop();
				ball.SetPosition(ballPosition.x, ballPosition.y);
				player.SetSettings("data/Player.png", 300, 550, brickWidth, brickHeight, 0);
				if (attempts > 0) {
					attempts--;
					if (attempts <= 0) { gameOver = true; }
				}
				startGame = false;
			}
		}
		else ball.Stop();
	}


	void MenuPicsList()
	{
		menuPics = { Rectangle("data/GameOver.png", 200, 250, 400, 100, 0),
						Rectangle("data/OneAttempt.png", 200, 250, 400, 100, 0),
						Rectangle("data/TwoAttempts.png", 200, 250, 400, 100, 0) ,
						Rectangle("data/ThreeAttempts.png", 200, 250, 400, 100, 0),
						Rectangle("data/FourAttempts.png", 200, 250, 400, 100, 0) ,
						Rectangle("data/ClickAnywhere.png", 200, 250, 400, 100, 0),
						Rectangle("data/YouWon.png", 200, 250, 400, 100, 0) };
	}

	void BoostDrop()
	{
		if (!fallingBlocks.empty()) {
			for (int i = 0; i < fallingBlocks.size(); i++)
			{
				if (fallingBlocks.at(i).MoveDown(screenHeight) || fallingBlocks.at(i).CheckBlocksCollision(player))
				{
					if (fallingBlocks.at(i).CheckBlocksCollision(player)) {
	
						boostLimit =  fallingBlocks.at(i).BoostDownCheck(player, fallingBlocks.at(i).boostID, boostLimit);
					}
					fallingBlocks.erase(fallingBlocks.begin() + i);
				}
			}
		}
	}

	void DrawingWall() {
		for (auto& rect : walls)
		{
			rect.Stop();
			if (rect.CheckPoint(ball.center)) {
				ball.turnAway = true;
			}
		}

	}

	void BallVsBreakableBlocksCollision(std::vector<Rectangle>& breakableBlocks) {

		for (int i = 0; i < breakableBlocks.size(); i++)
		{
			breakableBlocks.at(i).Stop();

			if (breakableBlocks.at(i).CheckCirleCollision (ball)) {
				ball.turnAway = true;
				if (breakableBlocks.at(i).DestroyRect()) {

					if (CreateBoost(breakableBlocks.at(i).position, brickWidth, brickHeight, fallingBlocks))
					{
						std::cout << "\n boost  - " << std::to_string(boostLimit);
						if (boostLimit < 3) {
							SetTimer(10000);
						}
				
					}
					//SpawnRandom(breakableBlocks.at(i).position);
					breakableBlocks.erase(breakableBlocks.begin() + i);
					
				}
			}
		}
	}


	void MovePlayer() 
	{
		if (movePlayerLeft)
		{
			player.MovePlayer(true,(float)cupStartDrawPoint+ brickHeight, (float)cupStartDrawPoint + cupWidth * brickWidth- brickHeight);
		}
		else player.Stop();

		if (movePlayerRight)
		{
			player.MovePlayer(false, (float)cupStartDrawPoint + brickHeight, (float)cupStartDrawPoint + cupWidth * brickWidth - brickHeight);
		}
		else player.Stop();
	}


	bool CreateBoost(Vector vector, float brickWidth, float brickHeight, std::vector<Rectangle>& Blocks) {
		int randomInt = rand() % 10 + 1;
		if (randomInt == 2 || randomInt == 7) {
			Blocks.push_back(Rectangle("data/47-Breakout-Tiles.png", vector.x, vector.y, brickWidth, brickHeight, 1));
			Blocks.at(Blocks.size() - 1).boostID = 1;
			return true;
		}
		return false;
	}

	void BuildWall() 
	{

		for (int i= cupStartDrawPoint; i <= cupWidth* brickWidth; i += brickWidth)
			{
					walls.push_back(Rectangle("data/01-Breakout-Tiles.png", (float)i, 0, brickWidth, brickHeight, 0));
			}
		for (int i = brickHeight; i <= screenHeight+ brickHeight; i += brickWidth)
			{
					walls.push_back(Rectangle("data/01-Breakout-Tiles_R.png", cupStartDrawPoint, (float)i, brickHeight*1.23, brickWidth, 0));
			}
		for (int i = brickHeight; i <= screenHeight + brickHeight; i += brickWidth)
			{
					walls.push_back(Rectangle("data/01-Breakout-Tiles_R.png", 600-37, (float)i, 37, 100, 0));
			}
	}
};

int main(int argc, char* argv[])
{

	return run(new GameMode);
}
