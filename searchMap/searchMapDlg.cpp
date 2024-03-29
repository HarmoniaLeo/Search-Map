
// searchMapDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "searchMap.h"
#include "searchMapDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

//全局变量
myMap target;
bool routeAval[13];
point* pointList[13][16];
route* routeList[13][16][16];
CBitmap m_backBitmap;
int chosenRoute;
CPoint p;
CString m_strProjFileName;
const int color[13][3] = { {0,0,0},{255,153,0},{25,25,0},{0,25,25},{0,255,0},{0,25,25},{0,0,255},{25,0,25},{192,32,32},{32,192,32},{32,32,192},{192,64,0},{0,192,64} };
Mat icon;
//边元素
route::route(vector<int>* xList, vector<int>* yList,int lastPointId)
{
	this->xList = xList;
	this->yList = yList;
	this->lastPointId = lastPointId;
	porr = 1;
	length = xList->size();
}

void route::paint(Mat& tar,int col,int sig)
{
	Vec3b pixel;
	pixel[0] = color[col][0];
	pixel[1] = color[col][1];
	pixel[2] = color[col][2];
	int start, end;
	if (sig == 0)
	{
		start = 0;
		end = xList->size();
	}
	else if (sig == 1)
	{
		start = 0;
		end = xList->size()/3;
	}
	else if (sig == 2)
	{
		start = xList->size() / 3;
		end = xList->size() / 3*2;
	}
	else if (sig == 3)
	{
		start = xList->size() / 3 * 2;
		end = xList->size();
	}
	for (int i =start; i < end; i++)
	{
		int x = (*xList)[i],y=(*yList)[i];
		tar.at<Vec3b>(y * 2, x * 2) = pixel;
		target.set(x, y, this);
		if (x * 2 + 1 < tar.cols)
			tar.at<Vec3b>(y * 2, x * 2 + 1) = pixel;
		if (y * 2 + 1 < tar.rows)
			tar.at<Vec3b>(y * 2 + 1, x * 2) = pixel;
		if (x * 2 - 1 >= 0)
		{
			tar.at<Vec3b>(y * 2, x * 2 - 1) = pixel;
			target.set(x - 1, y, this);
			if (y * 2 - 1 >= 0)
			{
				tar.at<Vec3b>(y * 2 - 1, x * 2 - 1) = pixel;
				target.set(x - 1, y-1, this);
			}
			if (y * 2 + 1 >= 0)
			{
				tar.at<Vec3b>(y * 2 + 1, x * 2 - 1) = pixel;
				target.set(x - 1, y + 1, this);
			}
		}
		if (y * 2 - 1 >= 0)
		{
			tar.at<Vec3b>(y * 2 - 1, x * 2) = pixel;
			target.set(x, y - 1, this);
			if (x * 2 - 1 >= 0)
			{
				tar.at<Vec3b>(y * 2 - 1, x * 2 - 1) = pixel;
				target.set(x - 1, y - 1, this);
			}
			if (x * 2 + 1 >= 0)
			{
				tar.at<Vec3b>(y * 2 - 1, x * 2 + 1) = pixel;
				target.set(x + 1, y - 1, this);
			}
		}
	}
}

route::~route()
{
	delete xList;
	delete yList;
}

//点元素
point::point(int x, int y, int id, int routeId)
{
	this->x = x;
	this->y = y;
	this->id = id;
	this->routeId = routeId;
	porr = 0;
	outRoute = nullptr;
}

void point::giveOutRoute(route* outRoute)
{
	this->outRoute = outRoute;
}

