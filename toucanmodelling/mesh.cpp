/*
 * Code is derived and revised from a school assignment
 */
#include "mesh.h"

#ifdef _WIN32
#include <Windows.h>
#include "GL\glut.h"
#define M_PI 3.141592654
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#elif __linux__
#include "GL/freeglut.h"
#endif

#include <cmath>
#include <string>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "mesh.h"
#include <map>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <cfloat>
using namespace std;

#define MESH_SCALER 4.20

///////////////////////////////////////////////////////
// I/O FUNCTIONS
///////////////////////////////////////////////////////

void Toumesh::read_stl(std::string filename) {
  cout << "Opening " << filename << endl;
  ifstream inFile;
  inFile.open(filename);
  if (!inFile.is_open()) {
    cout << "We cannot find your file " << filename << endl;
    exit(1);
  }

  string line;
  string trash;
  bool firstVertex = true;
  int faceIndex{1};
  double currCood;

  int i, j;
  while (getline(inFile, line)) {

    // HOLY SHIT string stream is slow
    // Todo(small): learn to optimize string stream?
/*
	std::istringstream iss(line);
	iss >> trash;
	if (trash[0] == 'v' || trash[0] == 'f') {
	  if (trash[0] == 'v') {
		++vertexCount;
		for (int k{0}; k < 3; ++k) {
		  iss >> currCood;
		  vList[vertexCount][k] = currCood;

		  if (firstVertex) minCoord[k] = maxCoord[k] = currCood;
		  else {
			if (minCoord[k] > currCood)
			  minCoord[k] = currCood;
			if (maxCoord[k] < currCood)
			  maxCoord[k] = currCood;
		  }
		  firstVertex = false;
		}
	  }
	  if (trash[0] == 'f') {
		++triCount;
		iss >> trash;
		for (int k = 0; k < 3; k++) {
		  iss >> normalCood;
		  nList[triCount][k] = normalCood;
		  triList[triCount][k] = faceIndex;
		  fnextList[triCount][k] = 0;
		  ++faceIndex;
		}
*/

    if ((line[6] == 'v' && line[12] == ' ') || (line[2] == 'f' && line[14] == ' ')) {
      if (line[6] == 'v') {
        vertexCount++;
        i = 12;
        const string linec{line};
        for (int k = 0; k < 3; k++) { // k is 0,1,2 for x,y,z
          skip_trash(linec, i, j);
          currCood = vList[vertexCount][k] = atof(line.substr(i, j - i).c_str());
          if (firstVertex)
            minCoord[k] = maxCoord[k] = currCood;
          else {
            if (minCoord[k] > currCood)
              minCoord[k] = currCood;
            if (maxCoord[k] < currCood)
              maxCoord[k] = currCood;
          }
          i = j;
          firstVertex = false;
        }
      }
      if (line[2] == 'f') {
        //iss >> trash;
        ++triCount;
        i = 14;
        const string linec{line};
        for (int k = 0; k < 3; k++) {
          skip_trash(linec, i, j);
          nList[triCount][k] = atof(line.substr(i, j - i).c_str());
          triList[triCount][k] = faceIndex;
          fnextList[triCount][k] = 0;
          i = j;
          ++faceIndex;
        }

      }
    }
  }
  cout << "No. of vertices: " << vertexCount << endl;
  cout << "No. of triangles: " << triCount << endl;
  compute_statistics();
}

