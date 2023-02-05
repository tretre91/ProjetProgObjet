/**
 * @file HUD.h
 * Defines a HUD class which allows to put temporary messages on top of the screen,
 * using the `message` function from Environnement.h.
 * It also takes care of displaying and syncing the state of the player (nb of hp)
 * and of the currently focused guard.
 */

#ifndef LAB_HUD_H
#define LAB_HUD_H

#include "Util.h"
#include <chrono>
#include <list>
#include <map>
#include <string>

class Labyrinthe;
class Character;

/**
 * @brief Class used to manage the messages displayed on the screen.
 * It keeps a list of (maybe temporary) messages which are then displayed as a "|" separated list
 * on the screen, alongside status information like the player's hp.
 */
class HUD
{
private:
	/**
	 * @brief Helper struct containing variables frequently drawn on the HUD.
	 * When update is called, the corresponding program variables are compared to the cache,
	 * and the static messages are only updated if there is a mismatch between the two.
	 */
	struct Cache {
		// player variables
		int player_health = 0;
		int player_max_health = 0;
		// focused guard variables
		Character* focused_guard = nullptr;
		int focused_guard_health = -1;
		int focused_guard_max_health = -1;
	};

	using time_point = Util::time_point;

	static Labyrinthe* _labyrinth;
	static std::string _data;
	// additional messages
	static std::list<std::string> _messages;
	static std::multimap<time_point, std::list<std::string>::iterator> _messages_deadlines;
	// currently focused guard
	static Character* _focused_guard;
	static time_point _focused_guard_timeout;
	// cache things
	static Cache _cache;
	static bool _redraw;

	/**
	 * @brief Update the cached variables.
	 * This functions sets the `_redraw` variable to true if any of the cache variable has changed.
	 */
	static void refresh_cache();

public:
	using milliseconds = Util::milliseconds;

	/**
	 * @brief Initializes the hud.
	 * @param l The labyrinth object
	 */
	static void init(Labyrinthe* l);

	/**
	 * @brief Updates the hud.
	 * @note Expired temporary messages are deleted when this function is called, thus it should be called
	 * frequently.
	 */
	static void update();

	/**
	 * @brief Adds a message to the hud.
	 * @param msg The message to add
	 * @param timeout The delay until the message is removed from the hud, pass 0 (`milliseconds::zero()`)
	 *                for the message to stay indefinitely. Defaults to zero.
	 */
	static void add_message(const std::string& msg, milliseconds timeout = milliseconds::zero());

	/**
	 * @brief Adds a status message about a guard.
	 * The message displays the guard's health status, it stays on for 4 seconds.
	 * @param guard The guard whose info will be displayed
	 */
	static void focus_guard(Character* guard);

	/**
	 * @brief Clears all added messages.
	 */
	static void clear();

	/**
	 * @brief Forces the hud to be refreshed.
	 * The hud will be re-drawn at the next update
	 */
	static void refresh() { _redraw = true; };
};

#endif
