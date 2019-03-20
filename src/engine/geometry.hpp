#include "stdafx.h"
#pragma once

struct s_AABB // axis aligned BBox
{
  glm::vec3 min_point;
  glm::vec3 max_point;
  float h; // AUTOSAR: w ... 2do: fix!!
  float w; // AUTOSAR: h ... 2do: fix!!
  float l;
};
