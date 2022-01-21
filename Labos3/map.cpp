
#include "map.h"


void GameMap::setStartMap() {
	for (int i = 0; i < mapHeight; i++) 
		for (int j = 0; j < mapWidth; j++) {
			map[i][j] = startMap[34 - j][i];
			if (map[i][j] == 4) {
				decisionTiles.push_back(make_pair(i-1, j));
				map[i][j] = 2;
			}
			if (map[i][j] == 5) {
				decisionTiles.push_back(make_pair(i-1, j));
				map[i][j] = 0;
			}
		}
	
}

void GameMap::displayMap() {
	for (int i = 0; i < mapHeight; i++) {
		for (int j = 0; j < mapWidth; j++)
			std::cout << map[i][j] << " ";
		cout << endl;
	}
}
	