void point::paint(Mat& tar,int col)
{
	Vec3b pixel;
	pixel[0] = color[col][0];
	pixel[1] = color[col][1];
	pixel[2] = color[col][2];
	tar.at<Vec3b>(y * 2, x * 2) = pixel;
	target.set(x, y, this);
	if(x * 2 + 1<tar.cols)
		tar.at<Vec3b>(y * 2, x * 2+1) = pixel;
	if (x * 2 + 2 < tar.cols)
	{
		tar.at<Vec3b>(y * 2, x * 2 + 2) = pixel;
		target.set(x+1, y, this);
	}
	if (y * 2 + 1 < tar.rows)
		tar.at<Vec3b>(y * 2+1, x * 2) = pixel;
	if (x * 2 + 1 < tar.cols && y * 2 + 1 < tar.rows)
	tar.at<Vec3b>(y * 2+1, x * 2+1) = pixel;
	if (y * 2 + 2 < tar.rows)
	{
		tar.at<Vec3b>(y * 2 + 2, x * 2) = pixel;
		target.set(x, y+1, this);
	}
	if (x * 2 - 1 >= 0)
	{
		tar.at<Vec3b>(y * 2, x * 2 - 1) = pixel;
		target.set(x - 1, y, this);
	}
	if (x * 2 - 2 >= 0)
	{
		tar.at<Vec3b>(y * 2, x * 2 - 2) = pixel;
		target.set(x-1, y, this);
	}
	if (y * 2 - 1 >= 0)
	{
		tar.at<Vec3b>(y * 2 - 1, x * 2) = pixel;
		target.set(x, y - 1, this);
	}
	if (y * 2 - 1 >= 0 && x * 2 - 1 >= 0)
	{
		tar.at<Vec3b>(y * 2 - 1, x * 2 - 1) = pixel;
		target.set(x-1, y - 1, this);
	}
	if (y * 2 - 2 >= 0)
	{
		tar.at<Vec3b>(y * 2 - 2, x * 2) = pixel;
		target.set(x, y-1, this);
	}
	if (x * 2 - 1 >= 0 && y * 2 + 1 < tar.rows)
	{
		tar.at<Vec3b>(y * 2 + 1, x * 2 - 1) = pixel;
		target.set(x-1, y, this);
	}
	if (y * 2 - 1 >= 0 && x * 2 + 1 < tar.cols)
	{
		tar.at<Vec3b>(y * 2 - 1, x * 2 + 1) = pixel;
		target.set(x, y - 1, this);
	}
	for (int i = -25; i < 25; i++)
	{
		for (int j = -25; j < 25; j++)
		{
			if (id != 0)
			{
				if (y * 2 + i >= 0 && y * 2 + i < tar.rows&&x * 2 + j >= 0 && x * 2 + j < tar.cols && !(icon.at<Vec3b>(j + 25, i + 25)[0] == 255 && icon.at<Vec3b>(j + 25, i + 25)[1] == 255 && icon.at<Vec3b>(j + 25, i + 25)[2] == 255) && !(icon.at<Vec3b>(j + 25, i + 25)[0] == 0 && icon.at<Vec3b>(j + 25, i + 25)[1] == 0 && icon.at<Vec3b>(j + 25, i + 25)[2] == 0))
					tar.at<Vec3b>(y * 2 + i, x * 2 + j) = icon.at<Vec3b>(j + 25, i + 25);
			}
			else
			{
				if (y * 2 + i >= 0 && y * 2 + i < tar.rows&&x * 2 + j >= 0 && x * 2 + j < tar.cols && !(icon.at<Vec3b>(j + 25, i + 25)[0] == 255 && icon.at<Vec3b>(j + 25, i + 25)[1] == 255 && icon.at<Vec3b>(j + 25, i + 25)[2] == 255) && !(icon.at<Vec3b>(j + 25, i + 25)[0] == 0 && icon.at<Vec3b>(j + 25, i + 25)[1] == 0 && icon.at<Vec3b>(j + 25, i + 25)[2] == 0))
				{
					tar.at<Vec3b>(y * 2 + i, x * 2 + j)[0] = icon.at<Vec3b>(j + 25, i + 25)[1];
					tar.at<Vec3b>(y * 2 + i, x * 2 + j)[1] = icon.at<Vec3b>(j + 25, i + 25)[2];
					tar.at<Vec3b>(y * 2 + i, x * 2 + j)[2] = icon.at<Vec3b>(j + 25, i + 25)[0];
				}
			}
		}
	}
}

void point::paintRoute(Mat& tar, int col,int sig=0)
{
	if (outRoute != nullptr)
		outRoute->paint(tar, col,sig);
}

//map
myMap::myMap()
{
	xLength=0;
	yLength=0;
	target=nullptr;
}

