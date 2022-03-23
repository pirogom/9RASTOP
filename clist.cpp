/*
 *	Filename : 			clist.cpp
 *	Description :		링크드리스트	
 */


#include "StdAfx.h"
#include "clist.h"

namespace MYLIST
{

/* 
 *	GetLastBlock
 */
sLIST_BLOCK *CList::GetLastBlock()
{
	sLIST_BLOCK *b, *last_b = NULL;
	sLIST_BLOCK *temp_b;

	LIST_WHILE( m_link, b, temp_b, next );
	last_b = b;
	LIST_WHILEEND();

	return last_b;
}

/*
 *	GetLastData
 */
void *CList::GetLastData()
{
	sLIST_BLOCK *b, *last_b = NULL;
	sLIST_BLOCK *temp_b;

	LIST_WHILE( m_link, b, temp_b, next );
	last_b = b;
	LIST_WHILEEND();

	return last_b->data;
}


/*
 *	pop
 */
void *CList::pop()
{
	void *popData;

	if( m_link )
	{
		popData = m_link;
		remove( popData );
	}
	else
		return NULL;
	return popData;
}

/*
 *	Destroy
 */
void CList::Destroy()
{
	sPLIST_BLOCK b, next_b;
	
	LIST_WHILE( m_link, b, next_b, next );	
	b->data = NULL;
	SAFE_FREE( b );
	LIST_WHILEEND();

	LIST_WHILE( m_memory, b, next_b , next );
	b->data = NULL;
	SAFE_FREE( b );
	LIST_WHILEEND();

	m_link = NULL;
	m_memory = NULL;
	m_totalCnt = 0;
	m_totalMemory = 0;
}

/*
 *	DestroyWithData
 */
void CList::DestroyWithData()
{
	sPLIST_BLOCK b, next_b;
	
	LIST_WHILE( m_link, b, next_b, next );	
	SAFE_FREE( b->data );
	SAFE_FREE( b );
	LIST_WHILEEND();

	LIST_WHILE( m_memory, b, next_b , next );
	b->data = NULL;
	SAFE_FREE( b );
	LIST_WHILEEND();

	m_link = NULL;
	m_memory = NULL;
	m_totalCnt = 0;
	m_totalMemory = 0;
}

/*
 *	Init
 */
void CList::Init()
{
	m_totalCnt = 0;
	m_totalMemory = 0;

	m_link = NULL;
	m_memory = NULL;
}

/*
 *	clear
 */
void CList::clear()
{
	sPLIST_BLOCK b, next_b;

	LIST_WHILE( m_link, b, next_b , next );
	SAFE_FREE( b->data );
	REMOVE_FROM_LIST( m_link, b, prev, next );
	INSERT_TO_LIST( m_memory, b, prev, next );
		
	m_totalCnt--;
	m_totalMemory++;
	LIST_WHILEEND();
}

/*
 *	FindData
 */
sPLIST_BLOCK CList::FindData( void *data )
{
	sPLIST_BLOCK b, temp_b;

	LIST_WHILE( m_link, b, temp_b, next );
	if( b->data == data )
		return b;
	LIST_WHILEEND();

	return NULL;
}

/*
 *	remove
 */
void CList::remove( void *data )
{
	sLIST_BLOCK *find = FindData( data );

	if( !find )
		return;

	m_totalCnt--;
	m_totalMemory++;	

	REMOVE_FROM_LIST( m_link, find, prev, next );
	INSERT_TO_LIST( m_memory, find, prev, next );
}


/*
 *	remove
 */
void CList::remove( sPLIST_BLOCK block )
{
	m_totalCnt--;
	m_totalMemory++;

	REMOVE_FROM_LIST( m_link, block, prev, next );
	INSERT_TO_LIST( m_memory, block, prev, next );
}

/*
 *	CreateBlock
 */
sLIST_BLOCK *CList::CreateBlock()
{
	sPLIST_BLOCK data;

	if( m_memory )
	{
		data = m_memory;
		REMOVE_FROM_LIST( m_memory, data, prev, next );
		m_totalMemory--;
		return data;
	}
	else
	{
		data = (sPLIST_BLOCK)malloc( sizeof(sLIST_BLOCK) );

		if( !data )
			return NULL;
		else
		{
			data->data = NULL;
			data->next = NULL;
			data->prev = NULL;
			return data;
		}
	}
	return NULL;    
}

/*
 *	push
 */
void CList::push( void *data )
{
	sPLIST_BLOCK tmpData = CreateBlock();

	if( !tmpData )
		return;

	tmpData->data = data;
	m_totalCnt++;

	INSERT_TO_LIST( m_link, tmpData, prev, next );
}

/*
 *	CList
 */
CList::CList()				
{
	m_totalCnt = 0;
	m_totalMemory = 0;

	m_link = NULL;
	m_memory = NULL;
}

/*
 *	~CList
 */
CList::~CList()				
{
	Destroy();
}

}




//



