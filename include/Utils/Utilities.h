#pragma once

namespace SciRenderer
{
  namespace Utilities
  {
    // Searching functions for pairs.
    template <typename U, typename T>
    bool pairSearch(const std::vector<std::pair<U, T>> &list, const U &name)
    {
      auto loc = std::find_if(list.begin(), list.end(),
                              [&name](const std::pair<U, T> &pair)
      {
        return pair.first == name;
      });

      return loc != list.end();
    }

    template <typename U, typename T>
    std::vector<std::pair<U, T>>::iterator
    pairGet(std::vector<std::pair<U, T>> &list, const U &name)
    {
      auto loc = std::find_if(list.begin(), list.end(),
                              [&name](const std::pair<U, T> &pair)
      {
        return pair.first == name;
      });

      return loc;
    }
  }
}
