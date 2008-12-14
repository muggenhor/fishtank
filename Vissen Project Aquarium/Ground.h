#pragma once

#include <vector>

class Ground
{
private:
	std::vector<int> ground;
	int widthAmount, lengthAmount;
public:
	Ground(int widthAmount, int lengthAmount);
	~Ground(void);

	void Draw();
};
