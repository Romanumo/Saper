#pragma once
#include <vector>

#include "Engine/Globals.h"
#include "Engine/Random.h"
#include "Engine/Component.h"
#include "Cell.h"
#include "Grid.h"

#pragma region Public

Grid::Grid(int x, int y) : Component(x, y, 0, 0)
{
	using namespace Config;
	ReserveChildrenSize(GRID_COLUMNS * GRID_ROWS);

	constexpr int spacing = CELL_SIZE + PADDING;
	SetRelSize(GRID_COLUMNS * spacing - PADDING,
		GRID_ROWS * spacing - PADDING);

	for (int row = 0; row < GRID_ROWS;row++)
	{
		for (int col = 0; col < GRID_COLUMNS;col++)
		{
			Cell* cell = new Cell(
				col * spacing,
				row * spacing,
				CELL_SIZE, CELL_SIZE, row, col
			);

			cell->SetAsChildOf(this);
		}
	}
}

void Grid::Render(SDL_Surface* surface)
{
	for (Component* child : GetChildren())
	{
		child->Render(surface);
	}
}

void Grid::HandleEvent(const SDL_Event& event)
{
	if (event.type == UserEvents::CELL_CLEARED)
	{
		auto* cell = static_cast<Cell*>(event.user.data1);
		if (cellsToClear == 0) PlaceBombs(*cell);

		HandleCellCleared(*cell);
	}
	if (event.type == UserEvents::NEW_GAME)
	{
		for (int i = 0;i < GetChildren().size();i++)
		{
			Cell* cell = GetChildToCell(i);
			cell->Reset();
		}

		RefreshGrid();
	}

	for (Component* child : GetChildren())
	{
		child->HandleEvent(event);
	}
}

#pragma endregion

#pragma region HandleGrid

void Grid::RefreshGrid()
{
	cellsToClear = 0;
}

void Grid::PlaceBombs(Cell& openedCell)
{
	int bombsToPlace = Config::BOMB_COUNT;
	cellsToClear =
		Config::GRID_COLUMNS * Config::GRID_ROWS - Config::BOMB_COUNT;

	std::vector<Cell*> safeCells = GetNeigborCells(openedCell);
	safeCells.push_back(&openedCell);

	while (bombsToPlace > 0)
	{
		const size_t randomIndex =
			Engine::Random::Int(0, GetChildren().size() - 1);

		Cell* bombCell = GetChildToCell(randomIndex);

		if (IsCellInsideArea(safeCells, bombCell)) continue;

		if (bombCell->PlaceBomb())
		{
			--bombsToPlace;
			SetBombHints(*bombCell);
		}
	}
}

void Grid::SetBombHints(const Cell& bombCell)
{
	std::vector<Cell*> bombNeighbors = GetNeigborCells(bombCell);
	for (Cell* cell : bombNeighbors)
	{
		cell->PlaceHint();
	}
}

void Grid::HandleCellCleared(const Cell& openedCell)
{
	if (openedCell.HasBomb())
	{
		SDL_Event gameLost{ UserEvents::GAME_LOST };
		SDL_PushEvent(&gameLost);
	}
	else
	{
		--cellsToClear;
		if (cellsToClear == 0)
		{
			SDL_Event gameWon{ UserEvents::GAME_WON };
			SDL_PushEvent(&gameWon);
		}
	}
}

#pragma endregion

#pragma region HandleCells

std::vector<Cell*> Grid::GetNeigborCells(const Cell& cell)
{
	std::vector<Cell*> neighbors;
	int cellRow = cell.GetRow();
	int cellCol = cell.GetCol();

	for (int row = cellRow - 1; row <= cellRow + 1;row++)
	{
		if (row < 0 || row >= Config::GRID_ROWS) continue;

		for (int col = cellCol - 1; col <= cellCol + 1;col++)
		{
			if (col < 0 || col >= Config::GRID_COLUMNS) continue;
			if (col == cellCol && row == cellRow) continue;

			Cell* neighbor = GetChildToCell(row * Config::GRID_COLUMNS + col);
			neighbors.push_back(neighbor);
		}
	}

	return neighbors;
}

bool Grid::IsCellInsideArea(std::vector<Cell*> area, Cell* cell)
{
	for (Cell* areaCell : area)
	{
		if (areaCell == cell) return true;
	}

	return false;
}

Cell* Grid::GetChildToCell(int index)
{
	if (Cell* cell = dynamic_cast<Cell*>(GetChildren()[index]))
	{
		return cell;
	}

	std::cout << "Cannot convert component to cell" << std::endl;
	return nullptr;
}

#pragma endregion
