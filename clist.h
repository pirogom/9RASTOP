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
	int m_totalCnt;									// �� ������ ����
	int m_totalMemory;								// �� �޸� ����

public:
	CList();										// ������
	~CList();										// �ı���

	void push( void *data );						// ������ �ֱ� 
	void *pop();									// ������ ����
	sLIST_BLOCK *FindData( void *data );			// ������ ã��
	sLIST_BLOCK *CreateBlock();						// sLIST_BLOCK ������ �����
	void *GetLastData();
	void *GetFirstData() { return m_link->data; };
	sLIST_BLOCK *GetLastBlock();
	sLIST_BLOCK *GetFirstBlock() { return m_link; };
	void remove( void *data );						// ������ �����
	void remove( sPLIST_BLOCK block ); 
	void Destroy();	
	void DestroyWithData();
	void Init();			
	void clear();									// ����Ʈ ����
	int GetSize() { return m_totalCnt; };			// �� ������ ����
	int GetMemSize() { return m_totalMemory; };		// �� �޸� ���� ������ ����

	sLIST_BLOCK			*m_link;					// ���� ����Ʈ ������
	sLIST_BLOCK			*m_memory;					// sLIST_BLOCK ������ �޸� ��Ŀ
};


}


#endif