myMap::myMap(CString cdir)
{
	string dir = CT2A(cdir.GetBuffer(cdir.GetLength()));
	Mat tar = imread(dir);
	Mat tar2(tar.rows/2, tar.cols/2, CV_8UC1);
	target = new mappoint* [tar.rows / 2+1];
	for (int i = 0; i < tar.rows / 2; i++)
		target[i] = new mappoint[tar.cols / 2+1];
	for (int i = 0; i < tar.rows-1; i+=2)
	{
		for (int j = 0; j < tar.cols-1; j+=2)
		{
			Vec3b pixel[4];
			pixel[0]= tar.at<Vec3b>(i, j);
			pixel[1] = tar.at<Vec3b>(i+1, j);
			pixel[2] = tar.at<Vec3b>(i, j+1);
			pixel[3] = tar.at<Vec3b>(i+1, j+1);
			int count = 0;
			for (int k = 0; k < 4; k++)
				if (pixel[k][0] >=238 && pixel[k][1] >= 238 && pixel[k][2] >= 238)
					count++;
			if (count >= 2)
			{
				target[i / 2][j / 2].aval = 1;
				tar2.at<uchar>(i / 2, j / 2) = 255;
			}
			else
			{
				target[i / 2][j / 2].aval = 0;
				tar2.at<uchar>(i / 2, j / 2) = 0;
			}
			target[i / 2][j / 2].elementThis=nullptr;
		}
	}
	xLength = tar.cols / 2;
	yLength = tar.rows / 2;
	imwrite("tmp.bmp", tar);
	imwrite("tmp2.bmp", tar2);
}

void myMap::set(int x, int y, element* val)
{
	target[y][x].elementThis = val;
}

void myMap::reset()
{
	for (int i = 0; i < yLength; i++)
		for(int j=0;j<xLength;j++)
			target[i][j].elementThis = nullptr;
}

int myMap::getState(int x, int y)
{
	if (target[y][x].elementThis == nullptr)
		return 0;//无
	if (target[y][x].elementThis->getPorR()==1)
		return 1;//路径
	if (target[y][x].elementThis->getPorR() == 0)
	{
		if (((point*)target[y][x].elementThis)->getX() == x && ((point*)target[y][x].elementThis)->getY() == y)
				return 2;//目标点
		else
			return 3;//普通点
	}
}

//dp状态
status::status()
{
	length = INT_MAX/2;
	end = 0;
	next = nullptr;
}

status::status(int length, int end,  status* next)
{
	this->length = length;
	this->end = end;
	this->next = next;
}

void status::setLength(int length)
{
	this->length = length;
}

void status::setPos(int end)
{
	this->end = end;
}

void status::setNext(status* next)
{
	this->next = next;
}

//全局函数

void insertRoute(int start, int end, vector<int>* xList, vector<int>* yList)
{
	routeList[chosenRoute][start][end] = new route(xList, yList,end);
}

void deleteRoute(int id)
{
	for (int i = 0; i < 16; i++)
	{
		if (routeList[chosenRoute][id][i] != nullptr)
		{
			delete routeList[chosenRoute][id][i];
			routeList[chosenRoute][id][i] = nullptr;
		}
		if (routeList[chosenRoute][i][id] != nullptr)
		{
			delete routeList[chosenRoute][i][id];
			routeList[chosenRoute][i][id] = nullptr;
		}
	}
}

int insertPoint(int x, int y)
{
	int i = 0;
	for (; i < 16; i++)
	{
		if (pointList[chosenRoute][i] == nullptr)
		{
			pointList[chosenRoute][i] = new point(x, y, i, chosenRoute);
			break;
		}
	}
	return i;
}

void insertStartPoint(int x, int y)
{
	pointList[chosenRoute][0] = new point(x, y, 0, chosenRoute);
}



struct bfsPoint
{
	int x;
	int y;
	int last;
};

