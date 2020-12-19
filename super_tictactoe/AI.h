#pragma once
#include<vector>
#include<thread>
#include<chrono>
#include<mutex>
#include<random>
#include<windows.h>
#include<iostream>
#include<ctime>
#include<cmath>
struct node {
	node() :father(nullptr), visit(0), lose(0), win(0), all_expanded(0),
		is_leaf(1), expand_count(0), last_pos{ -1,-1 } {
	}
	std::vector<node*> children;
	node* father;
	int visit, lose, win, expand_count;
	bool all_expanded, is_leaf;
	POINT last_pos;
	~node() {
		for (auto i : children)if (i != nullptr)delete i;
	}
};
bool operator==(const POINT& p1, const POINT& p2) {
	return p1.x == p2.x && p1.y == p2.y;
}
bool operator!=(const POINT& p1, const POINT& p2) {
	return !(p1 == p2);
}
template<typename _tGrid>
class AI {
public:
	AI() : m_grid(), m_current_grid()/*, m_mtx()*/ {
		//m_mtx.lock();
		//m_thread = std::thread(&AI<_tGrid>::mcts, this);
		//m_mtx.unlock();
		//m_thread.detach();
	}

	POINT get(const _tGrid& grid) {
		m_grid = grid;
		if (grid.get_last_pos() != POINT{ -1, -1 }) {
			POINT last_pos = grid.get_last_pos();
			node* new_root = nullptr;
			for (auto& i : m_root->children) {
				if (i->last_pos == last_pos) {
					new_root = i;
					i = nullptr;
					std::clog << "已找到落点" << std::endl;
					break;
				}
			}
			delete m_root;
			m_root = new_root;
			if (m_root)
				m_root->father = nullptr;
		}
		//delete m_root;
		//m_root = nullptr;
		if (m_root == nullptr) {
			std::clog << "未找到落点，创建新的" << std::endl;
			m_root = new node();
			m_root->last_pos.x = grid.get_last_pos().x;
			m_root->last_pos.y = grid.get_last_pos().y;
		}

		mcts();
		std::clog << "本次共拓展" << expand_count << "个节点" << std::endl;
		double maxx = -1;
		int index = -1;
		for (int i = 0; i < m_root->children.size(); i++) {
			if (m_root->children.at(i)->visit == 0)continue;
			double score = ucb(m_root->children.at(i));
			if (score == 1) {
				index = i;
				break;
			}
			if (score > maxx) {
				maxx = score;
				index = i;
			}
		}
		if (index == -1) {
			throw std::runtime_error("AI找不到合适的落点");
		}

		std::clog << "已找到答案" << std::endl;
		node* new_root = m_root->children.at(index);
		m_root->children.at(index) = nullptr;
		std::clog << "开始删点" << std::endl;
		delete m_root;
		std::clog << "删点完成" << std::endl;
		m_root = new_root;
		//m_mtx.unlock();
		std::clog << "已返回" << std::endl;
		return m_root->last_pos;
	}
	const static bool enabled_display = true;
private:

	void mcts() {
		expand_count = 0;
		clock_t start = clock();
		std::default_random_engine engine;
		engine.seed(time(nullptr));
		if (m_root == nullptr) {
			throw std::runtime_error("根节点为空");
		}
		while (clock() - start < CLOCKS_PER_SEC * 2) {
			//m_mtx.lock();

			node* nd = m_root;
			m_current_grid = m_grid;
			std::vector<POINT> tmp;
			get_available_choices(tmp);
			if (tmp.size() != m_root->children.size())
				throw std::runtime_error("???");
			if (!nd->is_leaf) {
				if (!nd->all_expanded) {
					if (++nd->expand_count >= nd->children.size())
						nd->all_expanded = true;
					for (auto i : nd->children)
						if (!i->visit) {
							nd = i;
							break;
						}
				}
				else {
					double maxx = -1e10;
					node* nxt = nullptr;
					for (auto i : nd->children) {
						double score = ucb(i);
						//std::clog << score << std::endl;
						if (score > maxx)nxt = i, maxx = score;
					}
					nd = nxt;
				}
				if (nd == nullptr) {
					int a = 0;
				}
				std::clog << "try press:"<<nd->last_pos.x+1<<" "<<nd->last_pos.y+1<<"...";
				m_current_grid.press(nd->last_pos);
			}
			std::clog << "succeed." << std::endl;
			while (!nd->is_leaf) {
				std::uniform_int_distribution<unsigned> dis(0, nd->children.size() - 1);
				int nxt = dis(engine);
				nd = nd->children.at(nxt);
				m_current_grid.press(nd->last_pos);
			}


			std::vector<POINT> choices;
			while (m_current_grid.is_running()) {
				nd->is_leaf = false;
				get_available_choices(choices);
				for (auto& i : choices) {
					node* child = new node();
					child->last_pos = i;

					child->father = nd;
					nd->children.push_back(child);
				}
				//int count = m_current_grid.get_count();
				std::uniform_int_distribution<unsigned> dis(0, nd->children.size() - 1);
				size_t nxt = dis(engine);
				if (++nd->expand_count == nd->children.size())
					nd->all_expanded = true;
				nd = nd->children.at(nxt);
				m_current_grid.press(nd->last_pos);
			}

			nd->is_leaf = true;
			int winner = m_current_grid.get_global_winner();
			while (nd) {
				nd->visit++;
				if (winner == 1)nd->win++;
				else if (winner == 2)nd->lose++;
				else if (winner != 3)throw std::runtime_error("非法的赢家");
				nd = nd->father;
			}
			++expand_count;
			//m_mtx.unlock();
		}
	}

	void get_available_choices(std::vector<POINT>& vec) {
		vec.clear();
		int state = m_current_grid.get_current_state();
		for (int i = 0; i < 9; i++) {
			if (!m_current_grid.is_limit()) {
				for (int j = 0; j < 9; j++)
					if (m_current_grid.try_press({ i,j }))vec.push_back({ i,j });
			}
			else if (m_current_grid.try_press({ state,i }))vec.push_back({ state,i });
		}
	}

	double ucb(node* nd)const {
		if (nd->visit == 0)
			throw std::runtime_error("节点尚未被访问");
		if (nd->father->visit == 0)
			throw std::runtime_error("父节点尚未被访问（玄学）");
		if (nd->father == nullptr)
			throw std::runtime_error("节点为父节点");
		const static double C = 1.414;
		node* fa = nd->father;
		return nd->win * 1.0 / nd->visit + C * sqrt(2 * log10(nd->visit) / fa->visit);
	}

	int expand_count = 0;

private:
	node* m_root;
	_tGrid m_grid, m_current_grid;
	//std::thread m_thread;
	//std::mutex m_mtx;
};