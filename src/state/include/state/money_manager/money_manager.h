/**
 * @file money_manager.h
 * Declares MoneyManager class which handles player currency
 */

#ifndef STATE_MONEY_MANAGER_MONEY_MANAGER_H
#define STATE_MONEY_MANAGER_MONEY_MANAGER_H

#include "state/actor/actor.h"
#include "state/actor/tower.h"
#include "state/state_export.h"
#include "state/utilities.h"
#include <cstdint>
#include <vector>

namespace state {
/**
 * MoneyManager class
 */
class STATE_EXPORT MoneyManager {
  private:
	/**
	 * Vector representing the amount of money each player has
	 */
	std::vector<int64_t> player_money;

	/**
	 * Maximum possible money for a player
	 */
	int64_t max_money;

	/**
	 * Reward amounts for a successful tower kill, indexed by tower level
	 */
	std::vector<int64_t> tower_kill_reward_amount;

	/**
	 * Reward amount for a successful soldier kill
	 */
	int64_t soldier_kill_reward_amount;

	/**
	 * Reward amounts for demolishing one's own tower, indexed by tower level
	 */
	std::vector<int64_t> tower_suicide_reward_amount;

  public:
	/**
	 * Constructor for MoneyManager class
	 */
	MoneyManager();

	MoneyManager(std::vector<int64_t> player_money, int64_t max_money,
	             std::vector<int64_t> tower_kill_reward_amount,
	             int64_t soldier_kill_reward_amount,
	             std::vector<int64_t> tower_suicide_reward_amount);

	/**
	 * Method to increase player money
	 *
	 * @param[in]  player_id  The player identifier
	 * @param[in]  amount     The amount of money to add
	 *                        Balance will cap at max_money even if increase
	 *                        amount is sufficient to exceed it
	 *
	 * @throw      std::out_of_range If the amount is not positive
	 */
	void Increase(PlayerId player_id, int64_t amount);

	/**
	 * Method to decrease player money
	 *
	 * @param[in]  player_id  The player identifier
	 * @param[in]  amount     The amount of money to deduct
	 *
	 * @throw      std::out_of_range If the amount is not positive
	 *                               player has insufficuent balance
	 */
	void Decrease(PlayerId player_id, int64_t amount);

	/**
	 * Reward the player for killing an enemy actor
	 *
	 * @param[in]  enemy_actor  Pointer to the killed enemy
	 */
	void RewardKill(Actor *enemy_actor);

	/**
	 * Reward the player for a tower suicide
	 *
	 * @param[in]  tower  Pointer to Tower that was destroyed
	 */
	void RewardSuicide(Tower *tower);

	/**
	 * Get the current balance amount of the PlayerId passed
	 *
	 * @param[in]  player_id  The player identifier
	 *
	 * @return     The balance.
	 */
	int64_t GetBalance(PlayerId player_id);

	/**
	 * Gets the maximum balance.
	 *
	 * @return     The maximum possible balance.
	 */
	int64_t GetMaxMoney();
};
}

#endif
