#pragma once

#include "cases/ICase.hpp"

#include <memory>
#include <vector>

class UiPanel {
public:
  static void draw(std::vector<std::unique_ptr<ICase>>& cases, int& currentCaseIndex);
};