void Toumesh::read_ply(std::string filename) {
  cout << "Opening " << filename << endl;
  ifstream inFile;
  inFile.open(filename);
  if (!inFile.is_open()) {
    cout << "We cannot find your file " << filename << endl;
    exit(1);
  }

  string line;
  int i{0}, j{0};
  int vertexCounter{1};
  int faceCounter{1};
  bool firstVertex = true;
  double currCood;
  bool headerEnded = false;

  while (getline(inFile, line)) {
    i = 0;
    j = 0;

    // Processing vertex lines
    if (headerEnded && vertexCounter <= vertexCount) {
      for (int k{0}; k < 3; ++k) {
        // x.xxxx y.yyyy z.zzzz
        while (line[j] != ' ' && line[j] != '\0')
          ++j;
        currCood = vList[vertexCounter][k] = atof(line.substr(i, j - i).c_str());
        if (firstVertex)
          minCoord[k] = maxCoord[k] = currCood;
        else {
          if (minCoord[k] > currCood)
            minCoord[k] = currCood;
          if (maxCoord[k] < currCood)
            maxCoord[k] = currCood;
        }
        i = j + 1;
        ++j;
      }
      ++vertexCounter;
      firstVertex = false;
    }
      // Processing face lines
    else if (headerEnded) {
      // 3 aaaa bbbb cccc
      const string linec{line};
      i = 1;
      for (int k{0}; k < 3; ++k) {
        skip_trash(linec, i, j);
        // +1 here since ply files count from 0
        triList[faceCounter][k] = atoi(line.substr(i, j - i).c_str()) + 1;
        fnextList[faceCounter][k] = 0;
        i = j;
      }
      ++faceCounter;
    } else if (line[0] == 'e') {
      if (line[1] == 'n') {
        headerEnded = true;
        continue;
      }
      // Skip over "element "
      while (line[i] != ' ') {
        ++i;
      }
      ++i;
      if (line[i] == 'v' || line[i] == 'f') {
        int temp_i{i};
        // Skip over "vertex" or "face"
        while (line[i] != ' ') {
          ++i;
        }
        ++i;
        j = i;
        while (line[j] != ' ' && line[j] != '\0') {
          ++j;
        }
        if (line[temp_i] == 'v')
          vertexCount = atoi(line.substr(i, j - i).c_str());
        else
          triCount = atoi(line.substr(i, j - i).c_str());
      }
    }
  }

  for (int tri{1}; tri <= triCount; ++tri) {
    compute_normal(triList[tri], nList[tri]);
  }
  // END OF MINE


  cout << "No. of vertices: " << vertexCount << endl;
  cout << "No. of triangles: " << triCount << endl;
  compute_statistics();
}
void Toumesh::read_x3d(std::string filename) {
  cout << "Opening " << filename << endl;
  ifstream inFile;
  inFile.open(filename);
  if (!inFile.is_open()) {
    cout << "We cannot find your file " << filename << endl;
    exit(1);
  }

  string line;
  int i{0}, j{0};
  constexpr int faceLineNumber{11};
  constexpr int coordLineNumber{12};
  int lineNumber{0};
  bool firstVertex = true;
  double currCood;

  int k{0}; // for xyz, ranges from 0 to 2
  while (getline(inFile, line)) {
    if (lineNumber == faceLineNumber) { //faces
      const string linec{line};
      while (!isdigit(linec[i])) {
        ++i; // skip until first face
      }
      j = i;
      while (linec[j] != '\0' && j < line.size()) {
        while (linec[j] != ' ' && linec[j] != '\0') {
          ++j;
        }
        if (linec[i] == '-') { // -1 separate faces in x3d
          i = ++j;
          k = 0;
          ++triCount;
          continue;
        }
        triList[triCount + 1][k] = atoi(line.substr(i, j - i).c_str()) + 1; // +1 since x3d starts counting at 0
        fnextList[triCount + 1][k] = 0;
        i = ++j;
        ++k;
      }
    } else if (lineNumber == coordLineNumber) {
      i = j = 0;
      k = 0;
      const string linec{line};
      while (linec[i] != '\"') {
        ++i; // skip until first vertex
      }
      ++i;
      j = i;
      while (linec[j] != '\0' && j < line.size()) {
        while (linec[j] != ' ' && linec[j] != '\0') {
          if (j == line.size() - 3)
            break;
          ++j;
        }
        currCood = vList[vertexCount + 1][k] = atof(line.substr(i, j - i).c_str());
        if (firstVertex) {
          minCoord[k] = maxCoord[k] = currCood;
          firstVertex = false;
        } else {
          if (minCoord[k] > currCood)
            minCoord[k] = currCood;
          if (maxCoord[k] < currCood)
            maxCoord[k] = currCood;
        }
        if (k == 2) {
          k = -1;
          ++vertexCount;
        }
        ++k;
        i = ++j;
      }
      break;
    }
    ++lineNumber;
  }
  for (int tri{1}; tri <= triCount; ++tri) {
    compute_normal(triList[tri], nList[tri]);
  }
  cout << "No. of vertices: " << vertexCount << endl;
  cout << "No. of triangles: " << triCount << endl;
  compute_statistics();
}

