/*
 * Code is derived and revised from a school assignment
 */

#include <iostream>
#include <cassert>
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

using namespace std;

//#define M_PI 3.141592654

Toumesh myToucan;


// global variable

bool m_Smooth{false};
bool m_Highlight{false};
GLfloat angle{0.0f};   /* in degrees */
GLfloat angle2{0.0f};   /* in degrees */
GLfloat zoom{1.0f};
int mouseButton{0};
int moving, startx, starty;

#define NO_OBJECT 4;
int current_object = 0;

using namespace std;

void setupLighting() {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_NORMALIZE);

  // Lights, material properties
  GLfloat ambientProperties[] = {0.7f, 0.7f, 0.7f, 1.0f};
  GLfloat diffuseProperties[] = {0.8f, 0.8f, 0.8f, 1.0f};
  GLfloat specularProperties[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat lightPosition[] = {-100.0f, 100.0f, 100.0f, 1.0f};

  glClearDepth(1.0);

  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientProperties);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularProperties);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

  // Default : lighting
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

}

void display_callback(void) {

  float mat_specular[] = {0.3f, 0.3f, 0.3f, 1.0f};
  float mat_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
  float mat_ambient_color[] = {0.8f, 0.8f, 0.2f, 1.0f};
  float mat_diffuse[] = {0.1f, 0.5f, 0.8f, 1.0f};
  float shininess = 20;
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
  glRotatef(angle2, 1.0, 0.0, 0.0);
  glRotatef(angle, 0.0, 1.0, 0.0);
  glScalef(zoom, zoom, zoom);
  myToucan.draw_mesh();
  glPopMatrix();
  glutSwapBuffers();
}

void keyboard_callback(unsigned char key, int x, int y) {
  string filename;
  switch (key) {
  case 'p':
  case 'P': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case 'w':
  case 'W': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  case 'v':
  case 'V': glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    break;
  case 's':
  case 'S': m_Smooth = !m_Smooth;
    break;
  case 'h':
  case 'H': m_Highlight = !m_Highlight;
    break;
  case 'o':
  case 'O': cout << "Enter the filename you want to write:";
    cin >> filename;
    myToucan.write_file(filename);
    break;
  case '1': cout << "Enter your STL file name:";
    cin >> filename;
    myToucan.reset_obj();
    myToucan.read_stl(std::move(filename));
    break;
  case '2': cout << "Enter your OBJ file name:";
    cin >> filename;
    myToucan.reset_obj();
    myToucan.read_obj(filename);
    break;
  case '3': cout << "Enter your PLY file name:";
    cin >> filename;
    myToucan.reset_obj();
    myToucan.read_ply(std::move(filename));
    break;
  case '4': current_object = key - '1';
    break;
  case '5': cout << "Enter your X3D file name:";
    cin >> filename;
    myToucan.reset_obj();
    myToucan.read_x3d(std::move(filename));
    break;

  case 'Q':
  case 'q': exit(0);
    break;

  default: break;
  }

  glutPostRedisplay();
}

void
mouse_callback(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    mouseButton = button;
    moving = 1;
    startx = x;
    starty = y;
  }
  if (state == GLUT_UP) {
    mouseButton = button;
    moving = 0;
  }
}

void motion_callback(int x, int y) {
  if (moving) {
    if (mouseButton == GLUT_LEFT_BUTTON) {
      angle = angle + (x - startx);
      angle2 = angle2 + (y - starty);
    } else
      zoom += ((y - starty) * 0.001);
    startx = x;
    starty = y;
    glutPostRedisplay();
  }

}

int main(int argc, char **argv) {
// Testing enext working properly
  int test_cases{1000000};
  for (int i = 0; i < test_cases; ++i) {
    assert(version_of(enext(make_tri_edge(i, 0))) == 1);
    assert(version_of(enext(make_tri_edge(i, 1))) == 2);
    assert(version_of(enext(make_tri_edge(i, 2))) == 0);
    assert(version_of(enext(make_tri_edge(i, 3))) == 5);
    assert(version_of(enext(make_tri_edge(i, 4))) == 3);
    assert(version_of(enext(make_tri_edge(i, 5))) == 4);
  }
  cout << "Enext working properly\n";
  // Testing sym working properly
  for (int i = 0; i < test_cases; ++i) {
    assert(version_of(sym(make_tri_edge(i, 0))) == 3);
    assert(version_of(sym(make_tri_edge(i, 1))) == 4);
    assert(version_of(sym(make_tri_edge(i, 2))) == 5);
    assert(version_of(sym(make_tri_edge(i, 3))) == 0);
    assert(version_of(sym(make_tri_edge(i, 4))) == 1);
    assert(version_of(sym(make_tri_edge(i, 5))) == 2);
  }
  cout << "Sym working properly\n";

  // Test org and dest with cat.obj
  cout << "Reading cat.obj\n";

  std::string test_file{"cat.obj"};
  myToucan.read_obj("cat.obj");
  //myToucan.print_first_n_triangles(10);
  myToucan.print_org_and_dest(1000);

  //cout << "1-4: Draw different objects"<<endl;
  cout << "S: Toggle Smooth Shading" << endl;
  cout << "H: Toggle Highlight" << endl;
  cout << "W: Draw Wireframe" << endl;
  cout << "P: Draw Polygon" << endl;
  cout << "V: Draw Vertices" << endl;
  cout << "O: Clone current mesh" << endl;
  cout << "1: Read STL\n";
  cout << "2: Read OBJ\n";
  cout << "3: Read PLY\n";
  cout << "5: Read X3D\n";
  cout << "Q: Quit" << endl << endl;

  cout << "Left mouse click and drag: rotate the object" << endl;
  cout << "Right mouse click and drag: zooming" << endl;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(600, 600);
  glutInitWindowPosition(50, 50);
  glutCreateWindow("Toucan Mesh Visualizer");
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glutDisplayFunc(display_callback);
  glutMouseFunc(mouse_callback);
  glutMotionFunc(motion_callback);
  glutKeyboardFunc(keyboard_callback);
  setupLighting();
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glMatrixMode(GL_PROJECTION);
  // hard-coded for now
  gluPerspective( /* field of view in degree */ 40.0,
      /* aspect ratio */ 1.0,
      /* Z near */ 1.0, /* Z far */ 80.0);
  glMatrixMode(GL_MODELVIEW);
  glutMainLoop();

  return 0;
}
