
#ifndef FALCLIST_H
#define FALCLIST_H

extern int SimCompare (VuEntity* ent1, VuEntity*ent2);

// =================================
// Falcon's Private Filters
// =================================

class FalconAllFilterType : public VuFilter 
	{
	public:

	public:
		FalconAllFilterType(void)							{}
		virtual ~FalconAllFilterType(void)					{}

		virtual VU_BOOL Test(VuEntity *)					{ return TRUE; }
		virtual VU_BOOL RemoveTest(VuEntity *)			{ return TRUE; }
		virtual int Compare(VuEntity *ent1, VuEntity *ent2)	{ return (SimCompare (ent1, ent2)); }
		virtual VuFilter *Copy()							{ return new FalconAllFilterType(); }
	};

class FalconNothingFilterType : public VuFilter
	{
	public:

	public:
		FalconNothingFilterType(void)						{}
		virtual ~FalconNothingFilterType(void)				{}

		virtual VU_BOOL Test(VuEntity *)					{ return FALSE; }
		virtual VU_BOOL RemoveTest(VuEntity *)			{ return TRUE; }
		virtual int Compare(VuEntity *ent1, VuEntity *ent2)	{ return (SimCompare (ent1, ent2)); }
		virtual VuFilter *Copy()							{ return new FalconNothingFilterType(); }
	};

extern FalconAllFilterType		FalconAllFilter;
extern FalconNothingFilterType	FalconNothingFilter;

// =================================
// Falcon's Private lists
// =================================

// TailInsertList is a Falcon-Private list structure which will add entries to the end of the list
class TailInsertList : public VuFilteredList
{
   public:
      TailInsertList(VuFilter *filter = &FalconNothingFilter);
      virtual ~TailInsertList(void);

      int Insert(VuEntity *entity);				// WARNING: DO NOT CALL THIS INSERT FUNCTION
      int ForcedInsert(VuEntity *entity);		// Call ForcedInsert instead.
};

// HeadInsertList is a Falcon-Private list structure which will add entries to the beginning of the list
class HeadInsertList : public VuFilteredList
{
   public:
      HeadInsertList(VuFilter *filter = &FalconNothingFilter);
      virtual ~HeadInsertList(void);

      int Insert(VuEntity *entity);				// WARNING: DO NOT CALL THIS INSERT FUNCTION
      int ForcedInsert(VuEntity *entity);		// Call ForcedInsert instead.
};

// Falcon FalconPrivateList is simply a Falcon-Private vu entity storage list
class FalconPrivateList : public VuFilteredList
{
   public:
      FalconPrivateList(VuFilter *filter = &FalconNothingFilter);
      virtual ~FalconPrivateList();

      int Insert(VuEntity *entity);				// WARNING: DO NOT CALL THIS INSERT FUNCTION
      int ForcedInsert(VuEntity *entity);		// Call ForcedInsert instead.
};

// Falcon PrivateFilteredList is identical to above but sorts entries
class FalconPrivateOrderedList : public VuFilteredList
{
   friend class VuListMuckyIterator;

   public:
      FalconPrivateOrderedList(VuFilter *filter = &FalconNothingFilter);
      virtual ~FalconPrivateOrderedList();

      int Insert(VuEntity *entity);				// WARNING: DO NOT CALL THIS INSERT FUNCTION
      int ForcedInsert(VuEntity *entity);		// Call ForcedInsert instead.
};

#endif