struct bfsPoint bfsToPoint(int x, int y)
{
	bool** map = new bool*[target.getYLength()];
	for (int i = 0; i < target.getYLength(); i++)
		map[i] = new bool[target.getXLength()];
	vector<struct bfsPoint> bfsList;
	struct bfsPoint tmp{ x, y, 0 };
	bfsList.push_back(tmp);
	map[y][x] = 0;
	for (int i = 0; i < bfsList.size(); i++)
	{
		tmp = bfsList[i];
		if (target.getAval(tmp.x, tmp.y))
			break;
		if (tmp.y+1<target.getYLength()&&map[tmp.y + 1][tmp.x])
		{
			struct bfsPoint tmp1 { tmp.x, tmp.y +1, 0 };
			bfsList.push_back(tmp1);
			map[tmp.y +1][tmp.x] = 0;
		}
		if (tmp.x + 1 < target.getXLength() && map[tmp.y][tmp.x+1])
		{
			struct bfsPoint tmp1 { tmp.x+1, tmp.y, 0 };
			bfsList.push_back(tmp1);
			map[tmp.y][tmp.x+1] = 0;
		}
		if (tmp.y -1>=0&&map[tmp.y -1][tmp.x])
		{
			struct bfsPoint tmp1 { tmp.x, tmp.y -1, 0 };
			bfsList.push_back(tmp1);
			map[tmp.y -1][tmp.x] = 0;
		}
		if (tmp.x-1>=0&&map[tmp.y][tmp.x -1])
		{
			struct bfsPoint tmp1 { tmp.x-1, tmp.y, 0 };
			bfsList.push_back(tmp1);
			map[tmp.y][tmp.x -1] = 0;
		}
	}
	for (int i = 0; i < target.getYLength(); i++)
		delete[] map[i];
	delete[] map;
	return tmp;
}

void bfsToRoute(int x, int y,int id)
{
	bool** map = new bool*[target.getYLength()];
	for (int i = 0; i < target.getYLength(); i++)
		map[i] = new bool[target.getXLength()];
	vector<struct bfsPoint> bfsList;
	struct bfsPoint tmp { x, y, 0 };
	bfsList.push_back(tmp);
	map[y][x] = 0;
	for (int i = 0; i < bfsList.size(); i++)
	{
		tmp = bfsList[i];
		if (target.getState(tmp.x,tmp.y)==2&&target.getPointId(tmp.x,tmp.y)!=id)
		{
			vector<int> *xList,*rxList,*yList,*ryList;
			xList = new vector<int>;
			yList = new vector<int>;
			rxList = new vector<int>;
			ryList = new vector<int>;
			int ptr = i;
			while (bfsList[ptr].last != ptr)
			{
				xList->push_back(bfsList[ptr].x);
				yList->push_back(bfsList[ptr].y);
				rxList->push_back(bfsList[ptr].x);
				ryList->push_back(bfsList[ptr].y);
				ptr = bfsList[ptr].last;
			}
			insertRoute(target.getPointId(tmp.x, tmp.y), id, xList, yList);
			reverse(rxList->begin(), rxList->end());
			reverse(ryList->begin(), ryList->end());
			insertRoute(id, target.getPointId(tmp.x, tmp.y), rxList, ryList);
		}
		if (tmp.y + 1 < target.getYLength() && map[tmp.y + 1][tmp.x]&&target.getAval(tmp.x, tmp.y+1))
		{
			struct bfsPoint tmp1 { tmp.x, tmp.y+ 1, i };
			bfsList.push_back(tmp1);
			map[tmp.y+1][tmp.x] = 0;
		}
		if (tmp.x + 1 < target.getXLength() && map[tmp.y][tmp.x + 1] && target.getAval(tmp.x +1, tmp.y))
		{
			struct bfsPoint tmp1 { tmp.x + 1, tmp.y, i };
			bfsList.push_back(tmp1);
			map[tmp.y][tmp.x + 1] = 0;
		}
		if (tmp.y - 1 >= 0 && map[tmp.y - 1][tmp.x] && target.getAval(tmp.x, tmp.y -1))
		{
			struct bfsPoint tmp1 { tmp.x, tmp.y - 1, i };
			bfsList.push_back(tmp1);
			map[tmp.y - 1][tmp.x] = 0;
		}
		if (tmp.x - 1 >= 0 && map[tmp.y][tmp.x - 1] && target.getAval(tmp.x -1, tmp.y))
		{
			struct bfsPoint tmp1 { tmp.x - 1, tmp.y, i };
			bfsList.push_back(tmp1);
			map[tmp.y][tmp.x - 1] = 0;
		}
		if (bfsList.size() >= 50000)
			int a = 0;
	}
	for (int i = 0; i < target.getYLength(); i++)
		delete[] map[i];
	delete[] map;
}

