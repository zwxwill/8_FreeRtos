/*
**  test list
*/
#include "test_list.h"
#include "srv_list.h"

static int VarIndexInStruct = 0;

typedef struct TestList
{
	char charVar;
	int intVar;
	short shortVar;
	struct ListHead list;
}TestList_t;

typedef struct VarElement
{
	struct ListHead node;
	int num;
}VarElement_t;
VarElement_t Var1, Var2, Var3;



typedef struct MasterList
{
	struct ListHead node;
	struct ListHead Varlist;
}MasterList_t;
MasterList_t Master;


void test_list(void)
{
	struct ListHead *temp, *n;
	struct VarElement *Var;
	
	VarIndexInStruct = offsetof(TestList_t, charVar);
	VarIndexInStruct = offsetof(TestList_t, intVar);
	VarIndexInStruct = offsetof(TestList_t, shortVar);
//	VarIndexInStruct = (TestList_t*)((unsigned char*)(l) - offsetof(TestList_t, intVar));


	INIT_PLCLIST(&(Master.Varlist));
	Var1.num = 1;
	Var2.num = 2;
	Var3.num = 3;
	PLCLIST_INSERT_TAIL(&(Master.Varlist), &(Var1.node));
	PLCLIST_INSERT_TAIL(&(Master.Varlist), &(Var2.node));
	PLCLIST_INSERT_TAIL(&(Master.Varlist), &(Var3.node));
	
	PLCLIST_FOREACH_SAFE(temp, n, (struct ListHead *)&(Master.Varlist))
	{
		Var = NODE_ENTRY(n, struct VarElement, node);
		Var->num = Var->num;
	}
}


