///////////////////////////////////////////////////
// listlong.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __CLS_LIST_LONG___
#define __CLS_LIST_LONG___

#pragma pack(push) //NADA
#pragma pack(2)

template <class T>
struct TListLong {
	struct TListLongEntry {
		TListLongEntry()
		{
		}
		void Init(TListLongEntry *a)
		{
			m_pNext = m_pPrev = 0;
			if (a)
			{
				m_pPrev = a;
				a->m_pNext = this;	
			}
		}
		void Reset() 
		{
			if (m_pPrev)
				m_pPrev->m_pNext = m_pNext;
			if (m_pNext)
				m_pNext->m_pPrev = m_pPrev;
				m_pNext = m_pPrev = 0;
		}
		void Init(T _val, TListLongEntry *_prev = 0, TListLongEntry *_next = 0)
		{
			m_iData = _val;
			m_pNext = _next;
			m_pPrev = _prev;

		} 
		union {
		TListLongEntry *m_pPrev;
		TListLongEntry *prev;
		};
		union {
		TListLongEntry *m_pNext;
		TListLongEntry *next;
		};
		union {
		T m_iData;
		T val;
		};
		
	};

	TListLong():m_pFirst(0),m_pLast(0),m_iNum(0) {};
	~TListLong() {  }

	T &Add(T a, TListLongEntry *entry)
	{					
		entry->Init(m_pLast);
		entry->m_iData = a;
		if (!m_pFirst) m_pFirst = entry;
		m_pLast = entry;
		++m_iNum;
		return entry->m_iData;
	}
	T &AddFirst(TListLongEntry *entry)
	{
		entry->prev = 0;
		entry->next = m_pFirst;
		m_pFirst = entry;
		if (entry->next)
			entry->next->prev = entry;
		else
			m_pLast = entry;
		++m_iNum;
		return entry->m_iData;
	}

	T &Add(TListLongEntry *entry)
	{
		entry->Init(m_pLast);
		if (!m_pFirst) m_pFirst = entry;
		m_pLast = entry;
		++m_iNum;
		return entry->m_iData;
	}
	void Reset() {m_pFirst = m_pLast = 0; m_iNum = 0; }
	void MoveTo(TListLongEntry *entry, TListLong *dst)
	{
		TListLongEntry *tmp = entry;
			if (m_pLast == tmp)
				m_pLast = tmp->m_pPrev;

			if (m_pFirst == tmp)
				m_pFirst = tmp->m_pNext;
			tmp->Reset();
			--m_iNum;
	
			dst->Add(tmp);
		}
	void MoveTo(TListLong *dst)
	{
		TListLongEntry *tmp, *entry = first;
		while(entry)
		{
			tmp = entry->next;
			MoveTo(entry, dst);
			entry = tmp;
		}
	}

	void Del(TListLongEntry *entry)
	{
		TListLongEntry *tmp = entry;
		if (last == tmp) last = tmp->prev;
		if (first == tmp) first = tmp->next;
		tmp->Reset();
		--m_iNum;
	}
	union {		
	TListLongEntry *m_pFirst;
	TListLongEntry *first;
	};
	union {		
	TListLongEntry *m_pLast;
	TListLongEntry *last;
	};
	union {		
		DINT num;
		DINT m_iNum;
	};
};
typedef TListLong<int> MListLong;
typedef TListLong<int>::TListLongEntry MListLongEntry;

#pragma pack(pop)

#endif // !defined(AFX_LISTLONG_H__FE4203AC_7D00_4221_A86C_B6EAD178B58F__INCLUDED_)
