#include "gmock/gmock.h"
#include "state/map/map.h"
#include "state/mocks/map_mock.h"
#include "state/tower_manager/tower_manager.h"
#include "state/utilities.h"
#include "gtest/gtest.h"
#include <cstdint>

using namespace std;
using namespace state;
using namespace physics;
using ::testing::AtLeast;

class TowerManagerTest : public testing::Test {
  protected:
	unique_ptr<TowerManager> tower_manager;
	std::vector<Tower *> towers;
	PlayerId player_id;

	unique_ptr<MoneyManager> money_manager;
	int64_t max_money;
	vector<int64_t> player_money;

	unique_ptr<IMap> map;
	vector<vector<MapElement>> grid;
	int map_size;
	int elt_size;

	TowerManagerTest() {

		this->player_id = PlayerId::PLAYER1;

		// Init Money Manager
		this->max_money = 100000;

		int64_t player_count = static_cast<int64_t>(PlayerId::PLAYER_COUNT);
		for (int i = 0; i < player_count; ++i) {
			player_money.push_back(5000);
		}

		this->money_manager =
		    make_unique<MoneyManager>(player_money, max_money);

		// Init Map
		this->map_size = 60;

		this->elt_size = 30;

		for (int i = 0; i < map_size; ++i) {
			vector<MapElement> row;

			for (int j = 0; j < map_size; ++j) {
				row.push_back(MapElement(
				    Vector(i * elt_size, j * elt_size),
				    (i < 5 && j < 5) ? TerrainType::LAND : TerrainType::WATER));
			}

			grid.push_back(row);
		}

		this->map = make_unique<Map>(grid, elt_size);

		// Init Tower Manager
		this->tower_manager = make_unique<TowerManager>(
		    towers, player_id, money_manager.get(), map.get());
	}
};

TEST_F(TowerManagerTest, ValidBuildTowerTest) {

	map->GetElementByOffset(Vector(0, 0)).SetOwnership(PlayerId::PLAYER1, true);

	tower_manager->BuildTower(Vector(0, 0));

	std::vector<Tower *> current_towers = tower_manager->GetTowers();

	// Check if tower added
	ASSERT_EQ(current_towers.size(), 1);

	// Check if the tower is at 0,0
	ASSERT_EQ(
	    current_towers[0]->GetPosition(),
	    Vector(0 + map->GetElementSize() / 2, 0 + map->GetElementSize() / 2));

	// Check if money deducted
	ASSERT_EQ(money_manager->GetBalance(PlayerId::PLAYER1),
	          5000 - TowerManager::build_costs[0]);

	// Check if tower properties are set
	ASSERT_EQ(current_towers[0]->GetHp(), TowerManager::max_hp_levels[0]);
	ASSERT_EQ(current_towers[0]->GetMaxHp(), TowerManager::max_hp_levels[0]);
	ASSERT_EQ(current_towers[0]->GetTowerLevel(), 1);

	// Check if territory around tower is set
	// Ensure enemy territory is NOT set
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			ASSERT_FALSE(
			    map->GetElementByOffset(Vector(i, j)).GetOwnership()[1]);
		}
	}

	// Check for player territory
	// Expected :
	vector<vector<int>> expect_ownership(10, vector<int>(10, 0));
	// Add 1 to the tower ranges, since the tower is at (1, 1)
	for (int i = 0; i <= TowerManager::tower_ranges[0]; ++i) {
		for (int j = 0; j <= TowerManager::tower_ranges[0]; ++j) {
			expect_ownership[i][j] = 1;
		}
	}

	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			ASSERT_EQ(map->GetElementByOffset(Vector(i, j)).GetOwnership()[0],
			          expect_ownership[i][j]);
		}
	}
}

TEST_F(TowerManagerTest, InvalidBuildTower) {
	Actor::SetActorIdIncrement(0);
	// Bad Position
	EXPECT_THROW(tower_manager->BuildTower(Vector(0, map->GetSize())),
	             std::out_of_range);
	EXPECT_THROW(tower_manager->BuildTower(Vector(map->GetSize(), 0)),
	             std::out_of_range);

	EXPECT_THROW(tower_manager->BuildTower(Vector(5000, 1)), std::out_of_range);
	EXPECT_THROW(tower_manager->BuildTower(Vector(-1, 1)), std::out_of_range);
	EXPECT_THROW(tower_manager->BuildTower(Vector(0, -20)), std::out_of_range);

	// Bad Territory
	EXPECT_THROW(tower_manager->BuildTower(Vector(0, 0)), std::out_of_range);

	// Bad Terrain
	EXPECT_THROW(tower_manager->BuildTower(Vector(10, 10)), std::out_of_range);

	map->GetElementByOffset(Vector(2, 2)).SetOwnership(PlayerId::PLAYER1, true);
	map->GetElementByOffset(Vector(2, 2)).SetOwnership(PlayerId::PLAYER2, true);
	EXPECT_THROW(tower_manager->BuildTower(Vector(2, 2)), std::out_of_range);

	// Tower already exists
	map->GetElementByOffset(Vector(3, 3)).SetOwnership(PlayerId::PLAYER1, true);
	tower_manager->BuildTower(Vector(3, 3)); // Valid
	EXPECT_THROW(tower_manager->BuildTower(Vector(3, 3)), std::out_of_range);

	// Not enough money
	// Leave one less than the build cost remaining
	int64_t amount_to_decrease = money_manager->GetBalance(PlayerId::PLAYER1) -
	                             TowerManager::build_costs[0] + 1;
	money_manager->Decrease(PlayerId::PLAYER1, amount_to_decrease);
	EXPECT_THROW(tower_manager->BuildTower(Vector(1, 2)), std::out_of_range);
}

