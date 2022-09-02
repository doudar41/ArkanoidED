#pragma once

#include "Framework.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <format>

//std::cout << "\n Vector - x " + std::to_string(rectPoints[1].x) + "/ y " + std::to_string(rectPoints[1].y);

class Vector {
public:
	float x, y;
	
	void Set(float xTemp, float yTemp) {
		x = xTemp;
		y = yTemp;
	}
	

	float ClampFloat(float source, float lowLimit, float highLimit) 
	{
		float result = source;
		if (source < lowLimit) {return lowLimit;	}
		if (source > highLimit) { return highLimit; }
		return result;
	}

	Vector NormalizeVector(Vector vector, Vector circleCenter)
	{
		float sx = vector.x - circleCenter.x;
		float sy = vector.y - circleCenter.y;
		float magnitude, mx, my;
		magnitude = std::sqrt(sx * sx + sy * sy);
	
		mx = (1.0f / magnitude) * sx;
		my = (1.0f / magnitude) * sy;
		//std::cout << "\n Vector - x " + std::to_string(magnitude) + "/ y " + std::to_string(my);
		return { mx,my };
	}

	Vector TurnVector( Vector vector) {
		//opposite direction
		float vx, vy;
		vy = vector.x;
		vx = vector.y;
		vector.x = -vx;
		vector.y = vy;

		return vector;
	}
};

/// <summary>
/// A Ball movement logic
/// </summary>

class Circle {
public:
	bool turnAway = false;
	Vector center = {0,0};
	float radius = 10;
	float speedModifier = 1;
	Sprite* sprite = nullptr;
	Vector spriteSize = {10,10};
	Vector direction = { 0,0 };
	

	void SetCircle(const char* path, float positionX, float positionY)
	{

		int wSize, hSize;
		sprite = createSprite(path);
		getSpriteSize(sprite, wSize, hSize);
		spriteSize = { (float)wSize , (float)hSize };
		radius = wSize * 0.5f;
		center = { positionX , positionY };
	}

	void SetPosition(float positionX, float positionY) {

		if (sprite != nullptr) {
			center.x = positionX;
			center.y = positionY;
			drawSprite(sprite, (int)center.x, (int)center.y);
		}
		else {
			std::cout << "no sprite to draw";
		}
		
	}

	void Addvector(Vector vector) {
		direction = vector.NormalizeVector(vector, center);
	
	}

	 void TurnCircle() 
	{
		direction = direction.TurnVector(direction);
	}

	bool MoveCircle() {
		int wSize, hSize;
		getScreenSize(wSize, hSize);
		if (center.x <= 0 || center.y <= 0 || center.x + radius * 2 >= wSize || turnAway) {
			TurnCircle(); turnAway = false;
		}
		//speedModifier = std::abs(direction.x * direction.y);
		SetPosition(center.x + direction.x * speedModifier, center.y + direction.y * speedModifier);
		if (center.y >= hSize) { return false; } //std::cout << "\n end game";

		return true;
	}

	void Stop() {
		direction = { 0,0 };
		drawSprite(sprite, (int)center.x, (int)center.y);
	}

};


/// <summary>
/// All block movements and collisions
/// </summary>

class Rectangle {
public:
	Vector rectPoints[4] = {0,0,0,0};
	Vector position = {400,550};
	Vector size = {100,30};
	Vector direction = { 0,0 };
	Sprite* sprite = nullptr;
	int lives = 0;
	float speedModifier = 3;
	bool hitRegistered = false;
	int boostID = 0;


	Rectangle(const char* path, float positionX, float positionY, float xSize, float ySize, int blocklives) {
		int wSize =0, hSize= 0;
		sprite = createSprite(path);
		lives = blocklives;
		if (sprite != nullptr) {

			setSpriteSize(sprite, (int)xSize, (int)ySize);
			position.Set(positionX, positionY);
			size = { xSize, ySize };
			rectPoints[0] = position; //TopLeft
			rectPoints[1] = { position.x, position.y + size.y }; //BottomLeft
			rectPoints[2] = { position.x + size.x, position.y }; //TopRight
			rectPoints[3] = { position.x + size.x, position.y + size.y }; //BottomRight
		}
	}