status* dpMat[16][65535][16];


void dpForRoute()
{
	int minLength = INT_MAX/2, start = 0;
	vector<int> nextPointList;
	if (pointList[chosenRoute][0] == nullptr)
		return;
	pointList[chosenRoute][0]->giveOutRoute(nullptr);
	nextPointList.push_back(0);
	for(int i=1;i<16;i++)
	{
		if (pointList[chosenRoute][i] == nullptr||routeList[chosenRoute][start][i] == nullptr)
			continue;
		pointList[chosenRoute][i]->giveOutRoute(nullptr);
		nextPointList.push_back(i);
	}
	int num = nextPointList.size();
	int mapping[16];
	for (int i = 0; i < num; i++)
		mapping[i] = nextPointList[i];

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			for (int k = 1; k <= (int)pow(2, num); k++)
			{
				if (dpMat[i][k][j] != nullptr)
					delete dpMat[i][k][j];
			}
		}
	}
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			if (i == j)
				continue;
			if (routeList[chosenRoute][mapping[i]][mapping[j]] != nullptr)
				dpMat[i][0][j] = new status(routeList[chosenRoute][mapping[i]][mapping[j]]->getLength(), mapping[j], nullptr);
			for (int k = 1; k <= (int)pow(2, num); k++)
			{
				dpMat[i][k][j] = new status();
				dpMat[i][k][j]->setPos(mapping[j]);
			}
		}
	}
	for (int k = 1; k < (int)pow(2, num); k++)
	{
		for (int i = 0; i < num; i++)
		{
			for (int j = 0; j < num; j++)
			{
				if (i == j || (k&(int)pow(2,i)) != 0 || (k&(int)pow(2, j)) != 0)
					continue;
				for (int p = 0; (int)pow(2, p) <= k; p++)
				{
					if (p == i || p == j)
						continue;
					if ((dpMat[i][0][p] != nullptr&&dpMat[p][k - (int)pow(2, p)][j] != nullptr&&dpMat[i][k][j] != nullptr) && (dpMat[i][0][p]->getLength() + dpMat[p][k - (int)pow(2, p)][j]->getLength() < dpMat[i][k][j]->getLength()))
					{
						dpMat[i][k][j]->setLength(dpMat[i][0][p]->getLength() + dpMat[p][k - (int)pow(2, p)][j]->getLength());
						dpMat[i][k][j]->setNext(dpMat[p][k - (int)pow(2, p)][j]);
						dpMat[i][k][j]->setPos(dpMat[i][0][p]->getEnd());
					}
				}
			}
		}
	}
	status* res = new status;
	for (int i = 1; i < num; i++)
	{
		if (dpMat[0][(int)pow(2, num) - 2 - (int)pow(2, i)][i]->getLength() < res->getLength())
			res = dpMat[0][(int)pow(2, num) - 2 - (int)pow(2, i)][i];
	}
	int ptr = mapping[0];
	while (res->getNext() != nullptr)
	{
		pointList[chosenRoute][ptr]->giveOutRoute(routeList[chosenRoute][ptr][res->getEnd()]);
		ptr = res->getEnd();
		res = res->getNext();
	}
	if (res->getEnd() != 0)
	{
		pointList[chosenRoute][ptr]->giveOutRoute(routeList[chosenRoute][ptr][res->getEnd()]);
		pointList[chosenRoute][res->getEnd()]->giveOutRoute(nullptr);
	}
}