TEST_F(TowerManagerTest, ValidUpgradeTower) {
	Actor::SetActorIdIncrement(0);
	// Build a Valid Tower
	map->GetElementByOffset(Vector(1, 1)).SetOwnership(PlayerId::PLAYER1, true);
	tower_manager->BuildTower(Vector(1, 1));

	// Upgrade Tower
	tower_manager->UpgradeTower(0);

	vector<vector<int>> expect_ownership(10, vector<int>(10, 0));
	// Add 1 to the tower ranges, since the tower is at (1, 1)
	for (int i = 0; i <= TowerManager::tower_ranges[1] + 1; ++i) {
		for (int j = 0; j <= TowerManager::tower_ranges[1] + 1; ++j) {
			expect_ownership[i][j] = 1;
		}
	}

	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			ASSERT_EQ(map->GetElementByOffset(Vector(i, j)).GetOwnership()[0],
			          expect_ownership[i][j]);
		}
	}

	ASSERT_EQ(money_manager->GetBalance(PlayerId::PLAYER1),
	          5000 - TowerManager::build_costs[0] -
	              TowerManager::build_costs[1]);

	ASSERT_EQ(tower_manager->GetTowers()[0]->GetTowerLevel(), 2);
	tower_manager->UpgradeTower(0);
	ASSERT_EQ(tower_manager->GetTowers()[0]->GetTowerLevel(), 3);
}

TEST_F(TowerManagerTest, InvalidUpgradeTower) {
	Actor::SetActorIdIncrement(0);
	// Build a Valid Tower
	map->GetElementByOffset(Vector(1, 1)).SetOwnership(PlayerId::PLAYER1, true);
	tower_manager->BuildTower(Vector(1, 1));

	// Upgrade an Invalid Tower
	EXPECT_THROW(tower_manager->UpgradeTower(20), std::out_of_range);

	// Valid Upgrades
	for (int i = 0; i < TowerManager::build_costs.size() - 1; ++i) {
		tower_manager->UpgradeTower(0);
	}

	// Invalid upgrade, limit reached
	EXPECT_THROW(tower_manager->UpgradeTower(0), std::out_of_range);
}

TEST_F(TowerManagerTest, RazeTower) {
	Actor::SetActorIdIncrement(0);
	// Build a Valid Tower
	map->GetElementByOffset(Vector(1, 1)).SetOwnership(PlayerId::PLAYER1, true);
	tower_manager->BuildTower(Vector(1, 1));

	// Attack the tower with 5 damage
	tower_manager->RazeTower(0, 5);
	ASSERT_EQ(tower_manager->GetTowerById(0)->GetHp(),
	          TowerManager::max_hp_levels[0] - 5);

	// Deal Fatal Blow
	tower_manager->RazeTower(0, 200);
	ASSERT_EQ(tower_manager->GetTowerById(0)->GetHp(), 0);

	// Test Base towers
	map->GetElementByOffset(Vector(2, 2)).SetOwnership(PlayerId::PLAYER1, true);
	tower_manager->BuildTower(Vector(2, 2), true);

	tower_manager->RazeTower(1, 1000);
	ASSERT_EQ(tower_manager->GetTowerById(1)->GetHp(),
	          TowerManager::max_hp_levels[0]);
}

TEST_F(TowerManagerTest, Update) {
	Actor::SetActorIdIncrement(0);
	// Build a Valid Tower
	map->GetElementByOffset(Vector(1, 1)).SetOwnership(PlayerId::PLAYER1, true);
	tower_manager->BuildTower(Vector(1, 1));

	// Deal Fatal Blow
	tower_manager->RazeTower(0, 200);
	ASSERT_EQ(tower_manager->GetTowerById(0)->GetHp(), 0);

	// Clean Dead Towers
	tower_manager->Update();
	ASSERT_EQ(tower_manager->GetTowers().size(), 0);
	ASSERT_THROW(tower_manager->GetTowerById(0), std::out_of_range);
}
