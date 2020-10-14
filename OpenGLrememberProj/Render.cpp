#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������

	/*glBindTexture(GL_TEXTURE_2D, texId);
	float x, y;
	glColor3f(1, 1, 0);
	for (double i = 0; i <= 360;) {

		glBegin(GL_TRIANGLES);
		x = 5 * cos(i);
		y = 5 * sin(i);

		glTexCoord2d(0.5 * cos(i) + 0.5, 0.5 * sin(i) + 0.5);
		glVertex2d(x, y);

		i = i + .5;
		x = 5 * cos(i);
		y = 5 * sin(i);
		glTexCoord2d(0.5 * cos(i) + 0.5, 0.5 * sin(i) + 0.5);
		glVertex2d(x, y);

		glTexCoord2d(0.5, 0.5);
		glVertex2d(0, 0);

		glEnd();
		i = i + .5;
	}*/


	double A[] = { 0, 0, 0 };
	double B[] = { 2, -1, 0 };
	double C[] = { 4, 4, 0 };

	double D[] = { 2, -1, 0 };
	double E[] = { 6, -3, 0 };
	double F[] = { -4,-5, 0 };

	double G[] = { -4, -5, 0 };
	double H[] = { 2, -1, 0 };
	double I[] = { 0, 0, 0 };

	double K[] = { -4, -5, 0 };
	double L[] = { -6, -2, 0 };
	double M[] = { 0, 0, 0 };

	double N[] = { 0, 0, 0 };
	double O[] = { 4, 4, 0 };
	double P[] = { -5, 7, 0 };

	double Q[] = { 0, 0, 0 };
	double R[] = { -5, 7, 0 };
	double S[] = { -8, 5, 0 };

	double A1[] = { 0, 0, 5 };
	double B1[] = { 2, -1, 5 };
	double C1[] = { 4, 4, 5 };

	double D1[] = { 2, -1, 5 };
	double E1[] = { 6, -3, 5 };
	double F1[] = { -4,-5, 5 };

	double G1[] = { -4, -5, 5 };
	double H1[] = { 2, -1, 5 };
	double I1[] = { 0, 0, 5 };

	double K1[] = { -4, -5, 5 };
	double L1[] = { -6, -2, 5 };
	double M1[] = { 0, 0, 5 };

	double N1[] = { 0, 0, 5 };
	double O1[] = { 4, 4, 5 };
	double P1[] = { -5, 7, 5 };

	double Q1[] = { 0, 0, 5 };
	double R1[] = { -5, 7, 5 };
	double S1[] = { -8, 5, 5 };

	double AA[] = { 2, -1, 0 };
	double BB[] = { 2, -1, 5 };
	double CC[] = { 4, 4, 5 };
	double DD[] = { 4, 4, 0 };

	double AA1[] = { 6, -3, 0 };
	double BB1[] = { 6, -3, 5 };
	double CC1[] = { -4, -5, 5 };
	double DD1[] = { -4, -5, 0 };

	double AA2[] = { 2, -1, 0 };
	double BB2[] = { 2, -1, 5 };
	double CC2[] = { 6, -3, 5 };
	double DD2[] = { 6, -3, 0 };

	double AA3[] = { -6, -2, 0 };
	double BB3[] = { -6, -2, 5 };
	double CC3[] = { -4, -5, 5 };
	double DD3[] = { -4, -5, 0 };

	double AA4[] = { 0, 0, 0 };
	double BB4[] = { 0, 0, 5 };
	double CC4[] = { -6, -2, 5 };
	double DD4[] = { -6, -2, 0 };

	double AA5[] = { 0, 0, 0 };
	double BB5[] = { 0, 0, 5 };
	double CC5[] = { -8, 5, 5 };
	double DD5[] = { -8, 5, 0 };

	double AA6[] = { -5, 7, 0 };
	double BB6[] = { -5, 7, 5 };
	double CC6[] = { -8, 5, 5 };
	double DD6[] = { -8, 5, 0 };

	double AA7[] = { -5, 7, 0 };
	double BB7[] = { -5, 7, 5 };
	double CC7[] = { 4, 4, 5 };
	double DD7[] = { 4, 4, 0 };

	glNormal3b(0, 0, 1);

	glBegin(GL_TRIANGLES);
	glColor3d(0.9, 0.2, 0);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glEnd();

	glNormal3b(0, 0, 1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, 1);
	glColor3d(0.5, 0.2, 0);
	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(F);
	glEnd();

	glNormal3b(0, 0, 1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, 1);
	glColor3d(0.3, 0.2, 0);
	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(I);
	glEnd();

	glNormal3b(0, 0, 1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, 1);
	glColor3d(0.1, 0.2, 0);
	glVertex3dv(K);
	glVertex3dv(L);
	glVertex3dv(M);
	glEnd();

	glNormal3b(0, 0, 1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, 1);
	glColor3d(0.1, 0.4, 0);
	glVertex3dv(N);
	glVertex3dv(O);
	glVertex3dv(P);
	glEnd();

	glNormal3b(0, 0, 1);

	glBegin(GL_TRIANGLES);
	glColor3d(0.1, 0.7, 1);
	glVertex3dv(Q);
	glVertex3dv(R);
	glVertex3dv(S);
	glEnd();

	glNormal3b(0, 0, -1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, -1);
	glColor3d(0.9, 0.2, 0);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glEnd();

	glNormal3b(0, 0, -1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, -1);
	glColor3d(0.5, 0.2, 0);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glEnd();

	glNormal3b(0, 0, -1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, -1);
	glColor3d(0.3, 0.2, 0);
	glVertex3dv(G1);
	glVertex3dv(H1);
	glVertex3dv(I1);
	glEnd();

	glNormal3b(0, 0, -1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, -1);
	glColor3d(0.1, 0.2, 0);
	glVertex3dv(K1);
	glVertex3dv(L1);
	glVertex3dv(M1);
	glEnd();

	glNormal3b(0, 0, -1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, -1);
	glColor3d(0.1, 0.4, 0);
	glVertex3dv(N1);
	glVertex3dv(O1);
	glVertex3dv(P1);
	glEnd();

	glNormal3b(0, 0, -1);

	glBegin(GL_TRIANGLES);
	glNormal3b(0, 0, -1);
	glColor3d(0.1, 0.7, 0);
	glVertex3dv(Q1);
	glVertex3dv(R1);
	glVertex3dv(S1);
	glEnd();

	glNormal3b(25, -10, 0);

	glBegin(GL_QUADS);
	glNormal3b(25, -10, 0);
	glColor3d(0.2, 0.1, 0.7);
	glVertex3dv(AA);
	glVertex3dv(BB);
	glVertex3dv(CC);
	glVertex3dv(DD);
	glEnd();

	glNormal3b(10, -50, 0);

	glBegin(GL_QUADS);
	glNormal3b(10, -50, 0);
	glColor3d(0.4, 0.1, 0.7);
	glVertex3dv(AA1);
	glVertex3dv(BB1);
	glVertex3dv(CC1);
	glVertex3dv(DD1);
	glEnd();

	glNormal3b(10, 20, 0);

	glBegin(GL_QUADS);
	glNormal3b(10, 20, 0);
	glColor3d(0.4, 0.1, 0.5);
	glVertex3dv(AA2);
	glVertex3dv(BB2);
	glVertex3dv(CC2);
	glVertex3dv(DD2);
	glEnd();

	glNormal3b(-15, -10, 0);

	glBegin(GL_QUADS);
	glNormal3b(-15, -10, 0);
	glColor3d(0.1, 0.1, 0.5);
	glVertex3dv(AA3);
	glVertex3dv(BB3);
	glVertex3dv(CC3);
	glVertex3dv(DD3);
	glEnd();

	glNormal3b(-10, 30, 0);

	glBegin(GL_QUADS);
	glNormal3b(-10, 30, 0);
	glColor3d(0.8, 0.1, 0.5);
	glVertex3dv(AA4);
	glVertex3dv(BB4);
	glVertex3dv(CC4);
	glVertex3dv(DD4);
	glEnd();

	glNormal3b(-25, -40, 0);

	glBegin(GL_QUADS);
	glNormal3b(-25, -40, 0);
	glColor3d(0.8, 0.9, 0.5);
	glVertex3dv(AA5);
	glVertex3dv(BB5);
	glVertex3dv(CC5);
	glVertex3dv(DD5);
	glEnd();

	glNormal3b(-10, 15, 0);

	glBegin(GL_QUADS);
	glNormal3b(-10, 15, 0);
	glColor3d(0.4, 0.9, 0.5);
	glVertex3dv(AA6);
	glVertex3dv(BB6);
	glVertex3dv(CC6);
	glVertex3dv(DD6);
	glEnd();

	glNormal3b(15, 5, 0);

	glBegin(GL_QUADS);
	glNormal3b(15, 5, 0);
	glColor3d(0.7, 0.9, 0.5);
	glVertex3dv(AA7);
	glVertex3dv(BB7);
	glVertex3dv(CC7);
	glVertex3dv(DD7);
	glEnd();


	//����� ��������� ���������� ��������


   //��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
									//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}