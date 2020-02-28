#include "DronePlotDB.h"

typedef struct sidoffset
{
	int SID;
	double offset;
} SIDOffset;

class DeDuplicate
{
	public:
		DeDuplicate(DronePlotDB& db, unsigned int mySID);
		
		// determine if something should be inserted, and take corresponding action
		bool checkDuplicate(DronePlot& data);
		
		// verify everything is legit and fix any errors
		void finalRun();
		
	protected:
	
	private:
		// case enumerated functions
		void correctForOffset(SIDOffset x);
		
		std::list<DronePlot>::iterator findDuplicate(DronePlot& data);
		
		bool isDup2(DronePlot& newdata, DronePlot& existing);
	
		// reference to the database we are managing
		DronePlotDB& db;
		
		// my SID (for checking)
		unsigned int mySID;
		
		// List of other replication servers and their time offsets from us
		std::vector<SIDOffset> knownOffsets;
		
		// election info
		unsigned int lowestSID; // this is a good leader :)
		double lowestTimeOff;    // leader's time offset
	   
	
};