#pragma once
#include <cstdint>

namespace dae
{
	using EventId = unsigned int;

	// Compile-time SDBM hash (from the slides)
	template <int length> struct sdbm_hash
	{
		consteval static unsigned int _calculate(const char* const text, unsigned int& value)
		{
			const unsigned int character = sdbm_hash<length - 1>::_calculate(text, value);
			value = character + (value << 6) + (value << 16) - value;
			return text[length - 1];
		}
		consteval static unsigned int calculate(const char* const text)
		{
			unsigned int value = 0;
			const auto character = _calculate(text, value);
			return character + (value << 6) + (value << 16) - value;
		}
	};
	template <> struct sdbm_hash<1>
	{
		consteval static int _calculate(const char* const text, unsigned int&) { return text[0]; }
	};
	template <size_t N> consteval unsigned int make_sdbm_hash(const char(&text)[N])
	{
		return sdbm_hash<N - 1>::calculate(text);
	}

	// Pre-defined event IDs
	inline constexpr EventId EVENT_PLAYER_DIED = make_sdbm_hash("PlayerDied");
	inline constexpr EventId EVENT_PLAYER_HIT = make_sdbm_hash("PlayerHit");
	inline constexpr EventId EVENT_PLAYER_GET_POINTS = make_sdbm_hash("PlayerGetPoints");
	inline constexpr EventId EVENT_ENEMY_HIT = make_sdbm_hash("EnemyHit");
	inline constexpr EventId EVENT_ENEMY_DIED = make_sdbm_hash("EnemyDied");

	struct Event
	{
		const EventId id;
		explicit Event(EventId _id) : id{ _id } {}
	};
}