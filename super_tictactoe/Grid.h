#pragma once
#include<stdexcept>
#include<memory.h>
#include<windows.h>
class Grid {
public:
	Grid() :m_last_state(9), m_is_end(false), m_side(1), m_limit(0), m_current_state(0), m_winner(0),
		m_last_pos{-1,-1},m_count(0){
		memset(m_grid, 0, sizeof m_grid);
	}

	int get_side() const { return m_side; }

	bool is_end()const { return m_is_end; }
	
	bool is_limit()const { return m_limit; }

	bool is_running()const { return !m_is_end; }

	int get(const POINT& pos)const {
		if (pos.x < 0 || pos.x>8 || pos.y < 0 || pos.y>8)
			throw std::out_of_range("参数超出范围");
		return m_grid[pos.x][pos.y];
	}

	int get_winner(int state)const {
		if (state < 0 || state>8)
			throw std::out_of_range("区块编号超出限制");
		return m_grid[9][state];
	}

	int get_global_winner()const {
		if (is_running())
			throw std::runtime_error("棋盘尚未结束");
		return m_winner;
	}

	int get_current_state()const { return m_current_state; }

	int get_state_count(int state)const {
		if (state < 0 || state>8)
			throw std::out_of_range("区块编号超出限制");
		return m_grid[10][state];
	}

	POINT get_last_pos()const {
		return m_last_pos;
	}

	int get_count()const { return m_count; }

	bool try_press(const POINT& pos) {
		if (m_is_end)return false;
		if (pos.x < 0 || pos.x>8 || pos.y < 0 || pos.y>8)return false;
		if (m_grid[pos.x][pos.y])return false;
		if (m_grid[9][pos.x])return false;
		if (m_limit && pos.x != m_current_state)return false;
		return true;
	}

	void press(const POINT& pos) {
		#pragma region 异常判定
		if (m_is_end)
			throw std::runtime_error("棋盘已结束");
		if (pos.x < 0 || pos.x>8 || pos.y < 0 || pos.y>8)
			throw std::out_of_range("不合法的落子位置");
		if (m_grid[pos.x][pos.y])
			throw std::runtime_error("该位置已有棋子");
		if(m_grid[9][pos.x])
			throw std::runtime_error("该区块已被占领");
		if (m_limit && pos.x != m_current_state)
			throw std::runtime_error("必须在当前限制的区块落子");
		#pragma endregion

		m_grid[pos.x][pos.y] = m_side;

		if (check(pos.x)) {
			set_winner(pos.x, m_side);
			if (check(9)) {
				m_winner = m_side;
				m_is_end = true;
			}
		}
		add_count(pos.x);
		if (get_state_count(pos.x) == 9 &&get_winner(pos.x) == 0)
		{
			set_winner(pos.x, 3);
		}
		if (get_winner(pos.x) && --m_last_state == 0) {
			m_is_end = true;
			m_winner = 3;
		}
		m_current_state = pos.y;
		if (get_winner(pos.y))m_limit = 0;
		else m_limit = 1;
		m_side = 3 - m_side;
		m_last_pos = pos;
		m_count++;
	}
private:
	//检测某个区块是否被占领
	//state：区块号，若为9，则为整盘棋是否已结束
	//为了防止脑抽，加上没什么用（划掉）的特判
	bool check(int state) {
		if (state < 0 || state>9)
			throw std::out_of_range("区块编号超出限制");
		//0 1 2
		//3 4 5
		//6 7 8
		static const int
			check_num[8][3] = { {0,1,2},{3,4,5},{6,7,8},
								{0,3,6},{1,4,7},{2,5,8},
								{0,4,8},{2,4,6} };
		bool result = false;
		for (int i = 0; i < 8 && !result; i++) {
			if (m_grid[state][check_num[i][0]] == m_side &&
				m_grid[state][check_num[i][1]] == m_side &&
				m_grid[state][check_num[i][2]] == m_side)
				result = true;
		}
		return result;
	}

	void set_winner(int state, int winner) {
		if (state < 0 || state>8)
			throw std::out_of_range("区块编号超出限制");
		m_grid[9][state] = winner;
	}

	void add_count(int state) {
		if (state < 0 || state>8)
			throw std::out_of_range("区块编号超出限制");
		++m_grid[10][state];
	}
private:
	//棋盘，m_grid[0-8][ ]为棋盘数据，
	//m_grid[9][ ]为每个区块的胜者
	//m_grid[10][ ]为每个区块已下的棋数
	int m_grid[11][9];
	int m_last_state;//剩余的可下区块数
	int m_side;//当前落子玩家
	bool m_is_end;//棋盘是否已结束
	bool m_limit;//是否有落子限制
	int m_current_state;//如果有落子限制，当前应该下哪个区块
	int m_winner;//胜者
	POINT m_last_pos;//上一次的落子坐标
	int m_count;//落子数，用于调试
};