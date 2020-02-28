#include "DeDuplicate.h"

bool DeDuplicate::checkDuplicate(DronePlot& data)
{
	auto dup = findDuplicate(data);
	if(dup == db.end()) // A duplicate does not exist
	{
		for(auto x : knownOffsets) // If I know the offset, adjust the data (CASE: O1, D0)
		{
			if(x.SID == data.node_id) 
			{
				data.timestamp = static_cast<time_t>(static_cast<double>(data.timestamp) - x.offset);
			}
		}
		return true; // Add the new data to DB (includes CASE: O1, D0 and O0, D0)
		
		
		// If unknown offset, data will be corrected once offset is discovered (another case below)
	}
	else // Found a duplicate 
	{
		for(auto x : knownOffsets)
		{
			if(x.SID == data.node_id) // I already have information, and know the offset so drop the incoming data
			{
				return false; // CASE: O1, D1
			}
		}
		// If I got here, I don't know the offset (so calculate and add to list) CASE O0, D1
		SIDOffset s;
		s.SID = data.node_id;
		s.offset = static_cast<double>(data.timestamp - (*dup).timestamp);
		knownOffsets.push_back(s);
		
		// backlog to correct any non-adjusted data from case O0, D0.  
		correctForOffset(s);
		return false;
	}
}

// I DON'T EVEN KNOW MY NAME
void DeDuplicate::correctForOffset(SIDOffset s)
{
	for(auto asdf = db.begin(); asdf != db.end(); asdf++)
	{
		if((*asdf).node_id == s.SID)
		{
			(*asdf).timestamp = static_cast<time_t>(static_cast<double>((*asdf).timestamp) - s.offset);
		}
	}
}

bool isDup(DronePlot& newdata, DronePlot& existing)
{
	if(newdata.drone_id != existing.drone_id)
	{
		return false;
	}
	if(newdata.latitude != existing.latitude)
	{
		return false;
	}
	if(newdata.longitude != existing.longitude)
	{
		return false;
	}
	return true;
}

bool DeDuplicate::isDup2(DronePlot& newdata, DronePlot& existing)
{
	if(newdata.drone_id != existing.drone_id)
	{
		return false;
	}
	if(newdata.latitude != existing.latitude)
	{
		return false;
	}
	if(newdata.longitude != existing.longitude)
	{
		return false;
	}
	// ACCOUNT FOR EDGE CASE THAT GIVES WRONG OFFSET 
	if(existing.node_id != mySID) 
	{
		return false;
	}
	return true;
}

std::list<DronePlot>::iterator DeDuplicate::findDuplicate(DronePlot& data)
{
	for(auto trex = db.begin(); trex != db.end(); trex++)
	{
		auto & item = *trex;
		if(isDup2(data, item))
		{
			return trex;
		}
	}
	return db.end();
}

void DeDuplicate::finalRun()
{
	for(auto i = db.begin(); i != db.end(); i++)
	{
		for(auto j = i; j != db.end(); j++)
		{
			if((i!=j) && isDup(*i, *j)) 
			{
				db.erase(j);
				j = i;
			}
		}
	}
	
	// Coordinate time
	lowestSID = mySID;
	lowestTimeOff = 0;
	for(auto x : knownOffsets)
	{
		if(x.SID < lowestSID)
		{
			lowestSID = x.SID;
			lowestTimeOff = x.offset;
		}
	}
	
	// correct all offsets
	for(auto hydrogen = db.begin(); hydrogen != db.end(); hydrogen++)
	{
		(*hydrogen).timestamp = static_cast<time_t>(static_cast<double>((*hydrogen).timestamp) + lowestTimeOff);
	}
	
	// Presbyterians like things to be ordered
	db.sortByTime();
	
}

DeDuplicate::DeDuplicate(DronePlotDB& db, unsigned int mySID)
	:db(db), mySID(mySID)
{
	
}