void Toumesh::read_obj(std::string filename) {
  cout << "Opening " << filename << endl;
  ifstream inFile;
  inFile.open(filename);
  if (!inFile.is_open()) {
    cout << "We cannot find your file " << filename << endl;
    exit(1);
  }

  string line;
  int i, j;
  bool firstVertex = true;
  double currCood;

  while (getline(inFile, line)) {
    if ((line[0] == 'v' || line[0] == 'f') && line[1] == ' ') {
      if (line[0] == 'v') {
        vertexCount++;
        i = 1;
        const char* linec{line.data()};
        for (int k = 0; k < 3; k++) { // k is 0,1,2 for x,y,z
          while (linec[i] == ' ')
            i++;
          j = i;
          while (linec[j] != ' ' && linec[j] != '\0')
            j++;
          currCood = vList[vertexCount][k] = atof(line.substr(i, j - i).c_str());
          if (firstVertex)
            minCoord[k] = maxCoord[k] = currCood;
          else {
            if (minCoord[k] > currCood)
              minCoord[k] = currCood;
            if (maxCoord[k] < currCood)
              maxCoord[k] = currCood;
          }
          i = j;
        }
        firstVertex = 0;
      }
      if (line[0] == 'f') {
        triCount++;
        i = 1;
        //const string linec{line};
        const string linec {line};
        for (int k = 0; k < 3; k++) {
          while (linec[i] == ' ')
            i++;
          j = i;
          while (linec[j] != ' ' && linec[j] != '\\' && linec[j] != '\0')
            j++;
          triList[triCount][k] = atof(line.substr(i, j - i).c_str());
          i = j;
          fnextList[triCount][k] = 0;
          while (linec[j] != ' ' && linec[j] != '\0')
            j++;
        }
      }
    }
  }

  for (int tri{1}; tri <= triCount; ++tri) {
    compute_normal(triList[tri], nList[tri]);
  }

  cout << "Vertices count: " << vertexCount << endl;
  cout << "Triangles count: " << triCount << endl;
  compute_statistics();
}

void Toumesh::write_file(const std::string &filename) {
  std::ofstream outFile(filename, std::ios::out);
  if (!outFile.is_open()) {
    cout << "Cease and desist \n" << filename << '\n';
    exit(1);
  }

  outFile << "# " << vertexCount << " vertices, " << triCount << " triangles\n";
  for (int v{1}; v <= vertexCount; ++v) {
    outFile << "v " << vList[v][0] << ' ' << vList[v][1] << ' ' << vList[v][2] << '\n';
  }
  for (int f{1}; f <= triCount; ++f) {
    outFile << "f " << triList[f][0] << ' ' << triList[f][1] << ' ' << triList[f][2] << '\n';
  }

  std::cout << "Write complete!";
  outFile.close();
}
//////////////////////////////////////////////////////////////////////
// UTILITIES HELPER FUNCTIONS
// * computing statistics such as minimum angles
// * drawing mesh
// * resetting the object stats (for re-reads)
// * skip trashes while reading
// * computing normals
// * some maths
//////////////////////////////////////////////////////////////////////

void Toumesh::draw_mesh() {

  glEnable(GL_LIGHTING);

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

  glPushMatrix();
  double longestSide{0.0};
  for (int i = 0; i < 3; i++)
    if (longestSide < (maxCoord[i] - minCoord[i]))
      longestSide = (maxCoord[i] - minCoord[i]);
  glScalef(MESH_SCALER / longestSide, MESH_SCALER / longestSide, MESH_SCALER / longestSide);
  glTranslated(-(minCoord[0] + maxCoord[0]) / 2.0,
               -(minCoord[1] + maxCoord[1]) / 2.0,
               -(minCoord[2] + maxCoord[2]) / 2.0);
  for (int i = 1; i <= triCount; i++) {
    glBegin(GL_POLYGON);
    glNormal3dv(nList[i]);
    for (int j = 0; j < 3; j++)
      glVertex3dv(vList[triList[i][j]]);
    glEnd();

  }
  glDisable(GL_LIGHTING);

  glPopMatrix();
}

