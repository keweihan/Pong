/*
Stress test of SimpleECS collision system. 

Kewei Han
*/
#include <SimpleECS_Core.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <memory>

using namespace std;
using namespace SimpleECS;

// Environment parameters
const int SCREEN_HEIGHT		= 540;
const int SCREEN_WIDTH		= 960;
const int WALL_THICKNESS	= 50000;

// Ball parameters
const int NUM_BALLS = 3;
const int MAX_SPEED	= 40;
const int MIN_SPEED	= 40;
const int X_SPEED		= 30;
const int SIDE_LENGTH = 20;

// Globals
Scene* pongScene;

class FrameCounter : public Component {
public:

	void initialize() {
		textRender = entity->getComponent<FontRenderer>();
	};

	void update() {
		framesPassed++;
		int64_t lifeTime = Timer::getProgramLifetime();
		int64_t avgFPS = lifeTime == 0 ? 0 : framesPassed / (Timer::getProgramLifetime() / 1000 + 1);
		string text = "Average FPS: " + std::to_string(avgFPS);

		textRender->text = text;
		entity->transform.position = Vector(0, 0);
	}

	uint64_t framesPassed = 0;
	FontRenderer* textRender = nullptr;
};


Entity* createFramesCounter()
{
	Entity* counter = new Entity();
	counter->addComponent(new FontRenderer("Default", "assets/bit9x9.ttf", 26, Color(0xff, 0xff,0xff,0xff)));
	counter->addComponent(new FrameCounter());
	return counter;
}

// Create ball with initial position and inbuilt randomized velocity
Entity* createBall(const int& x, const int &y)
{
	Entity* newBall = new Entity("ball");
	Component* staticComp = new RectangleRenderer(10, 10, Color(0xFF, 0xFF, 0xFF, 0xFF));
	newBall->addComponent(staticComp);

	Component* staticCollide = new BoxCollider(10, 10);
	newBall->addComponent(staticCollide);

	PhysicsBody* physics = new PhysicsBody();
	newBall->addComponent(physics);

	// Set position
	newBall->transform.position.x = x;
	newBall->transform.position.y = y;

	// Randomize direction and speed
	physics->velocity.x = (MIN_SPEED + (rand() % static_cast<int>(MAX_SPEED - MIN_SPEED + 1))) * (rand() % 2 == 0 ? -1 : 1);
	physics->velocity.y = (MIN_SPEED + (rand() % static_cast<int>(MAX_SPEED - MIN_SPEED + 1))) * (rand() % 2 == 0 ? -1 : 1);

	return newBall;
}

// Create a floor/ceiling object with sound effect on collision
Entity* createFloorCeilingWall()
{
	Entity* wall = new Entity();
	wall->addComponent(new BoxCollider(SCREEN_WIDTH + WALL_THICKNESS, WALL_THICKNESS));
	return wall;
}

// Create a side walls with sound effect and score tallying on collision
Entity* createSideWalls()
{
	Entity* wall = new Entity();
	wall->addComponent(new BoxCollider(WALL_THICKNESS, SCREEN_HEIGHT + WALL_THICKNESS));
	return wall;
}

// Create upper and lower walls in scene, and side walls that keep score tracking
void addBounds()
{
	Entity* topBound = createFloorCeilingWall();
	topBound->transform.position.y = SCREEN_HEIGHT / 2 + WALL_THICKNESS / 2;
	
	Entity* bottomBound = createFloorCeilingWall();
	bottomBound->transform.position.y = -SCREEN_HEIGHT / 2 - WALL_THICKNESS / 2;
	
	Entity* leftBound = createSideWalls();
	leftBound->transform.position.x = -SCREEN_WIDTH / 2 - WALL_THICKNESS / 2;

	Entity* rightBound = createSideWalls();
	rightBound->transform.position.x = SCREEN_WIDTH / 2 + WALL_THICKNESS / 2;
	
	pongScene->AddEntity(topBound);
	pongScene->AddEntity(bottomBound);
	pongScene->AddEntity(rightBound);
	pongScene->AddEntity(leftBound);
}

// Spawn balls with physics in a grid across screen
void spawnBalls(const int& numRow, const int& numColumn, const int& num)
{
	int rowSpacing		= (SCREEN_HEIGHT/numRow);
	int columnSpacing	= (SCREEN_WIDTH / numColumn);

	int ySpawnPos = -SCREEN_HEIGHT/2 + rowSpacing/2;
	int xSpawnPos = -SCREEN_WIDTH/2 + columnSpacing/2;

	int numSpawned = 0;

	for (int i = 0; i < numRow && numSpawned < num; ++i)
	{
		for (int j = 0; j < numColumn; ++j)
		{
			Entity* newBall = createBall(xSpawnPos, ySpawnPos);
			pongScene->AddEntity(newBall);
			xSpawnPos += columnSpacing;

			numSpawned++;
		}

		ySpawnPos += rowSpacing;
		xSpawnPos = -SCREEN_WIDTH / 2 + columnSpacing / 2;
	}
}

int main() {
	cout << "Hello World!" << endl;
	
	// Create scene
	pongScene = new Scene(Color(0, 0, 0, 255));

	// Populate scene
	addBounds();

	// Get a grid of squares
	int columns = ceil(sqrt(NUM_BALLS / ((double)SCREEN_HEIGHT / (double)SCREEN_WIDTH)));
	int test = ceil(NUM_BALLS / columns);
	spawnBalls(test, columns, NUM_BALLS);

	pongScene->AddEntity(createFramesCounter());
	
	// Create game with scene
	Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
	game.setName("Auto Pong");
	game.addScene(pongScene);

	// Start game loop
	game.startGame();
}