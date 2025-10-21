#include "select_chart_info.hpp"
#include "high_score/ksc_io.hpp"
#include "ini/config_ini.hpp"
#include "kson/io/ksh_io.hpp"

namespace
{
	HighScoreInfo LoadHighScoreInfo(FilePathView chartFilePath)
	{
		const KscKey condition
		{
			.gaugeType = static_cast<GaugeType>(ConfigIni::GetInt(ConfigIni::Key::kEffRateType, static_cast<int32>(GaugeType::kNormalGauge))),
			.turnMode = static_cast<TurnMode>(ConfigIni::GetInt(ConfigIni::Key::kTurn, static_cast<int32>(TurnMode::kNormal))),
			.btPlayMode = static_cast<JudgmentPlayMode>(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeBT, ConfigIni::Value::JudgmentMode::kOn)),
			.fxPlayMode = static_cast<JudgmentPlayMode>(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeFX, ConfigIni::Value::JudgmentMode::kOn)),
			.laserPlayMode = static_cast<JudgmentPlayMode>(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeLaser, ConfigIni::Value::JudgmentMode::kOn)),
		};

		return KscIo::ReadHighScoreInfo(chartFilePath, condition);
	}
}

FilePath SelectChartInfo::toFullPath(const std::string& u8Filename) const
{
	return FileSystem::PathAppend(FileSystem::ParentPath(m_chartFilePath), Unicode::FromUTF8(u8Filename));
}

SelectChartInfo::SelectChartInfo(FilePathView chartFilePath)
	: m_chartFilePath(chartFilePath)
	, m_chartData(kson::LoadKSHMetaChartData(chartFilePath.narrow()))
	, m_highScoreInfo(LoadHighScoreInfo(chartFilePath))
{
}

String SelectChartInfo::title() const
{
	return Unicode::FromUTF8(m_chartData.meta.title);
}

String SelectChartInfo::artist() const
{
	return Unicode::FromUTF8(m_chartData.meta.artist);
}

FilePath SelectChartInfo::jacketFilePath() const
{
	return toFullPath(m_chartData.meta.jacketFilename);
}

String SelectChartInfo::jacketAuthor() const
{
	return Unicode::FromUTF8(m_chartData.meta.jacketAuthor);
}

FilePathView SelectChartInfo::chartFilePath() const
{
	return m_chartFilePath;
}

String SelectChartInfo::chartAuthor() const
{
	return Unicode::FromUTF8(m_chartData.meta.chartAuthor);
}

int32 SelectChartInfo::difficultyIdx() const
{
	return m_chartData.meta.difficulty.idx;
}

int32 SelectChartInfo::level() const
{
	return m_chartData.meta.level;
}

String SelectChartInfo::dispBPM() const
{
	return Unicode::FromUTF8(m_chartData.meta.dispBPM);
}

FilePath SelectChartInfo::previewBGMFilePath() const
{
	return toFullPath(m_chartData.audio.bgm.filename);
}

SecondsF SelectChartInfo::previewBGMOffset() const
{
	return SecondsF{ m_chartData.audio.bgm.preview.offset / 1000.0 };
}

Duration SelectChartInfo::previewBGMDuration() const
{
	return Duration{ m_chartData.audio.bgm.preview.duration / 1000.0 };
}

double SelectChartInfo::previewBGMVolume() const
{
	return m_chartData.audio.bgm.vol;
}

FilePath SelectChartInfo::iconFilePath() const
{
	return toFullPath(m_chartData.meta.iconFilename);
}

String SelectChartInfo::information() const
{
	return Unicode::FromUTF8(m_chartData.meta.information);
}

const HighScoreInfo& SelectChartInfo::highScoreInfo() const
{
	return m_highScoreInfo;
}

bool SelectChartInfo::hasError() const
{
	return m_chartData.error != kson::ErrorType::None;
}

String SelectChartInfo::errorString() const
{
	return Unicode::FromUTF8(kson::GetErrorString(m_chartData.error));
}
