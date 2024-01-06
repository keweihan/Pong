
#include <SimpleECS_Core.h>;

#include <iostream>;
#include <vector>;

using namespace std;
using namespace SimpleECS;

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
	startScene->AddEntity(squareEntity);

	// Start game loop
	game.startGame();
}