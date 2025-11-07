#include "SelectionList.h"

CSelectionList::CSelectionList(UInt32Vector& vectorOfValue, UInt32Vector& weighVector)
{
	TRACE_ENSURE(vectorOfValue.size());
	TRACE_ENSURE(vectorOfValue.size()==weighVector.size());

	m_value = vectorOfValue;
	m_weigh = weighVector;
	ResetTempVariables();

	m_rand.Randomize();
}

CSelectionList::~CSelectionList()
{
	m_value.clear();
	m_weigh.clear();
	m_tempValue.clear();
	m_tempWeigh.clear();
}

UInt32 CSelectionList::Get()
{
	if( m_tempValue.empty() || m_tempWeigh.empty() )
		return -1;

	// Find a random class(selection range)
	UInt32 randno = m_rand.GetRandom(GetTempTotal());
	UInt32 tempno = 0;
	UInt32 foundIdx = -1;
	for( Index i = C_INDEX(0); i < m_tempWeigh.size(); ++i )
	{
		tempno += m_tempWeigh[i];
		if( tempno > randno )
		{
			foundIdx = i;
			break;
		}
	}
	TRACE_WARNDTL_IF_2(GLOBAL_LOGGER, foundIdx==-1, 
		_T("CSelectionList::Peek() [not match] foundIdx=%d, total=%d"), foundIdx, GetTempTotal());

	// store the output before erase
	UInt32 outputValue = m_tempValue[foundIdx];
	// erase the output address from temp vectors
	UInt32 counter;
	counter = 0;
	for( UInt32Vector::iterator itr = m_tempValue.begin(); counter < foundIdx; ++itr, ++counter );
	m_tempValue.erase(itr);
	counter = 0;
	for( UInt32Vector::iterator itr = m_tempWeigh.begin(); counter < foundIdx; ++itr, ++counter );
	m_tempWeigh.erase(itr);

	return outputValue;
}

UInt32 CSelectionList::GetTempTotal()
{
	UInt32 total = 0;
	for( Index i = C_INDEX(0); i < m_tempWeigh.size(); ++i )
		total += m_tempWeigh[i];
	return total;
}

VOID CSelectionList::Reset()
{
	ResetTempVariables();
}

VOID CSelectionList::ResetTempVariables()
{
	m_tempValue = GetValueVector();
	m_tempWeigh = GetWeighVector();
}