	void SetSettings(const char* path, float positionX, float positionY, float xSize, float ySize, int blocklives) {
		int wSize = 0, hSize = 0;
		sprite = createSprite(path);
		lives = blocklives;
		if (sprite != nullptr) {

			setSpriteSize(sprite, (int)xSize, (int)ySize);
			position.Set(positionX, positionY);
			size = { xSize, ySize };
			rectPoints[0] = position; //TopLeft
			rectPoints[1] = { position.x, position.y + size.y }; //BottomLeft
			rectPoints[2] = { position.x + size.x, position.y }; //TopRight
			rectPoints[3] = { position.x + size.x, position.y + size.y }; //BottomRight
		}

	}

	void SetBoost(int boost) {
		boostID = boost;
	}

	void SetPosition(float positionX, float positionY) {

		position.x = positionX;
		position.y = positionY;
		if (sprite != nullptr) {
			drawSprite(sprite, (int)positionX, (int)positionY);
		}
		else {
			std::cout << "no sprite to draw \n";
		}
		rectPoints[0] = position; //TopLeft
		rectPoints[1] = { position.x, position.y + size.y }; //BottomLeft
		rectPoints[2] = { position.x + size.x, position.y }; //TopRight
		rectPoints[3] = { position.x + size.x, position.y + size.y }; //BottomRight
	}

	Vector GetPosition() {
		return position;
	}

	void MovePlayer(bool rightLeft, float limitX, float limitY)
	{
		int wSize, hSize;
		getScreenSize(wSize, hSize);

		if (rightLeft) {
			direction = { -1,0 };
			if (position.x <= limitX) {
				direction = { 1,0 };
			}
			
			SetPosition(position.x + direction.x * speedModifier, position.y);
		}
		else
		{
			direction = { 1,0 };
			if (position.x + size.x >= limitY) {
				direction = { -1,0 };
			}
			SetPosition(position.x + direction.x * speedModifier, position.y);
		}
	}

	bool MoveDown(int yLimit) {

		direction = { 0,1 };
		SetPosition(position.x, position.y + direction.y * 1);
		if (position.y > yLimit) {
			return true;
		}
		return false;
	}

	bool CheckPoint(Vector ball)
	{
		if (ball.x >= rectPoints[0].x &&
			ball.y >= rectPoints[0].y &&
			ball.x < rectPoints[3].x &&
			ball.y < rectPoints[3].y) 
		{
			return true;
		}
		return false;
	}

	bool CheckBlocksCollision(Rectangle& player)
	{

			if (/* vertical check*/ player.rectPoints[0].y <= rectPoints[3].y
				&& player.rectPoints[3].y >= rectPoints[0].y
				/*horizontal check*/
				&& player.rectPoints[1].x <= rectPoints[2].x
				&& player.rectPoints[2].x >= rectPoints[1].x) {
				return true;
			}

		return false;
	}


	bool CheckCirleCollision(Circle ball) {
		Vector realcenter = { ball.radius + ball.center.x,ball.radius + ball.center.y };

		if (realcenter.x >= rectPoints[0].x &&
			realcenter.y >= rectPoints[0].y &&
			realcenter.x < rectPoints[3].x &&
			realcenter.y < rectPoints[3].y)
		{
			return true;
		}
		return false;
	}


	int  BoostDownCheck(Rectangle& player, int ID, int boostLimit) {
		if (boostLimit < 3) {
			switch (ID) {
			case 1:
				player.SetSettings("data/Player.png", player.position.x, player.position.y, player.size.x * 1.5, player.size.y, 0);
				return boostLimit +=1;
				break;

			}
		}
		return boostLimit;
	}


	void Stop()
	{
		direction = { 0,0 };
		SetPosition(position.x + direction.x * speedModifier, position.y);
	}


	~Rectangle() {
		//std::cout << "player destroyed";
	}

	bool DestroyRect() {

		if (lives > 0) {
			lives--;
			if (lives <= 0) {
				return true;
			}
		}
		return false;
	}
};

