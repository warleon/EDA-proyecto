#pragma once

#include <string>
template <class node_t>
class DiskRTree {
  using bbox_t = typename node_t::bbox_t;
  using point_t = typename bbox_t::point_t;
  using id_t = typename node_t::id_t;
  using pointSet_t = std::vector<point_t>;

  id_t rootId;

 public:
  // DiskRTree(std::string mainDir = ".", std::string ext = ".json") {}
  DiskRTree() { rootId = 0; }
  ~DiskRTree() {}

  bool insert(point_t p) {
    auto root = node_t::get(rootId);
    assert(!p.null() && root);
    rootId = root->id();              // in case new root had to be created
    if (root->null()) {               // only root could be null
      return root->box.tryInsert(p);  // should return true
    }

    id_t sRootId = root->insert(p);
    if (sRootId) {                        // if root has been splited
      auto nRoot = node_t::get(0);        // create new root to hold the 2 roots
      auto sRoot = node_t::get(sRootId);  // get sibling node  of past root
      // update the parent node of the sibling nodes
      rootId = nRoot->selfId;
      root->parentId = rootId;
      sRoot->parentId = rootId;
      // set the sibling nodes to be the new root sons
      nRoot->add(sRootId);
      nRoot->add(root->selfId);
      nRoot->resize();
    }
    return true;
  }
  pointSet_t getRegion(point_t a, point_t b) {
    pointSet_t result;
    bbox_t region(a, b);
    // TODO
    /*
    visitar solo los nodos q intersectan la region

    if(region.covers(currentPoint))
      result.push_back(currentPoint);
     */
    return result;
  }

 public:
  // Json format output
  template <class os_t>
  friend os_t& operator<<(os_t& os, DiskRTree<node_t>& rtree) {
    json format = rtree;
    return os << format;
  }
  // Json format input
  template <class is_t>
  friend is_t& operator>>(is_t& is, DiskRTree<node_t>& rtree) {
    json format;
    is >> format;
    rtree = format;
    return is;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(DiskRTree<node_t>, rootId)

  template <class os_t>
  friend void toSVG(os_t& os, DiskRTree<node_t>& object, size_t x, size_t y,
                    size_t width, size_t height) {
    return toSVG(os, *node_t::get(object.rootId), x, y, width, height);
  }
};