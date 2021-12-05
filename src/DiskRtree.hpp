#pragma once

template <class node_t>
class DiskRTree {
  using bbox_t = typename node_t::bbox_t;
  using point_t = typename bbox_t::point_t;
  using id_t = typename node_t::id_t;
  using pointSet_t = std::vector<point_t>;

  id_t rootId;

 public:
  DiskRTree() { rootId = 0; }
  ~DiskRTree() {}

  bool insert(point_t p) {
    auto root = node_t::get(rootId);
    assert(!p.null() && root);
    rootId = root->id();
    if (root->null()) {               // only root could be null
      return root->box.tryInsert(p);  // should return true
    }
    root->insert(p);
    if (root->parentId) {
      rootId = root->parentId;
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
  /*
  template <class os_t>
  friend os_t& operator<<(os_t& os, DiskRTree<node_t>& n) {
    auto root = node_t::get(n.rootId);
    assert(root);
    return os << *root;
  }
  */

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
};