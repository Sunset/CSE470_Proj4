/*
  NOTE: This is the file you will need to begin working with
		You will need to implement the RayTrace::CalculatePixel () function

  This file defines the following:
	RayTrace Class
*/

#ifndef RAYTRACE_H
#define RAYTRACE_H
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include "Utils.h"

/*
	RayTrace Class - The class containing the function you will need to implement

	This is the class with the function you need to implement
*/

class RayTrace
{
public:
	/* - Scene Variable for the Scene Definition - */
	Scene m_Scene;
	//SceneObject m_SceneObject;
	boolean intersection;
	boolean shadow;
	// -- Constructors & Destructors --
	int depth;
	RayTrace (void) {}
	~RayTrace (void) {}

	// -- Main Functions --
	// - CalculatePixel - Returns the Computed Pixel for that screen coordinate
	Vector CalculatePixel (int screenX, int screenY)
	{
		//direction of the ray for x and y pixel
		Vector Ray_Vec = ComputeRay(screenX,screenY);

		if ((screenX < 0 || screenX > WINDOW_WIDTH - 1) ||
			(screenY < 0 || screenY > WINDOW_HEIGHT - 1))
		{
			// Off the screen, return black
			return Vector (0.0f, 0.0f, 0.0f);
		}
		//trace(origin, direction, depth"n")
		return Trace(m_Scene.GetCamera().position,Ray_Vec,0);
	}
	bool isSphereIntersection(SceneSphere *SObject,Vector Origin_Intersect, Vector Ray_Intersect,float &distance)
	{

		float a=Ray_Intersect.Dot(Ray_Intersect);
		float b = (Origin_Intersect-SObject->center).Dot(Ray_Intersect)*2;
		float c = (Origin_Intersect-SObject->center).Dot((Origin_Intersect-SObject->center))
			-(SObject->radius)*(SObject->radius);

		float result = b*b - (4  * c*a);
		if(result < 0)
		{
			return false;
		}
		else
		{
			float FirstT = (-b + sqrt(result))/(2*a);
			float SecondT= (-b - sqrt(result))/(2*a);
			if(FirstT>0 && SecondT>0)
			{
				if(FirstT<SecondT)
				{
					distance=FirstT;
					return true;
				}
				else
				{
					distance = SecondT;
					return true;
				}
			}
			else if(FirstT <0 && SecondT>0)
			{
				distance = SecondT;
				return true;
			}
			else if(FirstT >0 && SecondT<0)
			{
				distance = FirstT;
				return true;
			}
			else
			{
				return false;
			}
		}
		return false;
	}
	Vector Phong( Vector Light1, Vector CalculatedRay, SceneBackground Background, SceneMaterial Material,Vector View_Vec, Vector Normal)
	{
		Vector R = (Normal * (Light1.Dot(Normal))* 2.0 - Light1).Normalize();
		Vector Diffuse = Material.diffuse*max(0.0,Normal.Dot(Light1));
		Vector Specular=Material.specular*pow(max(0.0,R.Dot(View_Vec)),(double)Material.shininess);
		Vector Ambient = Background.ambientLight;
		Vector colorOut = Diffuse+Specular;//+Ambient;//+Background.color;
		//if(Normal.Dot(Light1)>0)
			//printf("ok\n");
		Vector Red(1,0,0);
		return colorOut;
	}
	bool isTriangleIntersection(SceneTriangle *SObject,Vector Origin_Intersect, Vector Ray_Intersect,float &distance)
	{
		Vector a = SObject->vertex[0];
		Vector b = SObject->vertex[1];
		Vector c = SObject->vertex[2];

		Vector e1 = b-a;
		Vector e2 = c-a;
		Vector P = Ray_Intersect.Cross(e2);
		float AA = e1.Dot(P);
		if((AA>-0.0001) && (AA<0.0001))
			return false;
		float F = 1.0/AA;
		Vector S = Origin_Intersect-a;
		float U = F*(S.Dot(P));
		if((U<0.0) || (U>1.0))
			return false;
		Vector Q = S.Cross(e1);
		float V = F*(Ray_Intersect.Dot(Q));
		if((V<0.0)||((U+V)>1.0))
			return false;
		distance = F*(e2.Dot(Q));
		return true;
	}	
	bool isModelIntersection(SceneModel *SObject,Vector Origin_Intersect, Vector Ray_Intersect,float &distance)
	{
		for(int i = 0; i<SObject->GetNumTriangles();i++)
		{
			isTriangleIntersection(SObject->GetTriangle(i),Origin_Intersect,Ray_Intersect,distance);
		}
		return false;
	}
	void DrawTiles(SceneTriangle *plane)
	{
		plane->u;
		plane->v;
	}
	Vector Trace(Vector O, Vector R,int n)
	{
		Vector Test(1,0,0);
		Vector Color (0,0,0);
		Vector Color1;
		//Allen Hsia 12/4/2013 2:15.
		//change to (n>2) to have reflectiveness!! :D :D :D :D 
		//Change to (n>0) to have regular shadows
		//change to (n>1) to have some reflection but with most of the color
		//change n>(anything bigger than 2) FOR AMAZING AWESOMENESS! :D
		if(n>0)
		{
			return Color;
		}
		intersection=false;

		float minimum= m_Scene.GetCamera().farClip;		//the T
		float dist=0.0f;
		SceneObject *closestObj;
	
		for(int i = 0; i<m_Scene.GetNumObjects();i++)
		{
			
			intersection = false;
			SceneObject *m_SceneObject=m_Scene.GetObject(i);
			
			if (m_SceneObject->IsSphere())
			{			
				intersection = isSphereIntersection((SceneSphere*)m_SceneObject,O,R,dist);
			}
			else if(m_SceneObject->IsTriangle())
			{
				intersection = isTriangleIntersection((SceneTriangle*)m_SceneObject,O,R,dist);
				DrawTiles((SceneTriangle*)m_SceneObject);
			}
			else if(m_SceneObject->IsModel())
			{
				intersection = isModelIntersection((SceneModel*)m_SceneObject,O,R,dist);
			}
			if(intersection==true)
			{
				if(dist<minimum)
				{
					minimum=dist;
					closestObj= m_SceneObject;
				}	
			}
		}//end of for loop for objects
		
		if(minimum== m_Scene.GetCamera().farClip)
		{
			Color= m_Scene.GetBackground().color;
			return Color;
		}
		//doing shadows past here

		float light_dist=0.0f;
		float isShadow = 1.0f;
		Vector PoI = O + R*minimum;
		Vector View = R;
		for(int j = 0;j<m_Scene.GetNumLights();j++)
		{
			shadow = false;
			SceneLight m_SceneLight=m_Scene.GetLight(j);
			Vector Light = (m_SceneLight.position- PoI).Normalize() ;		//L->
			for(int k=0;k<m_Scene.GetNumObjects();k++)//in between light and the closet object
			{	
				SceneObject *m_SceneObject1=m_Scene.GetObject(k);
				if(m_SceneObject1->IsSphere())
				{
					//object, origin, ray_intersect, distance
					shadow = isSphereIntersection((SceneSphere*)m_SceneObject1, PoI+(Light*0.0001), Light, light_dist);
				}
				if(m_SceneObject1->IsTriangle())
				{
					//ALlen HSia 3:58 12/4/2013. Uncommenting this will cause the plan to be all black
					//shadow = isTriangleIntersection((SceneTriangle*)m_SceneObject1, PoI+(Light*0.0001), Light, light_dist);
				}
				if(shadow==true)
				{
					printf("isShadow");//debugging purposes
					
					isShadow=0.0;
					break;
				}
			}
			//1:38 12/4/2013 Allen Hsia. Uncommenting this will cause everything to be black. I really have no idea why.
			//if(closestObj->IsSphere()){
				SceneSphere *mySphere = (SceneSphere*)closestObj;
				mySphere->center;
				mySphere->material;
				//find the normal of the intersection position. take the center of the object minus the object intersection 
				//Finding normal for a sphere
				Vector Normal (PoI.x - mySphere->center.x, PoI.y - mySphere->center.y, PoI.z - mySphere->center.z);
				Normal = Normal.Normalize();
			
				Vector Reflect = (Normal * (View.Dot(Normal))* 2.0 - View).Normalize();
				Color1 = Color1+ Phong(Light,R,m_Scene.GetBackground(),m_Scene.GetMaterial(mySphere->material),View,Normal)*isShadow+  Trace(PoI+Reflect*.0001,Reflect,n+1);// +Trace(Reflection) +Trace(Refraction);
			//}
			/*if(closestObj->IsTriangle())
			{
				SceneTriangle *myTriangle = (SceneTriangle*)closestObj;
				Vector edge1 = myTriangle->vertex[1]-myTriangle->vertex[0];
				Vector edge2 = myTriangle->vertex[2]-myTriangle->vertex[0];
				Vector Cross = edge1.Cross(edge2).Normalize();
				Vector Reflect = (Cross * (View.Dot(Cross))* 2.0 - View).Normalize();
				Color1 = Color1+ Phong(Light,R,m_Scene.GetBackground(),m_Scene.GetMaterial(myTriangle->material[0]),View,Cross)*isShadow+  Trace(PoI+Reflect*.0001,Reflect,n+1);// 
			}*/
		}			
		return Color1;
	}
	Vector ComputeRay(int x, int y)
	{
							//the near clip					get field of view
		float height = 2.0 * m_Scene.GetCamera().nearClip*tan(m_Scene.GetCamera().fieldOfView/2.0 *(M_PI/180.0));

		float width = height * ((float)WINDOW_WIDTH/(float)WINDOW_HEIGHT);
		Vector Up_Vec =	m_Scene.GetCamera().up;//the up vector
		Vector View_Vec = m_Scene.GetCamera().target-m_Scene.GetCamera().position;//the vector from eye to position (position-eye)
		View_Vec = View_Vec.Normalize();
		Vector Left_Vec = Up_Vec.Cross(View_Vec);

		Vector Ray = Up_Vec * height *((float)y/(float)WINDOW_HEIGHT - 0.5) - Left_Vec*width*((float)x/(float)WINDOW_WIDTH - 0.5)+ (View_Vec * m_Scene.GetCamera().nearClip); 
		Ray = Ray.Normalize();

		return Ray;
	}
};

#endif // RAYTRACE_H
