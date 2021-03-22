#include "AiManager.h"

namespace hyperbright {
	namespace ai {
		AiManager::AiManager() {}

		AiManager::~AiManager() {}

		//////////////////////////////////////////////////////////////////
		void AiManager::loadAiVehicle(std::shared_ptr<entity::Vehicle> vehicle) {
			if (currentArena == 1) {
				loadedAi.push_back(std::make_shared<Ai>(Ai(vehicle, arena, e)));
			}
			else if (currentArena == 2) {
				loadedAi.push_back(std::make_shared<Ai>(Ai(vehicle, arena, e)));
			}
		}

		//////////////////////////////////////////////////////////////////

		void AiManager::makeMoves() {
			for (int i = 0; i < loadedAi.size(); i++) {
				if (loadedAi.at(i)->state) {
					loadedAi.at(i)->aiInput();
				}
				else {
					if (loadedAi.at(i)->vehicle->currentTile.x != -1) {
						generatePath(loadedAi.at(i));
						//std::cout<<loadedAi.at(i)->path.size()<<std::endl;
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////

		void AiManager::setArena(std::shared_ptr<entity::Arena> ar, int arenaSelection) {
			arena = ar;
			currentArena = arenaSelection;

			if (arenaSelection == 1) {
				/*
				a b c
				d e f
				g h i
				*/
				 a = glm::vec2(3, 3);
				 b = glm::vec2(20, 3);
				 c = glm::vec2(36, 3);
				 d = glm::vec2(3, 20);
				 e = glm::vec2(20, 20);
				 f = glm::vec2(36, 20);
				 g = glm::vec2(3, 36);
				 h = glm::vec2(20, 36);
				 i = glm::vec2(36, 36);

				 j = glm::vec2(3, 10);
				 k = glm::vec2(20, 10);
				 l = glm::vec2(30, 3);
				 m = glm::vec2(10, 20);
				 n = glm::vec2(30, 20);
				 o = glm::vec2(10, 36);
				 p = glm::vec2(20, 30);
				 q = glm::vec2(36, 30);
			}
			else if (arenaSelection == 2) {
				 a = glm::vec2(2, 2);//PU
				 b = glm::vec2(17, 2);//PU
				 c = glm::vec2(2, 17);//PU
				 d = glm::vec2(17, 17);//PU

				 e = glm::vec2(10, 9);//CHARGE

				 f = glm::vec2(9, 3);
				 g = glm::vec2(9, 16);
				 h = glm::vec2(5, 5);
				 i = glm::vec2(14, 5);
				 j = glm::vec2(2, 10);
				 k = glm::vec2(17, 10);
				 l = glm::vec2(4, 14);
				 m = glm::vec2(15, 14);
			}
		}

		/////////////////////////////////////////s/////////////////////////
		//GENERATE PATH FOR GIVEN AI
		void AiManager::generatePath(std::shared_ptr<Ai> ai) {

			if (currentArena==1) {
				ai->targetTile = generateTargetOne(ai->targetTile, ai);
			}
			else if (currentArena==2) {
				ai->targetTile = generateTargetTwo(ai->targetTile, ai);
			}
			//std::cout << "Goal Tile for "<< ai->vehicle->getId()<<" : " << ai->targetTile.x << " "<< ai->targetTile.y << std::endl;
			//std::vector<glm::vec2> pathList;
			//glm::vec2 currentTile = ai->vehicle->currentTile;
			//glm::vec2 target = ai->targetTile;

			ai->path.push_back(ai->targetTile);
			ai->state = HASTARGET;
			return;
		}

		//////////////////////////////////////////////////////////////////

		glm::vec2 AiManager::generateTargetTwo(glm::vec2 lastGoal, std::shared_ptr<Ai> ai) {
			//int selector;
			if (lastGoal == e) {
				ai->pastGoal = e;
				if (rand() % 2 == 1) {
					return f;
				}
				else {
					return g;
				}
			} else if(lastGoal == f){//NORTH OF CENTER
				ai->pastGoal = f;
				if (ai->vehicle->energy <= 0) {//EMERGY CHECK
					return e;
				}

				if (rand() % 2 == 1) {
					return h;
				}
				else {
					return i;
				}
			}
			else if (lastGoal == g) {//SOUTH OF CENTER
				ai->pastGoal = g;
				if (ai->vehicle->energy <= 0) {//ENERGY CHECK
					return e;
				}

				if (rand() % 2 == 1) {
					return l;
				}
				else {
					return m;
				}
			}
			else if (lastGoal == c) {//PU
				ai->pastGoal = c;
				return l;
			}
			else if (lastGoal == a) {//PU
				ai->pastGoal = a;
				return h;
			}
			else if (lastGoal == b) {//PU
				ai->pastGoal = i;
				return i;
			}
			else if (lastGoal == d) {//PU
				ai->pastGoal = d;
				return m;
			}
			else if (lastGoal == h) {//CORNER NW
				//ai->pastGoal = h;
				if (!ai->vehicle->hasPickup() && ai->pastGoal != a) {//PU CHECK
					ai->pastGoal = h;
					return a;
				}
				ai->pastGoal = h;
				if (rand() % 2 == 1) {
					return f;
				}
				else {
					return j;
				}
			}
			else if (lastGoal==j) {//LEFT SIDE
				if (ai->pastGoal!=l) {
					ai->pastGoal = j;
					return l;
				}
				else {
					ai->pastGoal = j;
					return h;
				}
			}
			else if (lastGoal == k) {//RIGHT SIDE
				if (ai->pastGoal != i) {
					ai->pastGoal = k;
					return i;
				}
				else {
					ai->pastGoal = k;
					return m;
				}
			}
			else if (lastGoal == i) {//CORNER NE
				//ai->pastGoal = i;
				if (!ai->vehicle->hasPickup() && ai->pastGoal != b) {//PU CHECK
					ai->pastGoal = i;
					return b;
				}
				ai->pastGoal = i;
				if (rand() % 2 == 1) {
					return f;
				}
				else {
					return k;
				}
			}
			else if (lastGoal == l) {//CORNER SW
				//ai->pastGoal = l;
				if (!ai->vehicle->hasPickup() && ai->pastGoal != c) {//PU CHECK
					ai->pastGoal = l;
					return c;
				}
				ai->pastGoal = l;
				if (rand() % 2 == 1) {
					return g;
				}
				else {
					return j;
				}
			}
			else if (lastGoal == m) {//CORNER SE
			//ai->pastGoal = m;
			if (!ai->vehicle->hasPickup() && ai->pastGoal != d) {//PU CHECK
				ai->pastGoal = m;
				return d;
			}
			ai->pastGoal = m;
			if (rand() % 2 == 1) {
				return g;
			}
			else {
				return k;
			}
			}
		}

		//////////////////////////////////////////////////////////////////
		

		//DECIDE WHERE TO GO NEXT

		//UPDATE PAST GOAL
		glm::vec2 AiManager::generateTargetOne(glm::vec2 lastGoal, std::shared_ptr<Ai> ai) {
			int selector;	
			
			if (lastGoal == a) {
				
				if (ai->pastGoal==j) {
					ai->pastGoal = lastGoal;
					return b;
				}
				else {
					ai->pastGoal = lastGoal;
					return j;
				}
			}
			else if (lastGoal == b) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != a) {
						ai->pastGoal = lastGoal;
						return a;
					}
					else if (selector == 1 && ai->pastGoal != l) {
						ai->pastGoal = lastGoal;
						return l;
					}
					else if ( ai->pastGoal != k){
						ai->pastGoal = lastGoal;
						return k;
					}
				}
			}
			else if (lastGoal == c) {
				if (ai->pastGoal == f) {
					ai->pastGoal = lastGoal;
					return l;
				}
				else {
					ai->pastGoal = lastGoal;
					return f;
				}
			}
			else if (lastGoal == d) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != m) {
						ai->pastGoal = lastGoal;
						return m;
					}
					else if (selector == 1 && ai->pastGoal != j) {
						ai->pastGoal = lastGoal;
						return j;
					}
					else if (ai->pastGoal != g) {
						ai->pastGoal = lastGoal;
						return g;
					}
				}
			}
			else if (lastGoal == e) {
				selector = rand() % 4;
				if (selector == 0) {
					ai->pastGoal = lastGoal;
					return k;
				}
				else if (selector == 1) {
					ai->pastGoal = lastGoal;
					return m;
				}
				else if (selector == 2) {
					ai->pastGoal = lastGoal;
					return p;
				}
				else {
					ai->pastGoal = lastGoal;
					return n;
				}
			}
			else if (lastGoal == f) {
				while(true){
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != c) {
						ai->pastGoal = lastGoal;
						return c;
					}
					else if (selector == 1 && ai->pastGoal != n) {
						ai->pastGoal = lastGoal;
						return n;
					}
					else if (ai->pastGoal != q) {
						ai->pastGoal = lastGoal;
						return q;
					}
				}
			}
			else if (lastGoal == g) {
				if (ai->pastGoal == o) {
					ai->pastGoal = lastGoal;
					return d;
				}
				else {
					ai->pastGoal = lastGoal;
					return o;
				}
			}
			else if (lastGoal == h) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != p) {
						ai->pastGoal = lastGoal;
						return p;
					}
					else if (selector == 1 && ai->pastGoal != o) {
						ai->pastGoal = lastGoal;
						return o;
					}
					else if (ai->pastGoal != i) {
						ai->pastGoal = lastGoal;
						return i;
					}
				}
			}
			else if (lastGoal == i) {
				if (ai->pastGoal == q) {
					ai->pastGoal = lastGoal;
					return h;
				}
				else {
					ai->pastGoal = lastGoal;
					return q;
				}
			}
			else if (lastGoal == j) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != a) {
						ai->pastGoal = lastGoal;
						return a;
					}
					else if (selector == 1 && ai->pastGoal != k) {
						ai->pastGoal = lastGoal;
						return k;
					}
					else if (ai->pastGoal != d) {
						ai->pastGoal = lastGoal;
						return d;
					}
				}
			}
			else if (lastGoal == k) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != j) {
						ai->pastGoal = lastGoal;
						return j;
					}
					else if (selector == 1 && ai->pastGoal != b) {
						ai->pastGoal = lastGoal;
						return b;
					}
					else if(ai->pastGoal != e) {
						ai->pastGoal = lastGoal;
						return e;
					}
				}
			}
			else if (lastGoal == l) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != b) {
						ai->pastGoal = lastGoal;
						return b;
					}
					else if (selector == 1 && ai->pastGoal != n) {
						ai->pastGoal = lastGoal;
						return n;
					}
					else if (ai->pastGoal != c) {
						ai->pastGoal = lastGoal;
						return c;
					}
				}
			}
			else if (lastGoal == m) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != d) {
						ai->pastGoal = lastGoal;
						return d;
					}
					else if (selector == 1 && ai->pastGoal != o) {
						ai->pastGoal = lastGoal;
						return o;
					}
					else if (ai->pastGoal != e) {
						ai->pastGoal = lastGoal;
						return e;
					}
				}
			}
			else if (lastGoal == n) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != e) {
						ai->pastGoal = lastGoal;
						return e;
					}
					else if (selector == 1 && ai->pastGoal != l) {
						ai->pastGoal = lastGoal;
						return l;
					}
					else if (ai->pastGoal != f) {
						ai->pastGoal = lastGoal;
						return f;
					}
				}
			}
			else if (lastGoal == o) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != g) {
						ai->pastGoal = lastGoal;
						return g;
					}
					else if (selector == 1 && ai->pastGoal != m) {
						ai->pastGoal = lastGoal;
						return m;
					}
					else if (ai->pastGoal != h) {
						ai->pastGoal = lastGoal;
						return h;
					}
				}
			}
			else if (lastGoal == p) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != e) {
						ai->pastGoal = lastGoal;
						return e;
					}
					else if (selector == 1 && ai->pastGoal != h) {
						ai->pastGoal = lastGoal;
						return h;
					}
					else if (ai->pastGoal != q) {
						ai->pastGoal = lastGoal;
						return q;
					}
				}
			}
			else if (lastGoal == q) {
				while (true) {
					selector = rand() % 3;
					if (selector == 0 && ai->pastGoal != p) {
						ai->pastGoal = lastGoal;
						return p;
					}
					else if (selector == 1 && ai->pastGoal != f) {
						ai->pastGoal = lastGoal;
						return f;
					}
					else if (ai->pastGoal != i) {
						ai->pastGoal = lastGoal;
						return i;
					}
				}
			}
		}
	}	// namespace ai
}	// namespace hyperbright