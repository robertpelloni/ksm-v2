#include <catch2/catch.hpp>
#include "music_game/play_result.hpp"

using namespace MusicGame;

TEST_CASE("Grade calculation with 100% gauge", "[PlayResult][Grade]")
{
	PlayResult result;
	result.gaugePercentageForGrade = 100.0;

	// スコア10000000
	result.score = 10000000;
	REQUIRE(result.grade() == Grade::kAAA);

	// グレードAAAの境界値テスト
	result.score = 9777778;
	REQUIRE(result.grade() == Grade::kAAA);
	result.score = 9777777;
	REQUIRE(result.grade() == Grade::kAA);

	// グレードAAの境界値テスト
	result.score = 9333334;
	REQUIRE(result.grade() == Grade::kAA);
	result.score = 9333333;
	REQUIRE(result.grade() == Grade::kA);

	// グレードAの境界値テスト
	result.score = 8777778;
	REQUIRE(result.grade() == Grade::kA);
	result.score = 8777777;
	REQUIRE(result.grade() == Grade::kB);

	// グレードBの境界値テスト
	result.score = 7777778;
	REQUIRE(result.grade() == Grade::kB);
	result.score = 7777777;
	REQUIRE(result.grade() == Grade::kC);

	// グレードCの境界値テスト
	result.score = 6666667;
	REQUIRE(result.grade() == Grade::kC);
	result.score = 6666666;
	REQUIRE(result.grade() == Grade::kD);

	// スコア0
	result.score = 0;
	REQUIRE(result.grade() == Grade::kD);
}

TEST_CASE("Grade calculation with 0% gauge", "[PlayResult][Grade]")
{
	PlayResult result;
	result.gaugePercentageForGrade = 0.0;

	// スコア10000000
	result.score = 10000000;
	REQUIRE(result.grade() == Grade::kA);

	// グレードAの境界値テスト
	result.score = 9888889;
	REQUIRE(result.grade() == Grade::kA);
	result.score = 9888888;
	REQUIRE(result.grade() == Grade::kB);

	// グレードBの境界値テスト
	result.score = 8888889;
	REQUIRE(result.grade() == Grade::kB);
	result.score = 8888888;
	REQUIRE(result.grade() == Grade::kC);

	// グレードCの境界値テスト
	result.score = 7777778;
	REQUIRE(result.grade() == Grade::kC);
	result.score = 7777777;
	REQUIRE(result.grade() == Grade::kD);

	// スコア0
	result.score = 0;
	REQUIRE(result.grade() == Grade::kD);
}
