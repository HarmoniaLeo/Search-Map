
// searchMap.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

// CsearchMapApp:
// 有关此类的实现，请参阅 searchMap.cpp
//

class CsearchMapApp : public CWinApp
{
public:
	CsearchMapApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CsearchMapApp theApp;

class element
{
protected:
	bool porr;
	int routeId;
public:
	bool getPorR() const { return porr; }
	int getRouteId() const { return routeId; }
};

class route :element
{
private:
	vector<int>* xList;
	vector<int>* yList;
	int length;
	int lastPointId;
public:
	route(vector<int>*, vector<int>*,int);
	int getLength() const { return length; }
	int getLastPointId() const{ return lastPointId; }
	void paint(Mat&,int,int);
	~route();
};

class point :element
{
private:
	int x, y,id;
	route* outRoute;
public:
	point(int, int, int, int);
	void giveOutRoute(route*);
	int getNextPoint() const
	{
		if (outRoute != nullptr)
			return outRoute->getLastPointId();
		else
			return 0;
	}
	void paint(Mat&,int);
	void paintRoute(Mat&, int,int);
	int getX() const { return x; }
	int getY() const { return y; }
	int getId() const { return id; }
};

class myMap
{
private:
	struct mappoint
	{
		element* elementThis;
		bool aval;
	};

	int xLength;
	int yLength;
	mappoint** target;
public:
	myMap(CString);
	myMap();
	int getRouteId(int x,int y) const
	{
		if (target[y][x].elementThis != nullptr)
			return target[y][x].elementThis->getRouteId();
		else
			return 0;
	}
	int getState(int, int);
	int getAval(int x, int y) const { return target[y][x].aval; }
	int getPointId(int x, int y) const
	{
		if (target[y][x].elementThis != nullptr && target[y][x].elementThis->getPorR() == 0)
			return ((point*)target[y][x].elementThis)->getId();
		else
			return -1;
	}
	int giveOutRoute(int x, int y,route* outRoute)
	{
		if (target[y][x].elementThis != nullptr && target[y][x].elementThis->getPorR() == 0)
			((point*)target[y][x].elementThis)->giveOutRoute(outRoute);
	}
	int getNextPoint(int x, int y)
	{
		if (target[y][x].elementThis != nullptr && target[y][x].elementThis->getPorR() == 0)
			((point*)target[y][x].elementThis)->getNextPoint();
		else
			return 0;
	}
	int getXLength() const { return xLength; }
	int getYLength() const { return yLength; }
	void set(int, int, element*);
	void reset();
	bool ready() const
	{
		if (target == nullptr)
			return 0;
		return 1;
	}
};

class status
{
private:
	int length, end;
	status *next;
public:
	status();
	status(int,int,status*);
	int getLength() const { return length; }
	int getEnd() const { return end; }
	status* getNext() const { return next; }
	void setLength(int);
	void setPos(int);
	void setNext(status*);
};