#pragma once
#include<windows.h>
#include<stdexcept>
#include<iostream>
template<typename _tGrid, typename _tOutput, typename _tPlayer1, typename _tPlayer2>
class Game {
public:
	Game() :m_grid(), m_p1(), m_p2(), m_output() {
		exception_level[0] = exception_level[1] = ThrowException;
	}
	template<typename _tPlayer>
	void Get(_tPlayer& current_player) {
		if (current_player.enabled_display) {
			system("cls");
			m_output.display(m_grid);
		}
		POINT result = current_player.get(m_grid);
		m_grid.press(result);
	}
	void Start() {
		while (m_grid.is_running()) {
			if (m_grid.get_side() == 1)
				Get(m_p1);
			else Get(m_p2);
		}
		system("cls");
		m_output.display(m_grid);
		std::cout << "game over!" << std::endl;
	}
	enum {
		ThrowException,
		Terminate,
		Retry
	}exception_level[2];
private:
	_tGrid m_grid;
	_tPlayer1 m_p1;
	_tPlayer2 m_p2;
	_tOutput m_output;
};