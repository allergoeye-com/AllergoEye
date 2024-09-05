#include "stdafx.h"
#include "RuningPolygon.h"
#include "ArRBMap.h"
//#define __TEST_


//--------------------------------------------------------------
//
//--------------------------------------------------------------
bool RuningPoygon::Init(AllocatorLine *pMyAlloc, Vector2D &c, int _iIndex, Box2D &_box)
{
	
	iIndex = _iIndex;
	list.SetAlocator(pMyAlloc, pMyAlloc == 0);
	Reset();
	
	box.center = c;
	LineItem &left = list.PushBack();
	_box.Line(Line2D::PL_LEFT, left);
	left.iIndex = -1 - Line2D::PL_LEFT;

	LineItem &top = list.PushBack();
	_box.Line(Line2D::PL_TOP, top);
	top.iIndex = -1 - Line2D::PL_TOP;

	LineItem &right = list.PushBack();
	_box.Line(Line2D::PL_RIGHT, right);
	right.iIndex = -1 - Line2D::PL_RIGHT;

	LineItem &bottom = list.PushBack();
	_box.Line(Line2D::PL_BOTTOM, bottom);
	bottom.iIndex = -1 - Line2D::PL_BOTTOM;

	SetBox();
	return true;
}
bool RuningPoygon::Init(IPoint *p, int len)
{
	if (p[0] == p[len - 1])
		--len;
	Vector2D c;
	c = ((POINT *)p)[0];
	int N = 1;
	LineItem *prev = 0;
	for (int i = 0; i < len;)
	{

		Vector2D dest, org = ((POINT *)p)[i];
		++i;
		while (org == ((POINT *)p)[i] && i < len)
			++i;
		dest = i == len ? ((POINT *)p)[0] : ((POINT *)p)[i];

		if (org == dest)
		{
			if (i != len)
			{
				--i;
				continue;
			}

		}
		if (prev)
		{
			if (prev->Orient(org, dest) == Line2D::COLLINEAR)
			{
				prev->dest = dest;
				continue;
			}

		}
		LineItem &tmp = list.PushBack();
		tmp.org = org;
		tmp.dest = dest;
		tmp.iIndex = i;
		c += dest;
		prev = &tmp;
		++N;
	}
	c /= N;
	box.center = c;
	SetBox();
	return true;
}
void RuningPoygon::Clip(Box2D &_box)
{
	Line2D d;
	_box.Line(Line2D::PL_LEFT, d);
	FlipLine(d);
	ClipLines(d,  -1 - Line2D::PL_LEFT);

	_box.Line(Line2D::PL_TOP, d);
	FlipLine(d);
	ClipLines(d,  -1 - Line2D::PL_TOP);

	_box.Line(Line2D::PL_RIGHT, d);
	FlipLine(d);
	ClipLines(d,  -1 - Line2D::PL_RIGHT);

	_box.Line(Line2D::PL_RIGHT, d);
	FlipLine(d);
	ClipLines(d,  -1 - Line2D::PL_BOTTOM);
}
//-----------------------------------------------------
//
//-----------------------------------------------------
void RuningPoygon::GetVertex(MArray<Vector2D> &res)
{
	EntryLine *node = (EntryLine *)list.GetFirstPos();
	while(node)
	{
		res.Add() = node->value.org; 
		if (!node->next)
			res.Add() = node->value.dest; 
		node = node->next;
	}

}
//-----------------------------------------------------
//
//-----------------------------------------------------
bool RuningPoygon::IsClosed()
{
	EntryLine *node = (EntryLine *)list.GetFirstPos();
	Vector2D v;
	while(node)
	{
		if (node->prev)
			if (node->value.org != v)
				return false; 
		v = node->value.dest; 
		node = node->next;
	}
	node = (EntryLine *)list.GetFirstPos();
	if (node->value.org != v)
		return false; 
	return true;		
	
}


