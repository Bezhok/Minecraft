#pragma once
#include <vector>
#include "Entity.h"

using std::vector;

class Box :
	public Entity
{
public:
	Box();
	~Box();
protected:
	vector<GLuint> m_box;
};

