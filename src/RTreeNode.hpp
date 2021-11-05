#pragma once

template <class BBox_t, std::size_t M>
struct RTreeNode {
  using bbox_t = BBox_t;
  std::array<RTreeNode*, M> sons;
  bbox_t box;
};