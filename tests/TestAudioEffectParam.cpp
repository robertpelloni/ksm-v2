#include <catch2/catch.hpp>
#include "ksmaudio/audio_effect/audio_effect_param.hpp"
#include <cmath>

using namespace ksmaudio::AudioEffect;

TEST_CASE("Parse length type parameters", "[AudioEffect][Param][Length]")
{
	// 分数形式(テンポ同期)
	REQUIRE(StrToValue(Type::kLength, "1/4") == 0.25f);
	REQUIRE(StrToValue(Type::kLength, "1/2") == 0.5f);
	REQUIRE(StrToValue(Type::kLength, "1/1") == 1.0f);

	// 小数形式(テンポ同期)
	REQUIRE(StrToValue(Type::kLength, "2.0") == 2.0f);
	REQUIRE(StrToValue(Type::kLength, "0.25") == 0.25f);
	REQUIRE(StrToValue(Type::kLength, "0") == 0.0f);

	// ミリ秒形式(非テンポ同期、区別のため負の値として扱う)
	REQUIRE(StrToValue(Type::kLength, "100ms") == -0.1f);
	REQUIRE(StrToValue(Type::kLength, "10.5ms") == Approx(-0.0105f));

	// 秒形式(非テンポ同期、区別のため負の値として扱う)
	REQUIRE(StrToValue(Type::kLength, "1s") == -1.0f);
	REQUIRE(StrToValue(Type::kLength, "0.1s") == Approx(-0.1f));

	// 範囲外の値は丸める
	REQUIRE(StrToValue(Type::kLength, "-1.0") == 0.0f);
}

TEST_CASE("Parse sample type parameters", "[AudioEffect][Param][Sample]")
{
	// samples形式
	REQUIRE(StrToValue(Type::kSample, "40samples") == 40.0f);
	REQUIRE(StrToValue(Type::kSample, "0samples") == 0.0f);
	REQUIRE(StrToValue(Type::kSample, "44100samples") == 44100.0f);

	// 範囲外の値は丸める
	REQUIRE(StrToValue(Type::kSample, "50000samples") == 44100.0f);
	REQUIRE(StrToValue(Type::kSample, "-100samples") == 0.0f);
}

TEST_CASE("Parse switch type parameters", "[AudioEffect][Param][Switch]")
{
	// on/off
	REQUIRE(StrToValue(Type::kSwitch, "on") == 1.0f);
	REQUIRE(StrToValue(Type::kSwitch, "off") == 0.0f);
}

TEST_CASE("Parse rate type parameters", "[AudioEffect][Param][Rate]")
{
	// 分数形式
	REQUIRE(StrToValue(Type::kRate, "1/2") == 0.5f);
	REQUIRE(StrToValue(Type::kRate, "1/4") == 0.25f);

	// パーセント形式
	REQUIRE(StrToValue(Type::kRate, "50%") == 0.5f);
	REQUIRE(StrToValue(Type::kRate, "0%") == 0.0f);
	REQUIRE(StrToValue(Type::kRate, "100%") == 1.0f);

	// 小数形式
	REQUIRE(StrToValue(Type::kRate, "0.5") == 0.5f);
	REQUIRE(StrToValue(Type::kRate, "0.0") == 0.0f);
	REQUIRE(StrToValue(Type::kRate, "1.0") == 1.0f);

	// 範囲外の値は丸める
	REQUIRE(StrToValue(Type::kRate, "150%") == 1.0f);
	REQUIRE(StrToValue(Type::kRate, "-10%") == 0.0f);
	REQUIRE(StrToValue(Type::kRate, "1.5") == 1.0f);
	REQUIRE(StrToValue(Type::kRate, "-0.5") == 0.0f);
}

