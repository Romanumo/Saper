#pragma once
#include <SDL.h>

#include "Engine/Component.h"
#include "Engine/Row.h"
#include "Engine/Column.h"

#include "Grid.h"
#include "NewGameButton.h"
#include "FlagCounter.h"

class MineSweeperUI
{
public:

#ifdef SHOW_DEBUG_HELPERS
	MineSweeperUI()
	{

	}
#endif // SHOW_DEBUG_HELPERS

	void Render(SDL_Surface* surface)
	{
		layout->Render(surface);
	}

	void HandleEvent(const SDL_Event& event)
	{
		layout->HandleEvent(event);
	}

private:
	Grid* minesweeperGrid{ new Grid{0,0} };

	NewGameButton* newGameButton{ new NewGameButton{0, 0,
	Config::GRID_WIDTH - Config::COUNTER_WIDTH,
	Config::FOOTER_HEIGHT - Config::PADDING } };

	FlagCounter* counter{ new FlagCounter{0,0,
	Config::COUNTER_WIDTH - Config::PADDING,
	Config::FOOTER_HEIGHT - Config::PADDING
	} };

	Engine::Column* layout{ new Engine::Column{
		Config::PADDING, Config::PADDING,
		std::vector<Engine::Component*>{
			minesweeperGrid, new Engine::Row{
				std::vector<Engine::Component*>{
					counter, newGameButton
				}
			}
		}
	} };
};