void Toumesh::compute_statistics() {
  int i;
  double minAngle = DBL_MAX;
  double maxAngle = DBL_MIN;

  int v1;
  int v2;
  int v3;
  double vec1[3];
  double vec2[3];
  double vec3[3];
  double l1;
  double l2;
  double l3;
  std::vector<double> angles(3);

  for (int tri{1}; tri <= triCount; ++tri) {
    v1 = triList[tri][0];
    v2 = triList[tri][1];
    v3 = triList[tri][2];

    vec1[0] = vList[v2][0] - vList[v1][0];
    vec1[1] = vList[v2][1] - vList[v1][1];
    vec1[2] = vList[v2][2] - vList[v1][2];

    vec2[0] = vList[v3][0] - vList[v2][0];
    vec2[1] = vList[v3][1] - vList[v2][1];
    vec2[2] = vList[v3][2] - vList[v2][2];

    vec2[0] = vList[v3][0] - vList[v2][0];
    vec3[0] = vList[v3][0] - vList[v1][0];
    vec3[1] = vList[v3][1] - vList[v1][1];
    vec3[2] = vList[v3][2] - vList[v1][2];

    l1 = sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1] + vec1[2] * vec1[2]);
    l2 = sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1] + vec2[2] * vec2[2]);
    l3 = sqrt(vec3[0] * vec3[0] + vec3[1] * vec3[1] + vec3[2] * vec3[2]);

    angles[0] = angle_between_vector(vec1, vec2, l1, l2);
    angles[1] = angle_between_vector(vec2, vec3, l2, l3);
    angles[2] = angle_between_vector(vec1, vec3, l1, l3);

    const auto[smoll, beeg]{std::ranges::minmax(angles)};
    ++statMinAngle[static_cast<int>(smoll / 10)];
    ++statMaxAngle[static_cast<int>(beeg / 10)];

    if (smoll < minAngle)
      minAngle = smoll;
    if (maxAngle < beeg)
      maxAngle = beeg;
  }

  cout << "Min. angle = " << minAngle << endl;
  cout << "Max. angle = " << maxAngle << endl;

  cout << "Statistics for Maximum Angles" << endl;
  for (i = 0; i < 18; i++)
    cout << statMaxAngle[i] << " ";
  cout << endl;
  cout << "Statistics for Minimum Angles" << endl;
  for (i = 0; i < 18; i++)
    cout << statMinAngle[i] << " ";
  cout << endl;
}

void Toumesh::reset_obj() {
  triCount = 0;
  vertexCount = 0;
  minCoord[0] = minCoord[1] = minCoord[2] = 0;
  maxCoord[0] = maxCoord[1] = maxCoord[2] = 0;
}

void Toumesh::skip_trash(const std::string &linec, int &i, int &j) {
  while (linec[i] == ' ') {
    i++;
  }
  j = i;
  while (linec[j] != ' ' && linec[j] != '\0') {
    j++;
  }
}

void Toumesh::compute_normal(const int *triangle, double n[3]) {
  const double vec1[3]{
      vList[triangle[1]][0] - vList[triangle[0]][0],
      vList[triangle[1]][1] - vList[triangle[0]][1],
      vList[triangle[1]][2] - vList[triangle[0]][2]};
  const double vec2[3]{
      vList[triangle[2]][0] - vList[triangle[1]][0],
      vList[triangle[2]][1] - vList[triangle[1]][1],
      vList[triangle[2]][2] - vList[triangle[1]][2]};

  n[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
  n[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
  n[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
  double nLength{magnitude(n)};

  n[0] /= nLength;
  n[1] /= nLength;
  n[2] /= nLength;
}

double Toumesh::magnitude(const double *vec) const {
  return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

double Toumesh::angle_between_vector(
    const double *v1,
    const double *v2,
    const double l1,
    const double l2) const { return std::acos(dot_product(v1, v2) / (l1 * l2)) * 180.0 / M_PI; }

double Toumesh::dot_product(const double *v1,
                            const double *v2) const { return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]; }

//////////////////////////////////////////
// DEBUGGING
//////////////////////////////////////////
void Toumesh::print_first_n_triangles(int first_n) {
  if (triCount < first_n) {
    first_n = triCount;
  }
  for (int i = 1; i <= first_n; ++i) {
    cerr << i << ' ' << triList[i][0] << ' ' << triList[i][1] << ' ' << triList[i][2] << '\n';
  }
}
void Toumesh::print_org(TriEdge ot) {
  cerr << "Org: " << org(ot) << '\t';
}
void Toumesh::print_dest(TriEdge ot) {
  cerr << "Dest: " << dest(ot) << '\t';
}
void Toumesh::print_org_and_dest(int first_n) {
  if (triCount < first_n) {
    first_n = triCount;
  }
  for (int i = 1; i <= first_n; ++i) {
    cerr << i << ' ' << triList[i][0] << ' ' << triList[i][1] << ' ' << triList[i][2] << '\t';
    print_org(make_tri_edge(i, 0));
    print_dest(make_tri_edge(i, 0));
    cerr << '\n';
  }
}