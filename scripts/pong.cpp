/*
Pong implementation using SimpleECS.

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

// Asset paths
const string FONT_FILE		= "assets/bit9x9.ttf";
const string SOUND_PADDLE	= "assets/PongPaddle.wav";
const string SOUND_WALL		= "assets/PongBlip1.wav";
const string SOUND_SCORE	= "assets/PongScore.wav";

// Environment parameters
const int SCREEN_HEIGHT		= 480;
const int SCREEN_WIDTH		= 640;
const int WALL_THICKNESS	= 50000;
const int PADDLE_LENGTH		= 45;

// Ball parameters
const int MAX_Y_SPEED	= 600;
const int MIN_Y_SPEED	= 400;
const int X_SPEED		= 400;

// Score tracking
int p1Score = 0;
int p2Score = 0;

// Globals
FontRenderer* leftText;
FontRenderer* rightText;
Scene* pongScene;
Entity* ball;

// PlayerTypes
enum PlayerType {
	PLAYER1,
	PLAYER2,
	COMPUTER1,
	COMPUTER2,
};

// Forward declares
Entity* createBall();
void spawnBall();

// Component for controlling paddle movement manually
class PaddleController : public Component {
public:
	PaddleController(PlayerType player) : player(player) {};
	
	void initialize() {}

	// Apply computer control of paddle based on ball position
	void aiControl(bool& downPressed, bool& upPressed)
	{
		bool comp2Active = ball->transform.position.x > 0 && player == COMPUTER2;
		bool comp1Active = ball->transform.position.x < 0 && player == COMPUTER1;
		if (comp2Active || comp1Active)
		{
			if (ball->transform.position.y > entity->transform->position.y)
			{
				upPressed = true;
				downPressed = false;
			}
			else
			{
				downPressed = true;
				upPressed = false;
			}
		}
	}

	void update() override
	{
		// Determine control scheme for this controller
		bool downPressed = player == PLAYER1 ?	Input::getKeyDown(KeyCode::KEY_S) :
												Input::getKeyDown(KeyCode::KEY_DOWN_ARROW);
		bool upPressed	= player == PLAYER1 ?	Input::getKeyDown(KeyCode::KEY_W) : 
												Input::getKeyDown(KeyCode::KEY_UP_ARROW);
		if(player == COMPUTER1 || player == COMPUTER2) 
		{
			aiControl(downPressed, upPressed);
		}

		// Move paddle based on input and limit movement
		if (upPressed && entity->transform->position.y < SCREEN_HEIGHT / 2 - PADDLE_LENGTH)
		{
			entity->transform->position.y += 0.4 * Timer::getDeltaTime();
		}
		else if (downPressed && entity->transform->position.y > -SCREEN_HEIGHT / 2 + PADDLE_LENGTH)
		{
			entity->transform->position.y -= 0.4 * Timer::getDeltaTime();
		}
	}

private:
	PlayerType player;
};

// Component for registering score
class BoundScoreRegister : public Component {
public: 
	BoundScoreRegister(PlayerType player) : player(player) {}
	void update() override {}
	void initialize() override {}
	void onCollide(const Collider& other) override
	{
		// Ball has collided. 
		if (other.entity->tag == "ball")
		{
			//Destroy ball
			pongScene->DestroyEntity(other.entity);

			// Tally score
			if (player == PLAYER1)
			{
				p2Score++;
				rightText->text = std::to_string(p2Score);
			}
			else
			{
				p1Score++;
				leftText->text = std::to_string(p1Score);
			}

			//Spawn new ball
			spawnBall();
		}
	}

	PlayerType player;
};

// Component for playing a sound effect on collision
class CollideSoundEffect : public Component {
public:
	CollideSoundEffect(std::string pathToEffect) {
		sound = make_shared<SoundPlayer>(pathToEffect);
	}
	void update() override {}
	void initialize() override {}
	void onCollide(const Collider& other) override
	{
		if (other.entity->tag == "ball")
		{
			sound->playAudio();
		}
	}

	shared_ptr<SoundPlayer> sound;
};

// Construct paddle for a corresponding a player type
Entity* createPaddle(PlayerType player)
{
	// Create paddle and add to scene
	Entity* paddle = new Entity();

	paddle->addComponent(new RectangleRenderer(10, PADDLE_LENGTH, Color(0xFF, 0xFF, 0xFF)));
	paddle->addComponent(new PaddleController(player));
	paddle->addComponent(new BoxCollider(10, PADDLE_LENGTH));
	paddle->addComponent(new CollideSoundEffect(SOUND_PADDLE));

	// Position differently based on player
	paddle->transform.position.x = player == PLAYER1 || player == COMPUTER1 ? -SCREEN_WIDTH / 2 + 20 : SCREEN_WIDTH / 2 - 20;

	return paddle;
}

// Create ball with initial randomized velocity
Entity* createBall()
{
	Entity* newBall = new Entity("ball");
	Component* staticComp = new RectangleRenderer(10, 10, Color(0xFF, 0xFF, 0xFF, 0xFF));
	newBall->addComponent(staticComp);

	Component* staticCollide = new BoxCollider(10, 10);
	newBall->addComponent(staticCollide);

	PhysicsBody* physics = new PhysicsBody();
	newBall->addComponent(physics);

	// Randomize direction and speed
	int direction = rand() % 2 == 0 ? -1 : 1;
	physics->velocity.x = X_SPEED * direction;
	physics->velocity.y = MIN_Y_SPEED + (rand() % static_cast<int>(MAX_Y_SPEED - MIN_Y_SPEED + 1)) * direction;

	return newBall;
}

// Create a floor/ceiling object with sound effect on collision
Entity* createFloorCeilingWall()
{
	Entity* wall = new Entity();
	wall->addComponent(new BoxCollider(SCREEN_WIDTH + WALL_THICKNESS, WALL_THICKNESS));
	wall->addComponent(new CollideSoundEffect(SOUND_WALL));
	return wall;
}

// Create a side walls with sound effect and score tallying on collision
Entity* createSideWalls(PlayerType player)
{
	Entity* wall = new Entity();
	wall->addComponent(new BoxCollider(WALL_THICKNESS, SCREEN_HEIGHT + WALL_THICKNESS));
	wall->addComponent(new BoundScoreRegister(player));
	wall->addComponent(new CollideSoundEffect(SOUND_SCORE));
	return wall;
}

// Create center line visual object
Entity* createCenterLine()
{
	Entity* line = new Entity();
	line->addComponent(new LineRenderer(Vector(0, -SCREEN_HEIGHT), Vector(0, SCREEN_HEIGHT), 5, Color(0xFF, 0xFF, 0xFF), 15));
	return line;
}

// Create upper and lower walls in scene, and side walls that keep score tracking
void addBounds()
{
	// Create and position top and bottom colliders
	Entity* topBound = createFloorCeilingWall();
	topBound->transform.position.y = SCREEN_HEIGHT / 2 + WALL_THICKNESS / 2;
	
	Entity* bottomBound = createFloorCeilingWall();
	bottomBound->transform.position.y = -SCREEN_HEIGHT / 2 - WALL_THICKNESS / 2;
	
	// Create and position side colliders with score tracker component
	Entity* leftBound = createSideWalls(PLAYER1);
	leftBound->transform.position.x = -SCREEN_WIDTH / 2 - WALL_THICKNESS / 2;

	Entity* rightBound = createSideWalls(PLAYER2);
	rightBound->transform.position.x = SCREEN_WIDTH / 2 + WALL_THICKNESS / 2;
	
	pongScene->AddEntity(topBound);
	pongScene->AddEntity(bottomBound);
	pongScene->AddEntity(rightBound);
	pongScene->AddEntity(leftBound);
}

// Create and position score text in scene
void addScoreCounters()
{
	Entity* leftScore = new Entity();
	leftScore->transform.position = Vector(-SCREEN_WIDTH / 4, 200);
	leftText = new FontRenderer("0", FONT_FILE, 54);
	leftText->color = Color(0xFF, 0xFF, 0xFF, 0xFF);
	leftScore->addComponent(leftText);

	Entity* rightScore = new Entity();
	rightScore->transform.position = Vector(SCREEN_WIDTH / 4, 200);;
	rightText = new FontRenderer("0", FONT_FILE, 54);
	rightText->color = Color(0xFF, 0xFF, 0xFF, 0xFF);
	rightScore->addComponent(rightText);

	pongScene->AddEntity(leftScore);
	pongScene->AddEntity(rightScore);
}

void spawnBall()
{
	Entity* newBall = createBall();
	pongScene->AddEntity(newBall);
	ball = newBall;
}

int main() {
	cout << "Hello World!" << endl;
	
	// Create scene
	pongScene = new Scene(Color(0, 0, 0, 255));

	// Populate scene
	addBounds();
	addScoreCounters();
	pongScene->AddEntity(createCenterLine());
	pongScene->AddEntity(createPaddle(COMPUTER1));
	pongScene->AddEntity(createPaddle(COMPUTER2));
	spawnBall();

	// Create game with scene
	Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
	game.setName("Auto Pong");
	game.addScene(pongScene);

	// Start game loop
	game.startGame();
}