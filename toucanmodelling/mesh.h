/*
 * Code is derived and revised from a school assignment
 */
#pragma once
#include <string>

// maximum number of vertices and triangles

#define MAX_VERTEX_ALLOWED 5000000
#define MAX_TRIANGLE_ALLOWED 2000000

typedef int TriEdge;
typedef int TriIndex;

inline TriEdge make_tri_edge(TriIndex t, int version) { return (t << 3) | version; };
inline TriIndex idex_of(TriEdge te) { return te >> 3; };
inline int version_of(TriEdge te) { return te & 0b111; };
inline TriEdge enext(TriEdge te) {
  // 0 -> 1 -> 2 -> 0
  // 3 <- 4 <- 5 <- 3
  return make_tri_edge(idex_of(te),
                       (version_of(te) < 3) ? (version_of(te) + 1) % 3 :
                       ((version_of(te) - 1) < 3) ? version_of(te) + 2 : version_of(te) - 1);
};
inline TriEdge sym(TriEdge te) { return version_of(te)<3 ? version_of(te)+3 : version_of(te)-3; };

class Toumesh {
  int vertexCount = 0;
  int triCount = 0;

  double vList[MAX_VERTEX_ALLOWED][3];   // vertices list
  int triList[MAX_TRIANGLE_ALLOWED][3];      // triangle list
  int fnextList[MAX_TRIANGLE_ALLOWED][3];     // fnext list for future (nte this assignment)
  double nList[MAX_TRIANGLE_ALLOWED][3];   // storing triangle normals

  double maxCoord[3];          // the maximum coordinates of x,y,z
  double minCoord[3];          // the minimum coordinates of x,y,z

  int statMinAngle[18]; // each bucket is  degrees has a 10 degree range from 0 to 180 degree
  int statMaxAngle[18];

public:
  Toumesh() {
    vertexCount = 0;
    triCount = 0;
  };
  // reads and write
  void read_obj(std::string filename);         // assumming file contains a manifold
  void read_stl(std::string filename);         // Vertices count bloated due to duplicate counting, nte fixing for now
  void read_ply(std::string filename);
  void read_x3d(std::string filename);
  void write_file(const std::string& filename); // don't want to move here, might use after write?

  //MINE


  // utilities
  void draw_mesh();
  void compute_statistics();
  void reset_obj();
  void skip_trash(const std::string& linec, int &i, int &j);
  void compute_normal(const int *triangle, double *n);
  double magnitude(const double *vec) const;
  double dot_product(const double *v1, const double *v2) const;
  double angle_between_vector(
      const double *v1,
      const double *v2,
      double l1,
      double l2) const;

  // geometries, here TriEdge represents a vertex
  inline TriEdge org(TriEdge te) { return triList[idex_of(te)][0];}
  inline TriEdge dest(TriEdge te){ return triList[idex_of(te)][1];}

  // Debuggin
  void print_first_n_triangles(int first_n);
  void print_org(TriEdge te);
  void print_dest(TriEdge te);
  void print_org_and_dest(int first_n);
};


