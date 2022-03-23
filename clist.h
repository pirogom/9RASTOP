#ifndef __CLIST_H__
#define __CLIST_H__


#ifndef SAFE_FREE 
#define SAFE_FREE( mem )	{ free(mem); mem = NULL; }
#endif

#define INSERT_TO_LIST( head, item, prev, next )			\
{															\
	item->next = head;										\
	if( head )												\
		head->prev = item;									\
	head = item;											\
}

#define REMOVE_FROM_LIST( head, item, prev, next )			\
{															\
	if( item == head )										\
		head = item->next;									\
	if( (item)->prev )										\
		(item)->prev->next = (item)->next;					\
	if( (item)->next )										\
		(item)->next->prev = (item)->prev;					\
	(item)->prev = NULL;									\
	(item)->next = NULL;									\
}													


#define LIST_WHILE( list, item, temp_d, next )				\
{															\
for( item = list; item; item = temp_d )						\
{															\
	temp_d = item->next;									\



#define LIST_WHILEEND()		}}
#define LIST_SKIP()			{continue;}	

/*
 *	 for GarbageCollection
 */
#define GET_MEMORY( list, data, type, prev, next )		\
{data = NULL;											\
if( list )												\
{														\
	data = list;										\
	REMOVE_FROM_LIST( list, data, prev, next );			\
}														\
else													\
{														\
	data = (type*)malloc(sizeof(type));					\
														\
	if( !data )											\
		data =NULL;										\
}}													


/*
 *	CList class
 */
struct sLIST_BLOCK
{
	void *data;

	sLIST_BLOCK *prev;
	sLIST_BLOCK *next;
};

typedef struct sLIST_BLOCK *sPLIST_BLOCK;

namespace MYLIST
{

class CList
{
private:
	int m_totalCnt;									// 총 데이터 갯수
	int m_totalMemory;								// 총 메모리 갯수

public:
	CList();										// 생성자
	~CList();										// 파괴자

	void push( void *data );						// 데이터 넣기 
	void *pop();									// 데이터 빼기
	sLIST_BLOCK *FindData( void *data );			// 데이터 찾기
	sLIST_BLOCK *CreateBlock();						// sLIST_BLOCK 데이터 만들기
	void *GetLastData();
	void *GetFirstData() { return m_link->data; };
	sLIST_BLOCK *GetLastBlock();
	sLIST_BLOCK *GetFirstBlock() { return m_link; };
	void remove( void *data );						// 데이터 지우기
	void remove( sPLIST_BLOCK block ); 
	void Destroy();	
	void DestroyWithData();
	void Init();			
	void clear();									// 리스트 비우기
	int GetSize() { return m_totalCnt; };			// 총 데이터 갯수
	int GetMemSize() { return m_totalMemory; };		// 총 메모리 상의 데아터 갯수

	sLIST_BLOCK			*m_link;					// 실질 리스트 데이터
	sLIST_BLOCK			*m_memory;					// sLIST_BLOCK 데이터 메모리 링커
};


}


#endif