void deletePoint(int id)
{
	if (pointList[chosenRoute][id] != nullptr)
		delete pointList[chosenRoute][id];
	pointList[chosenRoute][id] = nullptr;
	deleteRoute(id);
	dpForRoute();
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CsearchMapDlg 对话框



CsearchMapDlg::CsearchMapDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SEARCHMAP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsearchMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CsearchMapDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CsearchMapDlg::OnBnClickedOk)
	ON_WM_DROPFILES()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_AS_32777, &CsearchMapDlg::OnAs32777)
	ON_COMMAND(ID_2_32775, &CsearchMapDlg::On232775)
	ON_COMMAND(ID_2_32776, &CsearchMapDlg::On232776)
	ON_COMMAND(ID_ASCSDTDR_32780, &CsearchMapDlg::OnAscsdtdr32780)
	ON_COMMAND(ID_ASCSDTDR_32781, &CsearchMapDlg::OnAscsdtdr32781)
	ON_COMMAND(ID_ASCSDTDR_32792, &CsearchMapDlg::OnAscsdtdr32792)
	ON_COMMAND(ID_ASCSDTDR_32793, &CsearchMapDlg::OnAscsdtdr32793)
	ON_COMMAND(ID_ASCSDR_32784, &CsearchMapDlg::OnAscsdr32784)
	ON_COMMAND(ID_ASCSDR_32785, &CsearchMapDlg::OnAscsdr32785)
	ON_COMMAND(ID_ASCSDR_32786, &CsearchMapDlg::OnAscsdr32786)
	ON_COMMAND(ID_ASCSSRDR_32787, &CsearchMapDlg::OnAscssrdr32787)
	ON_COMMAND(ID_ASCSSRDR_32788, &CsearchMapDlg::OnAscssrdr32788)
	ON_COMMAND(ID_ASCSSRDR_32789, &CsearchMapDlg::OnAscssrdr32789)
	ON_COMMAND(ID_ASCSSR_32794, &CsearchMapDlg::OnAscssr32794)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_ASCS_32796, &CsearchMapDlg::OnAscs32796)
END_MESSAGE_MAP()


// CsearchMapDlg 消息处理程序

BOOL CsearchMapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_tipBtnCtr.Create(this, TTS_ALWAYSTIP);
	m_tipBtnCtr.AddTool(this);
	icon=imread("星级.bmp");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CsearchMapDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CsearchMapDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CsearchMapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CsearchMapDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
	
}


void CsearchMapDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnDropFiles(hDropInfo);
	for (int i = 0; i < 16; i++)
		deletePoint(i);
	routeAval[1] = 0;
	WCHAR szPath[MAX_PATH] = L"";
	UINT nChars = ::DragQueryFile(hDropInfo, 0, szPath, MAX_PATH);
	CString str(szPath, nChars);
	m_strProjFileName = str;

	HBITMAP bitmap;
	bitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(),m_strProjFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	m_backBitmap.DeleteObject();
	if (!m_backBitmap.Attach(bitmap))
	{
		MessageBox(_T("导入背景图失败!"), _T("提示"), MB_OK);
		return;
	}
	CStatic *p = (CStatic *)GetDlgItem(IDC_STATIC);
	p->SetBitmap(m_backBitmap);
	target = myMap(m_strProjFileName);
	chosenRoute = 0; 
	
	CRect rectDlg;
	GetWindowRect(rectDlg);//获得窗体在屏幕上的位置

	CRect rectCtrl;
	p->GetWindowRect(rectCtrl);
	this->ScreenToClient(rectCtrl);
	MoveWindow(rectDlg.left, rectDlg.top, rectCtrl.Width()+60, rectCtrl.Height()+70);
	CStatic *p2 = (CStatic *)GetDlgItem(IDC_TEXT);
	p2->ShowWindow(SW_HIDE);
}

void CsearchMapDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnRButtonDown(nFlags, point);
	CMenu menu, *pSubMenu;
	CStatic *item = (CStatic *)GetDlgItem(IDC_STATIC);
	GetCursorPos(&p);
	item->ScreenToClient(&p);
	if (p.x / 2 < target.getXLength() && p.y / 2 < target.getYLength())
	{
		if (chosenRoute == 0 && target.getRouteId(p.x / 2, p.y / 2) == 0)
			menu.LoadMenu(IDR_MENU1);
		else if (target.getState(p.x / 2, p.y / 2) == 0)
			menu.LoadMenu(IDR_MENU2);
		else if (chosenRoute != target.getRouteId(p.x / 2, p.y / 2))
			menu.LoadMenu(IDR_MENU6);
		else if (target.getState(p.x / 2, p.y / 2) == 1 || target.getPointId(p.x / 2, p.y / 2) == 0)
			menu.LoadMenu(IDR_MENU5);
		else if (target.getState(p.x / 2, p.y / 2) == 2 || target.getState(p.x / 2, p.y / 2) == 3)
			menu.LoadMenu(IDR_MENU4);
		pSubMenu = menu.GetSubMenu(0);
		item->ClientToScreen(&p);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
		item->ScreenToClient(&p);
	}
}


