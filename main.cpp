
#include <SimpleECS_Core.h>;

#include <iostream>;
#include <vector>;

using namespace std;
using namespace SimpleECS;

class RectangleMover : public Component {
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
};

int main() {
	cout << "Hello World!" << endl;
	
	// Setup game
	Game game;
	Scene* startScene = new Scene();
	game.addScene(startScene);

	// Create Square and add to scene
	Entity* squareEntity = new Entity();
	Component* rectComp = new RectangleRenderer(90, 90);
	squareEntity->addComponent(rectComp);

	Component* rectMover = new RectangleMover();
	squareEntity->addComponent(rectMover);

	startScene->AddEntity(squareEntity);

	// Start game loop
	game.startGame();
}