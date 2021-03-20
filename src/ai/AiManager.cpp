#include "AiManager.h"

namespace hyperbright {
	namespace ai {
		AiManager::AiManager() {}

		AiManager::~AiManager() {}

		//////////////////////////////////////////////////////////////////
		void AiManager::loadAiVehicle(std::shared_ptr<entity::Vehicle> vehicle) {
			loadedAi.push_back(std::make_shared<Ai>(Ai(vehicle, arena)));
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

		void AiManager::setArena(std::shared_ptr<entity::Arena> a) {
			arena = a;
		}

		//////////////////////////////////////////////////////////////////
		/*
		void AiManager::setArena(std::vector<std::vector<bool>> a) {
			arenaRep = a;
		}
		*/

		//////////////////////////////////////////////////////////////////
		//GENERATE PATH FOR GIVEN AI
		void AiManager::generatePath(std::shared_ptr<Ai> ai) {
			ai->targetTile = generateTarget(ai->targetTile, ai);
			//std::cout << "Goal Tile for "<< ai->vehicle->getId()<<" : " << ai->targetTile.x << " "<< ai->targetTile.y << std::endl;
			//std::vector<glm::vec2> pathList;
			//glm::vec2 currentTile = ai->vehicle->currentTile;
			//glm::vec2 target = ai->targetTile;

			ai->path.push_back(ai->targetTile);
			ai->state = HASTARGET;
			return;
		}

		//////////////////////////////////////////////////////////////////
		/*
		a b c
		d e f
		g h i
		*/
		glm::vec2 a = glm::vec2(3, 3);
		glm::vec2 b = glm::vec2(20, 3);
		glm::vec2 c = glm::vec2(36, 3);
		glm::vec2 d = glm::vec2(3, 20);
		glm::vec2 e = glm::vec2(20, 20);
		glm::vec2 f = glm::vec2(36, 20);
		glm::vec2 g = glm::vec2(3, 36);
		glm::vec2 h = glm::vec2(20, 36);
		glm::vec2 i = glm::vec2(36, 36);

		glm::vec2 j = glm::vec2(3, 10);
		glm::vec2 k = glm::vec2(20, 10);
		glm::vec2 l = glm::vec2(30, 3);
		glm::vec2 m = glm::vec2(10, 20);
		glm::vec2 n = glm::vec2(30, 20);
		glm::vec2 o = glm::vec2(10, 36);
		glm::vec2 p = glm::vec2(20, 30);
		glm::vec2 q = glm::vec2(36, 30);

		//DECIDE WHERE TO GO NEXT

		//UPDATE PAST GOAL
		glm::vec2 AiManager::generateTarget(glm::vec2 lastGoal, std::shared_ptr<Ai> ai) {
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