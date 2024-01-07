
#include <SimpleECS_Core.h>;

#include <iostream>;
#include <vector>;

using namespace std;
using namespace SimpleECS;

class RectangleMover : public Component {
	int collisionFrames = 0;

	void update() override
	{
		if (Input::getKeyDown(KeyCode::KEY_DOWN_ARROW))
		{
			entity->transform.posY -= 1;
		}
		else if (Input::getKeyDown(KeyCode::KEY_UP_ARROW))
		{
			entity->transform.posY += 1;
		}

		if (Input::getKeyDown(KeyCode::KEY_LEFT_ARROW))
		{
			entity->transform.posX -= 1;
		}
		else if (Input::getKeyDown(KeyCode::KEY_RIGHT_ARROW))
		{
			entity->transform.posX += 1;
		}
	}

	void initialize() override
	{

	}

	void onCollide(const Collider& collide) override
	{
		cout << "Colliding! " << collisionFrames << "th frame." << endl;
		++collisionFrames;
	}
};

int main() {
	cout << "Hello World!" << endl;
	
	// Setup game
	Game game;
	Scene* startScene = new Scene();
	game.addScene(startScene);

	// Create Controllable Square and add to scene
	Entity* squareEntity = new Entity();
	startScene->AddEntity(squareEntity);

	Component* rectComp = new RectangleRenderer(90, 90);
	squareEntity->addComponent(rectComp);

	Component* rectMover = new RectangleMover();
	squareEntity->addComponent(rectMover);

	Component* rectCollider = new BoxCollider(90, 90);
	squareEntity->addComponent(rectCollider);

	// Create Static Square and add to scene
	Entity* staticEntity = new Entity();
	startScene->AddEntity(staticEntity);

	Component* staticComp = new RectangleRenderer(45, 45);
	staticEntity->addComponent(staticComp);

	Component* staticCollide = new BoxCollider(45, 45);
	staticEntity->addComponent(staticCollide);

	// Start game loop
	game.startGame();
}