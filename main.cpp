
#include <SimpleECS_Core.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

using namespace std;
using namespace SimpleECS;

const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH = 640;
const int WALL_THICKNESS = 50000;
const int PADDLE_LENGTH = 45;

int leftScore = 0;
int rightScore = 0;

FontRenderer* leftText;
FontRenderer* rightText;

Entity* createBall();

// Component for controlling paddle movement manually
class PaddleController : public Component {

	bool isPlayer1;
public:
	PaddleController(bool isPlayer1) : isPlayer1(isPlayer1) {};
	
	void initialize() {}

	void update() override
	{
		if (!isPlayer1)
		{
			if (Input::getKeyDown(KeyCode::KEY_DOWN_ARROW) 
				&& entity->transform.position.y > -SCREEN_HEIGHT/2 + PADDLE_LENGTH)
			{
				entity->transform.position.y -= 3;
			}
			else if (Input::getKeyDown(KeyCode::KEY_UP_ARROW)
				&& entity->transform.position.y < SCREEN_HEIGHT / 2 - PADDLE_LENGTH)
			{
				entity->transform.position.y += 3;
			}
		}
		else
		{
			if (Input::getKeyDown(KeyCode::KEY_S)
				&& entity->transform.position.y > -SCREEN_HEIGHT / 2 + PADDLE_LENGTH)
			{
				entity->transform.position.y -= 3;
			}
			else if (Input::getKeyDown(KeyCode::KEY_W)
				&& entity->transform.position.y < SCREEN_HEIGHT / 2 - PADDLE_LENGTH)
			{
				entity->transform.position.y += 3;
			}
		}
	}
};

// Component for giving ball initial velocity and outputting collision information
class BallController : public Component {
public:	
	const bool CONTROL_ENABLE = false;

	void initialize() override
	{
		// Set references and starting velocity
		phys = entity->getComponent<PhysicsBody>();

		const int MAX_SPEED = 400;
		const int MIN_SPEED = 200;
		phys->velocity.x = MIN_SPEED + (rand() % static_cast<int>(MAX_SPEED - MIN_SPEED + 1))/2;
		phys->velocity.y = MIN_SPEED + (rand() % static_cast<int>(MAX_SPEED - MIN_SPEED + 1));
	}

	void update() override {}

	void onCollide(const Collision& collide) override
	{
		cout << "Collision!" << endl;
		cout << "x : " << collide.normal.x << " ";
		cout << "y : " << collide.normal.y << " ";
		cout << "penDepth : " << collide.penetration << " ";
	}

private:
	PhysicsBody* phys = nullptr;
};

// Component for registering score
class BoundScoreRegister : public Component {
public: 
	BoundScoreRegister(bool isPlayerLeft, Scene* activeScene) 
		: isPlayerLeft(isPlayerLeft), activeScene(activeScene) {}
	void update() override {}
	void initialize() override {}
	void onCollide(const Collider& other) override 
	{
		if (other.entity->tag != "ball") return;

		// Ball has collided. Add score, destroy ball and respawn
		activeScene->DestroyEntity(other.entity);
		
		if (isPlayerLeft)
		{
			leftScore++;
			leftText->text = std::to_string(leftScore);
		}
		else
		{
			rightScore++;
			rightText->text = std::to_string(rightScore);
		}

		Entity* ball = createBall();
		activeScene->AddEntity(ball);
		ball->getComponent<BallController>()->initialize();
	}

	bool isPlayerLeft;
	Scene* activeScene;
};

Entity* createPaddle(bool isLeft)
{
	// Create left paddle and add to scene
	Entity* paddle = new Entity();

	paddle->addComponent(new RectangleRenderer(10, PADDLE_LENGTH, Color(0xFF, 0xFF, 0xFF)));
	paddle->addComponent(new PaddleController(isLeft));
	paddle->addComponent(new BoxCollider(10, PADDLE_LENGTH));

	paddle->transform.position.x = isLeft ? -SCREEN_WIDTH / 2 + 20 : SCREEN_WIDTH / 2 - 20;

	return paddle;
}

