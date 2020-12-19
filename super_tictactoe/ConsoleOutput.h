#pragma once
#include<iostream>
#include<windows.h>
class ConsoleOutput {
public:
	template<typename _tGrid>
	void display(const _tGrid& grid) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					for (int l = 0; l < 3; l++)
						std::cout << grid.get(POINT{ i * 3 + k,j * 3 + l }) << " ";
					std::cout << " ";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		POINT last_pos = grid.get_last_pos();
		if (last_pos.x != -1)
			std::cout << "last pos:" << last_pos.x+1 << " " << last_pos.y+1 << std::endl;
		std::cout << "limit:" << grid.is_limit() << std::endl;
	}
};