int CsearchMapDlg::RePaint()
{
	// TODO: 在此处添加实现代码.
	string dir = CT2A(m_strProjFileName.GetBuffer(m_strProjFileName.GetLength()));
	Mat tar = imread(dir);
	target.reset();
	for (int j = 1; j < 13; j++)
	{
		if (routeAval[j])
		{
			for (int i = 0; i < 16; i++)
			{
				if (pointList[j][i] == nullptr)
					continue;
				pointList[j][i]->paintRoute(tar, j);
			}
		}
	}
	for (int j = 1; j < 13; j++)
	{
		if (routeAval[j])
		{
			for (int i = 0; i < 16; i++)
			{
				if (pointList[j][i] == nullptr)
					continue;
				pointList[j][i]->paint(tar,j);
			}
		}
	}
	imwrite("tmp.bmp",tar);
	HBITMAP bitmap;
	bitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(), _T("tmp.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	m_backBitmap.DeleteObject();
	if (!m_backBitmap.Attach(bitmap))
	{
		MessageBox(_T("导入背景图失败!"), _T("提示"), MB_OK);
		return 0;
	}
	CStatic *p = (CStatic *)GetDlgItem(IDC_STATIC);
	p->SetBitmap(m_backBitmap);
	return 0;
}


void CsearchMapDlg::OnAs32777()//添加起点
{
	// TODO: 在此添加命令处理程序代码
	int id = 0;
	for (int i = 1; i < 13; i++)
	{
		if (!routeAval[i])
		{
			routeAval[i] = 1;
			id = i;
			break;
		}

	}
	if (id == 0)
	{
		MessageBox(_T("不能再添加路径了!"), _T("提示"), MB_OK);
		return;
	}
	chosenRoute = id;
	bfsPoint tar = bfsToPoint(p.x / 2, p.y / 2);
	insertPoint(tar.x, tar.y);
	RePaint();
}


void CsearchMapDlg::On232775()
{
	// TODO: 在此添加命令处理程序代码
	OnAs32777();
}


void CsearchMapDlg::On232776()//更改起点
{
	// TODO: 在此添加命令处理程序代码
	deletePoint(0);
	bfsPoint tar = bfsToPoint(p.x / 2, p.y / 2);
	insertPoint(tar.x, tar.y);
	bfsToRoute(tar.x, tar.y, 0);
	dpForRoute();
	RePaint();
}


void CsearchMapDlg::OnAscsdtdr32780()
{
	// TODO: 在此添加命令处理程序代码
	OnAs32777();
}


void CsearchMapDlg::OnAscsdtdr32781()
{
	// TODO: 在此添加命令处理程序代码
	On232776();
}


void CsearchMapDlg::OnAscsdtdr32792()//删除目标点
{
	// TODO: 在此添加命令处理程序代码
	deletePoint(target.getPointId(p.x / 2, p.y / 2));
	RePaint();
}


void CsearchMapDlg::OnAscsdtdr32793()//删除路线
{
	// TODO: 在此添加命令处理程序代码
	for (int i = 0; i < 16; i++)
		deletePoint(i);
	chosenRoute = 0;
	routeAval[1] = 0;
	RePaint();
}


void CsearchMapDlg::OnAscsdr32784()
{
	// TODO: 在此添加命令处理程序代码
	OnAs32777();
}


void CsearchMapDlg::OnAscsdr32785()
{
	// TODO: 在此添加命令处理程序代码
	On232776();
}


void CsearchMapDlg::OnAscsdr32786()
{
	// TODO: 在此添加命令处理程序代码
	OnAscsdtdr32793();
}


void CsearchMapDlg::OnAscssrdr32787()
{
	// TODO: 在此添加命令处理程序代码
	OnAs32777();
}


void CsearchMapDlg::OnAscssrdr32788()
{
	// TODO: 在此添加命令处理程序代码
	On232776();
}



void CsearchMapDlg::OnAscssrdr32789()
{
	// TODO: 在此添加命令处理程序代码
	chosenRoute = target.getRouteId(p.x / 2, p.y / 2);
}


void CsearchMapDlg::OnAscssr32794()
{
	// TODO: 在此添加命令处理程序代码
	OnAscsdtdr32793();
}


void CsearchMapDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnLButtonDown(nFlags, point);
	CStatic *item = (CStatic *)GetDlgItem(IDC_STATIC);
	GetCursorPos(&p);
	item->ScreenToClient(&p);
	if (chosenRoute != 0&& p.x / 2 < target.getXLength() && p.y / 2 < target.getYLength())
	{
		bfsPoint tar = bfsToPoint(p.x / 2, p.y / 2);
		int id=insertPoint(tar.x, tar.y);
		if (id == 0)
		{
			MessageBox(_T("不能再加点了!"), _T("提示"), MB_OK);
			return;
		}
		bfsToRoute(tar.x, tar.y, id);
		dpForRoute();
		RePaint();
	}
}