//-----------------------------------------------------
//
//-----------------------------------------------------
double RuningPoygon::Square()
{
	EntryLine *node = (EntryLine *)list.GetFirstPos();
	
	double res = 0;
	while(node)
	{
		double a = (node->value.dest - node->value.org).Len();
		double h = node->value.PerpDistance(box.center);
		res += a * h * 0.5;
		node = node->next;
	}
	return res;
}

//---------------------------------------
//
//---------------------------------------
void RuningPoygon::SetBox()
{

	ListLine::iterator it(list);
	double Q = 0;
	box.radiusQ = 0;
	for (LineItem *p = it.begin(); p; p = it.next())
	{
		FlipLine(*p);
		if ((Q = (p->org - box.center).LenQ()) > box.radiusQ)
			box.radiusQ = Q;
		if ((Q = (p->dest - box.center).LenQ()) > box.radiusQ)
			box.radiusQ = Q;
	}
	box.radius = sqrt(box.radiusQ);	
}
bool RuningPoygon::IsLeft(RuningPoygon *pe)
{
	EntryLine *node = (EntryLine *)list.GetFirstPos();
	while(node)
	{
		if (node->value.iIndex == -1 - Line2D::PL_LEFT)
			return true;
		if (node->value.iIndex >= 0)
			if (pe[node->value.iIndex].IsLeft())
				return true;
		
		node = node->next;
	}
	return false;
}
bool RuningPoygon::IsRight(RuningPoygon *pe)
{
	EntryLine *node = (EntryLine *)list.GetFirstPos();
	while(node)
	{
		if (node->value.iIndex == -1 - Line2D::PL_RIGHT)
			return true;
		if (node->value.iIndex >= 0)
			if (pe[node->value.iIndex].IsRight())
				return true;
	
		node = node->next;
	}
	return false;
}
bool RuningPoygon::IsLeft()
{
	EntryLine *node = (EntryLine *)list.GetFirstPos();
	while(node)
	{
		if (node->value.iIndex == -1 - Line2D::PL_LEFT)
			return true;
		
		node = node->next;
	}
	return false;
}
bool RuningPoygon::IsRight()
{
	EntryLine *node = (EntryLine *)list.GetFirstPos();
	while(node)
	{
		if (node->value.iIndex == -1 - Line2D::PL_RIGHT)
			return true;
	
		node = node->next;
	}
	return false;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool RuningPoygon::IsClipLines(Line2D &p, int _iIndex, DecPos pos[2], Vector2D n[2])
{
	FlipLine(p);
	Vector2D pt;
	double t;
	Line2D::ORIENT cross;
	EntryLine *node = list.GetPtr();
	int i = 0;
	double tt = 0.0;
	while(node)
	{
		if (node->value.iIndex == _iIndex)
			return 0;
		node = node->next;
	}
	node = list.GetPtr();
	while(node)
	{
		if ((cross = node->value.Intersect(p, t)) == Line2D::COLLINEAR)
			return 0;
		if (cross == Line2D::CROSS)
		{
			if (t > -1.0e-14 && t < 1.0 + 1.0e-14)
			{
				if (t < 1.0e-8)
				  t = 0.0;
				  
				if (t > 1.0 || 1.0 - t < 1.0e-8)
					t = 1.0;   
				pt = node->value.Point(t);
				if (fabs (pt.x) < PRECISION) 
					pt.x = 0.0;
				if (fabs (pt.y) < PRECISION) 
					pt.y = 0.0;
				if (!i)
				{
					tt = t; 
					n[i] = pt;
					pos[i] = node;
					++i;
				}
				else
				if (n[i - 1] != pt)
				{
					n[i] = pt;
					pos[i] = node;
					LineItem s(n[0], n[1], _iIndex);
					if (s.Dir(box.center) == Line2D::BACK)
					{
						DecPos p = pos[0];
						pos[0] = pos[1];
						pos[1] = p;
						Vector2D v = n[0];
						n[0] = n[1];
						n[1] = v;
						 
					}	
					return true;
				}
				else
				if (tt == 1.0)
				{
					n[0] = pt;
					pos[0] = node;

				}
				
			}
		}
		node = node->next;
	}
	return false;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
bool RuningPoygon::IsClipXRay(Segment2D &p,  Vector2D n[2])
{
	Vector2D pt;
	double t;
	if (!box.IsClip(p.org, p.dest)) return 0;
	Line2D::ORIENT cross;
	EntryLine *node = list.GetPtr();
	int i = 0;
	double tt = 0.0;
	while(node)
	{
		if ((node->value.org.x > p.dest.x && node->value.dest.x > p.dest.x) ||
			(node->value.org.x < p.org.x && node->value.dest.x < p.org.x) ||
			(node->value.org.y > p.dest.y && node->value.dest.y > p.dest.y) ||
			(node->value.org.y < p.org.y && node->value.dest.y < p.org.y)
		)
		{
			node = node->next;
				continue;
		}
		if ((cross = node->value.Intersect(p, t)) == Line2D::COLLINEAR)
		{
			if (node->value.org.x > node->value.dest.x)
			{
				n[1] = node->value.org;
				n[0] = node->value.dest;
			}
			else
			{
				n[0] = node->value.org;
				n[1] = node->value.dest;
			}
			goto ret_ok;
		}
		if (cross == Line2D::CROSS)
		{
			if (t > -1.0e-14 && t < 1.0 + 1.0e-14)
			{
				if (t < 1.0e-8)
				  t = 0.0;

				if (t > 1.0 || 1.0 - t < 1.0e-8)
					t = 1.0;
				pt = node->value.Point(t);
				if (fabs (pt.x) < PRECISION)
					pt.x = 0.0;
				if (fabs (pt.y) < PRECISION)
					pt.y = 0.0;
				if (!i)
				{
					tt = t;
					n[i] = pt;
					++i;
				}
				else
				if (n[i - 1] != pt)
				{
					n[i] = pt;
					goto ret_ok;
				}
				else
				if (tt == 1.0)
				{
					n[0] = pt;


				}

			}
		}
		node = node->next;
	}
	if (i == 1)
	{
		if (n[0] != p.org && PtIn(p.org))
		{
			n[1] = n[0];
			n[0] = p.org;
		}
		else
		if (PtIn(p.dest))
			n[1] = p.dest;
		else
			n[1] = n[0];
	}
	else
	if (PtIn(p.dest) && PtIn(p.org))
	{
		n[1] = p.dest;
		n[0] = p.org;
	}
	else
	return false;
ret_ok:
	if (n[0].x > n[1].x)
	{
		pt = n[0];
		n[0] = n[1];
		n[1] = pt;
	}
	n[0].x = max(n[0].x, p.org.x);
	n[1].x = min(n[1].x, p.dest.x);
	return true;

}
//-----------------------------------------------------
//
//-----------------------------------------------------
bool RuningPoygon::ClipLines(Line2D &p, int _iIndex)
{
	DecPos pos[2];
	Vector2D n[2];
	if (IsClipLines(p, _iIndex, pos, n))
		return ClipLines(_iIndex, pos, n);
	return 0;
}

bool RuningPoygon::CheckLink(int _iIndex)
{
	EntryLine *node = list.GetPtr();
	while(node)
	{
		if (node->value.iIndex == _iIndex)
			return true;
		node = node->next;
	}
	return 0;
}
bool RuningPoygon::AddLineToPoint(Line2D &p, int _iIndex)
{
	FlipLine(p);
	Vector2D pt;
	double t;
	Line2D::ORIENT cross;
	EntryLine *node = list.GetPtr();
	int i = 0;

	while(node)
	{
		if (node->value.iIndex == _iIndex)
			return 1;
		node = node->next;
	}
	EntryLine *pos[2];
	Vector2D n[2];
	node = list.GetPtr();
	double tt[2];
	bool fOk = 0;
	while(!fOk && node)
	{
		if ((cross = node->value.Intersect(p, t)) == Line2D::COLLINEAR)
		{
			
			
				tt[i] = i ? 0.0 : 1.0; 
				n[i] = i ? node->value.org : node->value.dest; 
				pos[i] = node;
			if (i)
				fOk = true;	
			
		}
		if (cross == Line2D::CROSS)
		{
			if (t > -PRECISION && t < 1.0 + PRECISION)
			{
				if (t < 1.0e-8)
				  t = 0.0;
				  
				if (t > 1.0 || 1.0 - t < 1.0e-8)
					t = 1.0;   
				pt = node->value.Point(t);
				if (fabs (pt.x) < PRECISION) 
					pt.x = 0.0;
				if (fabs (pt.y) < PRECISION) 
					pt.y = 0.0;
				if (!i)
				{
					tt[i] = t; 
					n[i] = pt;
					pos[i] = node;
					
					++i;
				}
				else
				{
					tt[i] = t; 
					n[i] = pt;
					pos[i] = node;
					LineItem s(n[0], n[1], _iIndex);
					if (s.Dir(box.center) == Line2D::BACK)
					{
						EntryLine *p = pos[0];
						pos[0] = pos[1];
						pos[1] = p;
						Vector2D v = n[0];
						n[0] = n[1];
						n[1] = v;
						 
					}	
					fOk = true;
				}
				
			}
		}
		node = node->next;
	}
	if (fOk)
	{
		if ((tt[0] == 1.0 && tt[1] == 0.0) || (tt[1] == 1.0 && tt[0] == 0.0))
		{
			LineItem s(n[0], n[1], _iIndex);
			list.InsertAfter(pos[0], s);
			return true;

		}
	}
	return false;
}
bool RuningPoygon::CheckCenter () 
{
	EntryLine *node = list.GetPtr();
	Vector2D tst(0.0, 0.0);
	
	while(node)
	{
		tst += node->value.org; 
		if(node->value.Dir(box.center) != Line2D::FRONT)
			return false;
		node = node->next;
	}
	tst /= (double)list.Len();
	node = list.GetPtr();
	while(node)
	{
		if(node->value.Dir(tst) != Line2D::FRONT)
			return false;
		node = node->next;
	}
	return true;

}

//---------------------------------------
//
//---------------------------------------
bool RuningPoygon::ClipLines(int _iIndex, DecPos pos[2], Vector2D n[2], bool fInStack)
{
	EntryLine *node;
	LineItem s(n[0], n[1], _iIndex);

	FlipLine(s);
	bool fOrg, fDest;
	node = (EntryLine *)pos[0];
	fOrg = s.Dir(node->value.org) == Line2D::BACK;
	fDest = s.Dir(node->value.dest) == Line2D::BACK;
#ifdef __TEST_
	ListLine tmp = list;
#endif
	bool fInsert = true;
	if (!fOrg && !fDest)
	{
		if (s.org == node->value.dest)
			list.InsertAfter(pos[0], s);
		else
			node->value = s;
	}
	else
	if (fDest)
	{
		if (node->value.org == s.org)
		{
			node->value = s;
		}
		else
		{
			node->value.dest = s.org;
			list.InsertAfter(pos[0], s);
		}
	}
	else
	if (fOrg)
	{
		node->value.org = s.org;
		if (!node->prev)
			list.PushFront(s);
		else
			list.InsertBefore(pos[0], s);
	}
	else
	{
		DecPos pos1[2];
		 Vector2D n1[2];
		Line2D p(n[0], n[1]);
		IsClipLines(p, _iIndex, pos1, n1);
		return 0;
	}
	node = (EntryLine *)pos[1];
	
	fOrg = s.Dir(node->value.org) == Line2D::BACK;
	fDest = s.Dir(node->value.dest) == Line2D::BACK;
	if (node->value.org != s.dest)
	if (!fOrg || !fDest)
	{
		if (fDest)
			node->value.dest = s.dest;
		if (fOrg)
		{
			if (node->value.dest == s.dest)
			{
				list.Remove(node);
			}
			else
			node->value.org = s.dest;
		}
	}

	node = (EntryLine *)list.GetFirstPos();
	while(node)
	{
		fOrg = s.Dir(node->value.org) == Line2D::BACK;
		fDest = s.Dir(node->value.dest) == Line2D::BACK;
#ifdef __TEST_
		if(node->value.org == node->value.dest)
		{
		
		list = tmp;
			DecPos pos1[2];
			 Vector2D n1[2];
			Line2D p(n[0], n[1]);
			if (IsClipLines(p, _iIndex, pos1, n1))
				return ClipLines(_iIndex, pos1, n1, true); 
			return 0;
		}
#endif
		
		if ((fOrg && fDest) || (fOrg && (node->value.dest == s.dest || node->value.dest == s.org)) ||
		(fDest && (node->value.org == s.dest || node->value.org == s.org)) || node->value.org == node->value.dest)
		{
		
			list.Remove(node);
			node = (EntryLine *)list.GetFirstPos();
			continue;
		}
		node = node->next;
	}
#ifdef __TEST_
	if (!CheckCenter ())
	{
		list = tmp;
		if (fInStack)
		{
	/*	DecPos pos1[2];
		 Vector2D n1[2];
		Line2D p(n[0], n[1]);
		if (IsClipLines(p, _iIndex, pos1, n1))
			ClipLines(_iIndex, pos1, n1, true); 
		else 
	*/		return false;
		}
		DecPos pos1[2];
		 Vector2D n1[2];
		Line2D p(n[0], n[1]);
		if (IsClipLines(p, _iIndex, pos1, n1))
			ClipLines(_iIndex, pos1, n1, true); 
		else 
			return false;
	
	}
	if (!IsClosed())
	{
		list = tmp;
		if (fInStack)
		{
	/*	DecPos pos1[2];
		 Vector2D n1[2];
		Line2D p(n[0], n[1]);
		if (IsClipLines(p, _iIndex, pos1, n1))
			ClipLines(_iIndex, pos1, n1, true); 
		else 
	*/		return false;
		}
		DecPos pos1[2];
		 Vector2D n1[2];
		Line2D p(n[0], n[1]);
		if (IsClipLines(p, _iIndex, pos1, n1))
			ClipLines(_iIndex, pos1, n1, true); 
		else 
			return false;
	}
{
	node = (EntryLine *)list.GetFirstPos();
	EntryLine *node1 = node;
	bool fOk = true;
	while(node)
	{
		if (node->next)
		{
			if (node->value.Sgn(node->next->value) > 0)
			{
				fOk = false;
				//break;
			}
		}
		else
			if (node->value.Sgn(node1->value) > 0)
			{
				fOk = false;
				//break;
			}
		node = node->next;
	}
	if (!fOk)
	{
		list = tmp;
		if (fInStack)
		{
	/*	DecPos pos1[2];
		 Vector2D n1[2];
		Line2D p(n[0], n[1]);
		if (IsClipLines(p, _iIndex, pos1, n1))
			ClipLines(_iIndex, pos1, n1, true); 
		else 
	*/		return false;
		}
		DecPos pos1[2];
		 Vector2D n1[2];
		Line2D p(n[0], n[1]);
		if (IsClipLines(p, _iIndex, pos1, n1))
			ClipLines(_iIndex, pos1, n1, true); 
		else 
			return false;
	}
	
}
#endif			
	SetBox();
	return true;
}

//----------------------------------------------
//
//----------------------------------------------
void RuningPoygon::FlipLine(Line2D &p)
{
	if (p.n.x * box.center.x + p.n.y * box.center.y + p.dist < -PRECISION)
		p.Flip();
}
//----------------------------------------------
//
//----------------------------------------------
bool RuningPoygon::PtIn(Vector2D &v)
{
	EntryLine *node = list.GetPtr();
	while(node)
	{
		if(node->value.Dir(v) == Line2D::BACK)
			return false;
		node = node->next;
	}
   return true;
}

//----------------------------------------------
//
//----------------------------------------------
void RuningPoygon::FlipLine(Segment2D &p)
{
	
	if (p.Dir(box.center) == Line2D::BACK)
		p.Flip();
}