TEST_CASE("Parse freq type parameters", "[AudioEffect][Param][Freq]")
{
	// Hz形式
	REQUIRE(StrToValue(Type::kFreq, "1500Hz") == 1500.0f);
	REQUIRE(StrToValue(Type::kFreq, "10Hz") == 10.0f);
	REQUIRE(StrToValue(Type::kFreq, "20000Hz") == 20000.0f);

	// kHz形式
	REQUIRE(StrToValue(Type::kFreq, "1.5kHz") == 1500.0f);
	REQUIRE(StrToValue(Type::kFreq, "0.01kHz") == 10.0f);
	REQUIRE(StrToValue(Type::kFreq, "20.0kHz") == 20000.0f);

	// 範囲外の値は丸める
	REQUIRE(StrToValue(Type::kFreq, "5Hz") == 10.0f);
	REQUIRE(StrToValue(Type::kFreq, "30000Hz") == 20000.0f);
}

TEST_CASE("Parse pitch type parameters", "[AudioEffect][Param][Pitch]")
{
	// 浮動小数点形式(量子化なし、正の値で返される)
	REQUIRE(StrToValue(Type::kPitch, "12.0") == Approx(60.0f)); // 12.0 + 48.0
	REQUIRE(StrToValue(Type::kPitch, "-6.5") == Approx(41.5f)); // -6.5 + 48.0
	REQUIRE(StrToValue(Type::kPitch, "0.0") == Approx(48.0f)); // 0.0 + 48.0

	// 整数形式(量子化あり、負の値で返される)
	REQUIRE(StrToValue(Type::kPitch, "12") == Approx(-60.0f)); // -(12 + 48.0)
	REQUIRE(StrToValue(Type::kPitch, "-6") == Approx(-42.0f)); // -(-6 + 48.0)
	REQUIRE(StrToValue(Type::kPitch, "0") == Approx(-48.0f)); // -(0 + 48.0)

	// 境界値
	REQUIRE(StrToValue(Type::kPitch, "48.0") == Approx(96.0f)); // 48.0 + 48.0
	REQUIRE(StrToValue(Type::kPitch, "-48.0") == Approx(0.0f)); // -48.0 + 48.0

	// 範囲外の値は0にする
	REQUIRE(StrToValue(Type::kPitch, "50.0") == 0.0f);
	REQUIRE(StrToValue(Type::kPitch, "-50.0") == 0.0f);
}

TEST_CASE("Parse int type parameters", "[AudioEffect][Param][Int]")
{
	// 整数値
	REQUIRE(StrToValue(Type::kInt, "10") == 10.0f);
	REQUIRE(StrToValue(Type::kInt, "-5") == -5.0f);
	REQUIRE(StrToValue(Type::kInt, "0") == 0.0f);
}

TEST_CASE("Parse float type parameters", "[AudioEffect][Param][Float]")
{
	// 浮動小数点値
	REQUIRE(StrToValue(Type::kFloat, "2.5") == 2.5f);
	REQUIRE(StrToValue(Type::kFloat, "-10") == -10.0f);
	REQUIRE(StrToValue(Type::kFloat, "0.707") == Approx(0.707f));
}

TEST_CASE("Parse dB type parameters", "[AudioEffect][Param][DB]")
{
	// dB形式
	REQUIRE(StrToValue(Type::kDB, "-8.0dB") == -8.0f);
	REQUIRE(StrToValue(Type::kDB, "0dB") == 0.0f);
	REQUIRE(StrToValue(Type::kDB, "3.5dB") == 3.5f);

	// dBサフィックスがない場合は0.0fを返す
	REQUIRE(StrToValue(Type::kDB, "-8.0") == 0.0f);
	REQUIRE(StrToValue(Type::kDB, "3.5") == 0.0f);
}

TEST_CASE("Parse unspecified type parameters", "[AudioEffect][Param][Unspecified]")
{
	// kUnspecifiedは常に0.0fを返す
	REQUIRE(StrToValue(Type::kUnspecified, "any value") == 0.0f);
	REQUIRE(StrToValue(Type::kUnspecified, "123") == 0.0f);
}
