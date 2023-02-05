/**
 * @file Mark.h
 * This module defines the different types of marks, that is:
 * - `Mark`: normal marks with no effects, base class of all other marks
 * - `HealingMark`: mark which heals the player when stepped on
 * - `TeleporterMark`: mark which teleports the player to another location when stepped on
 */

#ifndef LAB_MARK_H
#define LAB_MARK_H

#include "Cell.h"
#include "Environnement.h"

/**
 * @brief Possible mark types
 */
enum class MarkType {
	normal,
	heal,
	teleporter
};

/**
 * @brief Struct representing a mark with no effects.
 */
struct Mark : public Box {
public:
	// mark default textures
	inline static int _empty_texture;
	inline static int _healing_texture;
	inline static int _mark_texture;

	/**
	 * @brief default constructor, initalizes a mark with its position and an empty texture.
	 */
	Mark(int x, int y) : Mark(x, y, _empty_texture) {}

	/**
	 * @brief Creates a mark with its a position and a given texture.
	 */
	Mark(int x, int y, int texture_id) : Box{x, y, texture_id} {}

	virtual ~Mark() {}

	/**
	 * @brief Returns the type of this mark.
	 */
	virtual MarkType type() const { return MarkType::normal; }
};

/**
 * @brief Struct for marks which heal for a certain amount when stepped on.
 */
struct HealingMark : public Mark {
	int _heal_amount = 20; // the ammount of hp given by this mark

	/**
	 * @brief Creates a healing mark, with its position and the amount of hps it will heal for.
	 * This mark will have a default healing texture.
	 */
	HealingMark(int x, int y, int amount) : HealingMark(x, y, _healing_texture, amount) {}

	/**
	 * @brief Creates a healing mark, with its position, a given texture, and the amount of hp it heals for.
	 */
	HealingMark(int x, int y, int texture_id, int amount) : Mark(x, y, texture_id), _heal_amount(amount) {}

	/**
	 * @brief Returns the type of this mark
	 * @note It will always return `MarkType::heal` for a HealingMark object
	 */
	MarkType type() const override { return MarkType::heal; }
};

/**
 * @brief Class for marks which teleport the player to another mark.
 */
struct TeleporterMark : public Mark {
	TeleporterMark* _target = nullptr; // this mark's teleport target

	/**
	 * @brief Creates a teleporter mark, with its position and it's target
	 * @param target A pointer to another TeleporterMark, nullptr if this teleporter should not have a target
	 */
	TeleporterMark(int x, int y, TeleporterMark* target) : TeleporterMark(x, y, _mark_texture, target) {}

	/**
	 * @brief Creates a teleporter mark, with its position, a given texture and it's target
	 * @param target A pointer to another TeleporterMark, nullptr if this teleporter should not have a target
	 */
	TeleporterMark(int x, int y, int texture_id, TeleporterMark* target) : Mark(x, y, texture_id), _target(target) {}

	/**
	 * @brief Returns the type of this mark
	 * @note It will always return `MarkType::teleporter` for a TeleporterMark object
	 */
	MarkType type() const override { return MarkType::teleporter; }
};

#endif
