
#include <SimpleECS_Core.h>;

#include <iostream>;
#include <cstdlib>
#include <vector>;

using namespace std;
using namespace SimpleECS;

class PaddleController : public Component {

	bool isPlayer1;
public:
	PaddleController(bool isPlayer1) : isPlayer1(isPlayer1) {};
	
	void initialize() override
	{

	}

	void update() override
	{
		if (!isPlayer1)
		{
			if (Input::getKeyDown(KeyCode::KEY_DOWN_ARROW))
			{
				entity->transform.posY -= 3;
			}
			else if (Input::getKeyDown(KeyCode::KEY_UP_ARROW))
			{
				entity->transform.posY += 3;
			}
		}
		else
		{
			if (Input::getKeyDown(KeyCode::KEY_W))
			{
				entity->transform.posY += 3;
			}
			else if (Input::getKeyDown(KeyCode::KEY_S))
			{
				entity->transform.posY -= 3;
			}
		}
	}
};

class FreeController : public Component {
private:
	PhysicsBody* phys = nullptr;

public:
	
	const bool CONTROL_ENABLE = false;

	void initialize() override
	{
		phys = entity->getComponent<PhysicsBody>();

		const int MAX_SPEED = 600;
		const int MIN_SPEED = 200;
		phys->velocity.x = MIN_SPEED + (rand() % static_cast<int>(MAX_SPEED - MIN_SPEED + 1));
		phys->velocity.y = MIN_SPEED + (rand() % static_cast<int>(MAX_SPEED - MIN_SPEED + 1));
	}

	void update() override
	{
		if (!CONTROL_ENABLE) return;
		if (Input::getKeyDown(KeyCode::KEY_DOWN_ARROW))
		{
			//entity->transform.posY -= 1;
			phys->velocity.y = -100;
		}
		else if (Input::getKeyDown(KeyCode::KEY_UP_ARROW))
		{
			//entity->transform.posY += 1;
			phys->velocity.y = 100;
		}
		else if (Input::getKeyDown(KeyCode::KEY_RIGHT_ARROW))
		{
			//entity->transform.posX += 1;
			phys->velocity.x = 100;
		}
		else if (Input::getKeyDown(KeyCode::KEY_LEFT_ARROW))
		{
			//entity->transform.posX -= 1;
			phys->velocity.x = -100;
		}
		else
		{

		}

		//cout << "Frame Time" << Timer::getDeltaTime() << endl; 
	}

	void onCollide(const Collider& collide) override
	{
		cout << "Collision!" << endl;
	}

	void onCollide(const Collision& collide) override
	{
		cout << "x : " << collide.normal.x << " ";
		cout << "y : " << collide.normal.y << " ";
		cout << "penDepth : " << collide.penetration << " ";
	}
};

Entity* createPaddle(bool isLeft)
{
	// Create left paddle and add to scene
	Entity* squareEntity = new Entity();

	Component* rectComp = new RectangleRenderer(10, 45, Color(0xFF, 0xFF, 0xFF));
	squareEntity->addComponent(rectComp);

	Component* rectMover = new PaddleController(isLeft);
	squareEntity->addComponent(rectMover);

	Component* rectCollider = new BoxCollider(10, 45);
	squareEntity->addComponent(rectCollider);

	squareEntity->transform.posX = isLeft ? -640 / 2 + 20 : 640 / 2 - 20;

	return squareEntity;
}

Entity* createBall()
{
	Entity* staticEntity = new Entity();
	Component* staticComp = new RectangleRenderer(15, 15, Color(0xFF, 0xFF, 0xFF, 0xFF));
	staticEntity->addComponent(staticComp);

	Component* staticCollide = new BoxCollider(15, 15);
	staticEntity->addComponent(staticCollide);

	Component* controller = new FreeController();
	staticEntity->addComponent(controller);

	PhysicsBody* physics = new PhysicsBody();
	staticEntity->addComponent(physics);

	return staticEntity;
}

void createBounds(Scene* scene)
{
	const int WALL_THICKNESS = 50000;

	Entity* topBound = new Entity();
	Component* topCollide = new BoxCollider(640 + WALL_THICKNESS, WALL_THICKNESS);
	topBound->addComponent(topCollide);
	topBound->transform.posY = 480 / 2 + WALL_THICKNESS/2;

	Entity* bottomBound = new Entity();
	Component* bottomCollide = new BoxCollider(640 + WALL_THICKNESS, WALL_THICKNESS);
	bottomBound->addComponent(bottomCollide);
	bottomBound->transform.posY = -480 / 2 - WALL_THICKNESS/2;

	Entity* leftBound = new Entity();
	Component* leftCollide = new BoxCollider(WALL_THICKNESS, 480 + WALL_THICKNESS);
	leftBound->addComponent(leftCollide);
	leftBound->transform.posX = -640 / 2 - WALL_THICKNESS/2;

	Entity* rightBound = new Entity();
	Component* rightCollide = new BoxCollider(WALL_THICKNESS, 480 + WALL_THICKNESS);
	rightBound->addComponent(rightCollide);
	rightBound->transform.posX = 640 / 2 + WALL_THICKNESS/2;

	scene->AddEntity(topBound);
	scene->AddEntity(bottomBound);
	scene->AddEntity(rightBound);

}

int main() {
	cout << "Hello World!" << endl;
	
	// Setup game
	Game game;
	Scene* startScene = new Scene(Color(0,0,0,255));
	game.addScene(startScene);
	createBounds(startScene);

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