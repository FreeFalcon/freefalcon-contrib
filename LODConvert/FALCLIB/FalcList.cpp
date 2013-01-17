#include <time.h>
#include "falclib.h"
#include "classtbl.h"
//#include "simlib.h"
//#include "simvucb.h"
#include "vu2\src\vu_priv.h"
#include "F4Vu.h"
#include "FalcList.h"

// =================================
// Falcon's Private lists
// =================================

// Private collection functions.  These collections provide a ForcedInsert that
// actually does the inserting. The Insert function does nothing and is provided
// to keep VU from adding things to the lists. The application must call ForcedInsert
// to actually add to the list.
// The optional filter in the constructor can add efficiency if it has a reasonable
// RemoveTest() function which will filter out entities which couldn't be in the list

// Private List

FalconPrivateList::FalconPrivateList(VuFilter *filter) : VuFilteredList(filter)
	{
	}

FalconPrivateList::~FalconPrivateList()
	{
	}

int FalconPrivateList::Insert(VuEntity *entity)
	{
	return 0;
	entity;
	}

int FalconPrivateList::ForcedInsert(VuEntity *entity)
	{
	ShiAssert ( filter_->RemoveTest(entity) );
	ShiAssert ( entity->VuState() == VU_MEM_ACTIVE );

	return VuLinkedList::Insert(entity);
	}

// Ordered List

FalconPrivateOrderedList::FalconPrivateOrderedList(VuFilter *filter) : VuFilteredList(filter)
	{
	}

FalconPrivateOrderedList::~FalconPrivateOrderedList()
	{
	}

int FalconPrivateOrderedList::Insert(VuEntity *entity)
	{
	return 0;
	entity;
	}

int FalconPrivateOrderedList::ForcedInsert(VuEntity *entity)
	{
	VuListMuckyIterator iter(this);

	ShiAssert ( filter_->RemoveTest(entity) );
	ShiAssert ( entity->VuState() <= VU_MEM_ACTIVE );

	for (VuEntity *ptr = iter.GetFirst(); ptr; ptr = iter.GetNext())
		{
		if (FalconAllFilter.Compare(entity, ptr) > 0)
			{
			iter.InsertCurrent(entity);
			return 1;
			}
		else if (entity == ptr)
			{
			ShiAssert (entity != ptr);
			return 1;
			}
		}
	iter.InsertCurrent(entity);
	return 1;
	}

// Tail insert list

TailInsertList::TailInsertList(VuFilter *filter) : VuFilteredList(filter)
	{
	}

TailInsertList::~TailInsertList(void)
	{
	}

int TailInsertList::Insert(VuEntity *entity)
	{
	return 0;
	entity;
	}

int TailInsertList::ForcedInsert(VuEntity *entity)
	{
	VuLinkNode* cur;
	VuLinkNode* tmp;
   int count = 1;

	ShiAssert ( filter_->RemoveTest(entity) );
	ShiAssert ( entity->VuState() <= VU_MEM_ACTIVE );
	
	VuEnterCriticalSection();
//	VuReferenceEntity(entity);
	tmp = new VuLinkNode (entity, tail_);
	if (head_ == tail_)
		head_ = tmp;
	else
		{
		cur = head_;
      count ++;
		while (cur->next_ != tail_)
      {
         count ++;
			cur = cur->next_;
      }

		ShiAssert (cur->next_ == tail_);
		cur->next_ = tmp;
		}
	VuExitCriticalSection();
	return count;
	}

// Head insert list

HeadInsertList::HeadInsertList(VuFilter *filter) : VuFilteredList(filter)
	{
	}

HeadInsertList::~HeadInsertList(void)
	{
	}

int HeadInsertList::Insert(VuEntity *entity)
	{
	return 0;
	entity;
	}

int HeadInsertList::ForcedInsert(VuEntity *entity)
	{
	VuLinkNode* tmp;
	int count = 1;

	ShiAssert ( filter_->RemoveTest(entity) );
	ShiAssert ( entity->VuState() <= VU_MEM_ACTIVE );
	
	VuEnterCriticalSection();
//	VuReferenceEntity(entity);
	tmp = new VuLinkNode (entity, tail_);

	tmp->next_ = head_;
	head_ = tmp;

	VuExitCriticalSection();
	return count;
	}
