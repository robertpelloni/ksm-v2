#include <catch2/catch.hpp>
#include <Siv3D.hpp>
#include "kson/ChartData.hpp"
#include "kson/io/ksh_io.hpp"
#include "kson/io/kson_io.hpp"
#include "Common/FsUtils.hpp"

// We simulate what Editor::loadChart does to verify kson dual-format IO is functioning within the context of the project root.
TEST_CASE("Editor loadChart functionality", "[Editor]")
{
    // Create dummy files
    const FilePath ksonPath = U"test_dummy.kson";
    const FilePath kshPath = U"test_dummy.ksh";

    // Write minimal valid JSON for kson
    TextWriter writerKson(ksonPath);
    writerKson.write(U"{\"version\":\"0.8.0\",\"meta\":{\"title\":\"KSON Test\",\"artist\":\"Author\"}}");
    writerKson.close();

    // Write minimal valid KSH for ksh
    TextWriter writerKsh(kshPath);
    writerKsh.write(U"title=KSH Test\nartist=Author\n#define_fx testFx type=Retrigger\n#define_filter testFilter type=PeakingFilter\n--\n0000|00|--\n0000|00|--\n0000|00|--\n0000|00|--\n");
    writerKsh.close();

    SECTION("Load .kson format")
    {
        auto loaded = kson::LoadKsonChartData(ksonPath.toUTF8());
        REQUIRE(loaded.error == kson::Error::None);
        REQUIRE(loaded.chartData.meta.title == "KSON Test");
    }

    SECTION("Load .ksh format")
    {
        auto loaded = kson::LoadKshChartData(kshPath.toUTF8());
        REQUIRE(loaded.error == kson::Error::None);
        REQUIRE(loaded.chartData.meta.title == "KSH Test");

        // Verify custom fx metadata
        REQUIRE(loaded.chartData.audio.audioEffect.fx.defContains("testFx"));
        REQUIRE(loaded.chartData.audio.audioEffect.laser.defContains("testFilter"));
    }

    // Cleanup
    FileSystem::Remove(ksonPath);
    FileSystem::Remove(kshPath);
}
