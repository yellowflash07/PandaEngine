#pragma once

class Random
{
public:
	Random();
	~Random();
	static float GetRandomFloat(float min, float max);
	static int GetRandomInt(int min, int max);
};