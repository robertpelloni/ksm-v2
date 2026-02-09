#pragma once
#include <CoTaskLib.hpp>
#include "InputGateClient.hpp"

class InputGateScene : public Co::SceneBase
{
private:
	std::shared_ptr<noco::Canvas> m_canvas;
	InputGate::InputGateClient m_client;
	Array<InputGate::SongInfo> m_songList;

	void populateSongList();

public:
	InputGateScene();

	virtual ~InputGateScene() = default;

	virtual Co::Task<void> start() override;

	virtual void update() override;

	virtual void draw() const override;
};