Entity* createBall()
{
	Entity* staticEntity = new Entity("ball");
	Component* staticComp = new RectangleRenderer(15, 15, Color(0xFF, 0xFF, 0xFF, 0xFF));
	staticEntity->addComponent(staticComp);

	Component* staticCollide = new BoxCollider(15, 15);
	staticEntity->addComponent(staticCollide);

	Component* controller = new BallController();
	staticEntity->addComponent(controller);

	PhysicsBody* physics = new PhysicsBody();
	staticEntity->addComponent(physics);

	return staticEntity;
}

void createBounds(Scene* scene)
{
	Entity* topBound = new Entity();
	topBound->transform.position.y = SCREEN_HEIGHT / 2 + WALL_THICKNESS / 2;
	Component* topCollide = new BoxCollider(SCREEN_WIDTH + WALL_THICKNESS, WALL_THICKNESS);
	topBound->addComponent(topCollide);
	

	Entity* bottomBound = new Entity();
	bottomBound->transform.position.y = -SCREEN_HEIGHT / 2 - WALL_THICKNESS / 2;
	Component* bottomCollide = new BoxCollider(SCREEN_WIDTH + WALL_THICKNESS, WALL_THICKNESS);
	bottomBound->addComponent(bottomCollide);
	
	Entity* leftBound = new Entity();
	leftBound->transform.position.x = -SCREEN_WIDTH / 2 - WALL_THICKNESS / 2;
	Component* leftCollide = new BoxCollider(WALL_THICKNESS, SCREEN_HEIGHT + WALL_THICKNESS);
	leftBound->addComponent(leftCollide);
	leftBound->addComponent(new BoundScoreRegister(true, scene));
	
	Entity* rightBound = new Entity();
	rightBound->transform.position.x = SCREEN_WIDTH / 2 + WALL_THICKNESS / 2;
	Component* rightCollide = new BoxCollider(WALL_THICKNESS, SCREEN_HEIGHT + WALL_THICKNESS);
	rightBound->addComponent(rightCollide);
	rightBound->addComponent(new BoundScoreRegister(false, scene));
	
	scene->AddEntity(topBound);
	scene->AddEntity(bottomBound);
	scene->AddEntity(rightBound);
}

void createText(Scene* scene)
{
	Entity* leftScore = new Entity();
	leftScore->transform.position = Vector(-SCREEN_WIDTH / 4, 200);
	leftText = new FontRenderer("0", "bit9x9.ttf", 54);
	leftText->color = Color(0xFF, 0xFF, 0xFF, 0xFF);
	leftScore->addComponent(leftText);

	Entity* rightScore = new Entity();
	rightScore->transform.position = Vector(SCREEN_WIDTH / 4, 200);;
	rightText = new FontRenderer("0", "bit9x9.ttf", 54);
	rightText->color = Color(0xFF, 0xFF, 0xFF, 0xFF);
	rightScore->addComponent(rightText);

	scene->AddEntity(leftScore);
	scene->AddEntity(rightScore);
}

int main() {
	cout << "Hello World!" << endl;
	
	// Setup game
	Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
	Scene* startScene = new Scene(Color(0,0,0,255));
	game.addScene(startScene);
	createBounds(startScene);
	createText(startScene);

	// Create center line
	Entity* line = new Entity();
	line->addComponent(new LineRenderer(Vector(0, -240), Vector(0, 300), 5, Color(0xFF, 0xFF, 0xFF), 15));
	startScene->AddEntity(line);

	// Create paddles
	startScene->AddEntity(createPaddle(false));
	startScene->AddEntity(createPaddle(true));

	// Create moveable Square and add to scene
	startScene->AddEntity(createBall());	

	// Start game loop
	game.startGame();
}