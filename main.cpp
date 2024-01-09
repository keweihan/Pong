
#include <SimpleECS_Core.h>;

#include <iostream>;
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
	
	void initialize() override
	{
		phys = entity->getComponent<PhysicsBody>();
	}

	void update() override
	{
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

	Component* rectComp = new RectangleRenderer(20, 90);
	squareEntity->addComponent(rectComp);

	Component* rectMover = new PaddleController(isLeft);
	squareEntity->addComponent(rectMover);

	Component* rectCollider = new BoxCollider(20, 90);
	squareEntity->addComponent(rectCollider);

	squareEntity->transform.posX = isLeft ? -640 / 2 : 640 / 2;

	return squareEntity;
}

Entity* createBall()
{
	Entity* staticEntity = new Entity();
	Component* staticComp = new RectangleRenderer(45, 45, Color(0xFF, 0, 0, 0xFF));
	staticEntity->addComponent(staticComp);

	Component* staticCollide = new BoxCollider(45, 45);
	staticEntity->addComponent(staticCollide);

	Component* controller = new FreeController();
	staticEntity->addComponent(controller);

	PhysicsBody* physics = new PhysicsBody();
	staticEntity->addComponent(physics);

	return staticEntity;
}

int main() {
	cout << "Hello World!" << endl;
	
	// Setup game
	Game game;
	Scene* startScene = new Scene();
	game.addScene(startScene);

	// Create paddles
	startScene->AddEntity(createPaddle(false));
	startScene->AddEntity(createPaddle(true));

	// Create moveable Square and add to scene
	startScene->AddEntity(createBall());

	// Start game loop
	game.startGame();
}