BOOL CsearchMapDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此处添加实现代码.
	if (NULL != m_tipBtnCtr)
	{
		m_tipBtnCtr.RelayEvent(pMsg);
	}

	return CWnd::PreTranslateMessage(pMsg);
}


void CsearchMapDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnMouseMove(nFlags, point);
	CPoint po;
	CStatic *item = (CStatic *)GetDlgItem(IDC_STATIC);
	GetCursorPos(&po);
	item->ScreenToClient(&po);
	if (po.x / 2 >=0 && po.y / 2 >=0 && target.ready() && po.x / 2 < target.getXLength() && po.y / 2 < target.getYLength())
	{
		if (target.getState(po.x / 2, po.y / 2) == 2 || target.getState(po.x / 2, po.y / 2) == 3)
		{
			int ptr = 0,count=0,totalLength=0,lastLength=0;
			while (pointList[chosenRoute][ptr]->getId() != target.getPointId(po.x / 2, po.y / 2))
			{
				if (pointList[chosenRoute][ptr]->getNextPoint() == 0)
					break;
				totalLength += routeList[chosenRoute][ptr][pointList[chosenRoute][ptr]->getNextPoint()]->getLength();
				lastLength = routeList[chosenRoute][ptr][pointList[chosenRoute][ptr]->getNextPoint()]->getLength();
				ptr = pointList[chosenRoute][ptr]->getNextPoint();
				count++;
			}
			CString strTemp;
			strTemp.Format(_T("第%d个点 总路程：%d 到上一点路程：%d"), count,totalLength,lastLength);
			m_tipBtnCtr.UpdateTipText(strTemp, this);
		}
		else
			m_tipBtnCtr.UpdateTipText(_T(""), this);
	}
}

void CsearchMapDlg::OnAscs32796()
{
	// TODO: 在此添加命令处理程序代码
	string dir = CT2A(m_strProjFileName.GetBuffer(m_strProjFileName.GetLength()));
	Mat tar = imread(dir);
	target.reset();
	int ptr = 0;
	while (true)
	{
		pointList[chosenRoute][ptr]->paint(tar,1);
		imwrite("tmp.bmp", tar);
		HBITMAP bitmap;
		bitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(), _T("tmp.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		m_backBitmap.DeleteObject();
		if (!m_backBitmap.Attach(bitmap))
			MessageBox(_T("导入背景图失败!"), _T("提示"), MB_OK);
		CStatic *p = (CStatic *)GetDlgItem(IDC_STATIC);
		p->SetBitmap(m_backBitmap);
		Sleep(100);

		if (pointList[chosenRoute][ptr]->getNextPoint() != 0)
		{
			pointList[chosenRoute][ptr]->paintRoute(tar, 1,1);
			pointList[chosenRoute][ptr]->paintRoute(tar, 1,2);
			pointList[chosenRoute][ptr]->paintRoute(tar, 1,3);
			imwrite("tmp.bmp", tar);
			HBITMAP bitmap;
			bitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(), _T("tmp.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			m_backBitmap.DeleteObject();
			if (!m_backBitmap.Attach(bitmap))
				MessageBox(_T("导入背景图失败!"), _T("提示"), MB_OK);
			CStatic *p = (CStatic *)GetDlgItem(IDC_STATIC);
			p->SetBitmap(m_backBitmap);
			Sleep(100);
		}

		

		if (pointList[chosenRoute][ptr]->getNextPoint() == 0)
			break;
		else
			ptr = pointList[chosenRoute][ptr]->getNextPoint();
		RePaint();
	}
}
