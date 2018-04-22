#include "ConfigFileReader.h"


//Reset all member variables, empties data containers.
void CConfigFileReader::Reset()
{
	ResetFloatConverter();
	ResetLineReader();
	
	if ( m_cIFile.is_open() )
	{
		m_cIFile.close();
	}
	
	memset( m_caLine, 0, km_iMaxLineLength );
	m_strLine			= "";
	m_strVariableName	= "";
	m_strValue			= "";

	m_eReaderLoadState	= EState_Undefined;
	m_eCurrentSpawn		= ELoops_Undefined;
	m_strCurrentSpawn	= "";

	//Uninstanced / Globals data containers.
	m_cUninstancedFloatVariables	= {};
	m_cUninstancedVectorsVariables	= {};

	//Spawn parameter containers
	m_cPatrolEnemySpawns		= {};
	m_cPursueEnemySpawns		= {};
	m_cFlyingEnemySpawns		= {};
	m_cStaticPlatformSpawns		= {};
	m_cMovingPlatformSpawns		= {};
	//m_cBouncyPlatformSpawns		= {};
	m_cCrumblingPlatformSpawns	= {};
	m_cHazardSpawns				= {};
}

//Reset the string to float stringstream
void CConfigFileReader::ResetFloatConverter()
{
	static std::locale uselocale( "" );
	ssFloatCoverter.imbue( uselocale );
	ssFloatCoverter.str( "" );
	ssFloatCoverter.clear();
}

void CConfigFileReader::ResetLineReader() 
{
	//Booleans for line reader status tracking
	m_bIsValue2DVector		= false;
	m_bIsFirstCharacter		= true;
	m_bSkipUntilEqualSign	= false;
	m_bVariableNameFound	= false;
	m_bValueFound			= false;
	m_bEndSymbolEncountered = false;

	//Input string variables
	m_strVariableName	= "";
	m_strValue			= "";
}


//Setup the input file for reading, setup mode and reset variables.
void CConfigFileReader::Init( std::string strFilename, EInputState EState_InputMode ) 
{
	Reset();
	m_cIFile.open(strFilename);

	//If stream is in bad state, exit.
	if ( !m_cIFile )
	{
		return;
	}

	switch ( EState_InputMode )
	{
	case EState_Uninstanced:
		m_eReaderLoadState = EState_Uninstanced;
		ReadVariablesFile();
		break;

	case EState_Spawn:
		m_eReaderLoadState = EState_Spawn;
		ReadSpawningListFile();
		break;

	case EState_Undefined:
	default:
		m_eReaderLoadState = EState_Undefined;
		break;
	}

	m_cIFile.close();
}




//================== File Reading Loopers ===================

//Input loop for reading in variables that don't belong to entity instances.
void CConfigFileReader::ReadVariablesFile() 
{
	ResetLineReader();
	sStringPair sOutputValues;

	while ( m_cIFile.getline( m_caLine, km_iMaxLineLength ) )
	{
		if ( m_bEndSymbolEncountered )
		{
			break;
		}
		m_bIsFirstCharacter = true;

		//Reset the string
		m_cIss.clear();
		m_strLine = std::string( m_caLine );
		m_cIss.str( m_strLine );
		m_strVariableName	= "";
		m_strValue			= "";

		sOutputValues = ReadVariableFromLine();

		//if line data isn't empty, print.
		if ( !( IsLineDataEmpty(sOutputValues) ) )
		{
			//std::cout << sOutputValues.first << std::endl << sOutputValues.second << std::endl;
			//Add data to containers.
			ProcessVariableStringIntoData( sOutputValues );
		}

		//Reset boolean line variables after every line.
		ResetLineReader();
	}
}


//Input loop for reading data that belongs to entity instances.
void CConfigFileReader::ReadSpawningListFile() 
{
	bool bStartNewInstanceList		= false;
	bool bBreakForLoop				= false;
	std::string strSpawnCategory	= "";
	ResetLineReader();

	while ( m_cIFile )
	{
		while ( m_cIFile.getline( m_caLine, km_iMaxLineLength ) )
		{
			if ( m_bEndSymbolEncountered )
			{
				break;
			}
			m_bIsFirstCharacter = true;
			bBreakForLoop		= false;

			//Reset the string & stringstream
			m_cIss.clear();
			m_strLine = std::string( m_caLine );
			m_cIss.str( m_strLine );

			for ( char c : m_caLine )
			{
				//Check if the current line can be discarded
				if ( bBreakForLoop )
				{
					break;
				}

				switch ( c )
				{
				//Special character to end read - debug.
				case '!':
					return;
				//ignore the commented line - next loop iteration
				case '#':
					bBreakForLoop = true;
					break;
				//New spawn type encountered
				case '[':
					strSpawnCategory = "";
					m_cIss >> strSpawnCategory;
					m_strCurrentSpawn = strSpawnCategory;
					m_eCurrentSpawn = m_cStringToEntityEnumMap[strSpawnCategory];
					bBreakForLoop = true;
					break;
				//Start a new instance list
				case '@':
					bStartNewInstanceList = true;
					bBreakForLoop = true;
					break;
				default:
					if ( iswspace( c ) != 0 )
					{
						continue;
					}
					else
					{
						bBreakForLoop = true;
						break;
					}
				}
			}

			//Reset boolean line variables after every line.
			ResetLineReader();

			//Break reading inner while loop to start new reading function.
			if ( bStartNewInstanceList )
			{
				break;
			}
		}


		//Read spawn instance data.
		if ( bStartNewInstanceList )
		{
			SetNewInstanceData();
			bStartNewInstanceList = false;
		}
	}
}


//=================== Line-by-Line Reader ====================

//Read the line to find the name and associated value in text file
//Returns pair of two strings <name, value>
sStringPair CConfigFileReader::ReadVariableFromLine()
{
	//Reset booleans before reading line characters
	ResetLineReader();

	for ( char c : m_strLine )
	{
		//Special character to end read - debug.
		if ( c == '!' )
		{
			m_bEndSymbolEncountered = true;
			break;
		}

		//Continue looping in the string until '=' met.
		if ( m_bSkipUntilEqualSign )
		{
			if ( c == '=' )
			{
				m_bSkipUntilEqualSign = false;
			}
			continue;
		}

		//iswspace returns non-zero if whitespace characters
		if ( iswspace( c ) != 0 )
		{
			//consume
			continue;
		}
		else if ( c == '#' && m_bIsFirstCharacter )
		{
			//ignore the commented line - next loop iteration
			break;
		}
		else if ( c == 'v' && m_bIsFirstCharacter )
		{
			//This value is a vector value
			m_bIsValue2DVector = true;
		}
		else if ( m_bVariableNameFound == false )
		{
			//store the contiguous string element
			m_cIss >> m_strVariableName;
			//If it's a 2d vector, we skip the v from the name.
			if ( m_bIsValue2DVector )
			{
				m_cIss >> m_strVariableName;
			}

			m_bVariableNameFound = true;
			//Jump to the equals sign
			m_bSkipUntilEqualSign = true;
		}
		else
		{
			//Read the value at this block and store it.
			m_strValue.push_back( c );
			m_bValueFound = true;
		}

		m_bIsFirstCharacter = false;
	}

	return sStringPair(m_strVariableName, m_strValue);
}



//============ Data Processing Auxillary Functions ===========

//Take the string pair and process them into the relevant data container.
void CConfigFileReader::ProcessVariableStringIntoData( sStringPair cDataPair )
{
	ResetFloatConverter();
	
	//Different data storage for different types of data.
	if ( m_bIsValue2DVector )
	{
		std::string strXValue = "";
		std::string strYValue = "";
		float fXValue = 0.0f;
		float fYValue = 0.0f;
		bool bFirstValueRead = false;

		for ( char c : cDataPair.second )
		{
			switch ( c )
				{
			case '(':
			case ')':
				break;
			case ',':
				bFirstValueRead = true;
				break;
			default:
				if ( iswspace( c ) != 0 )
				{
					continue;
				}

				//Output the characters into the respective strings.
				if ( bFirstValueRead )
				{
					strYValue.push_back( c );
				}
				else
				{
					strXValue.push_back( c );
				}
				break;
			}
		}

		//first value.
		ssFloatCoverter << strXValue;
		ssFloatCoverter >> fXValue;

		//Clear the stringstream so it can be reused.
		ssFloatCoverter.str( "" );
		ssFloatCoverter.clear();

		//second value.
		ssFloatCoverter << strYValue;
		ssFloatCoverter >> fYValue;

		//add pair to map.
		sFloatPair pair_value( fXValue, fYValue );
		if ( m_eReaderLoadState == EState_Uninstanced )
		{
			m_cUninstancedVectorsVariables[cDataPair.first] = pair_value;
		}
		else if ( m_eReaderLoadState == EState_Spawn )
		{
			m_cSpawnVectorVariables.push_back( pair_value );
		}

	}
	else
	{
		float fValue;
		ssFloatCoverter << cDataPair.second;
		ssFloatCoverter >> fValue;
		if ( m_eReaderLoadState == EState_Uninstanced )
		{
			m_cUninstancedFloatVariables[cDataPair.first] = fValue;
		}
		else if ( m_eReaderLoadState == EState_Spawn )
		{
			m_cSpawnFloatVariables.push_back(fValue);
		}
	}
}


//Make a new instance of data for current spawn type.
void CConfigFileReader::SetNewInstanceData() 
{
	//Potential data members
	SEnemyParams sEnemyParams;
	SMovingPlatformParams sMovingPlatformParams;
	//SBouncyPlatformParams sBouncyPlatformParams;
	sFloatPair sTempFloatPair;
	float fTempFloat;

	//Instantiate a vector of string pairs
	std::vector<sStringPair> cStringPairValues;

	//Loop m_eCurrentSpawn length times.
	for ( int iLoop = 0; iLoop < m_eCurrentSpawn; ++iLoop )
	{
		//Get a new line per iteration.
		m_cIFile.getline( m_caLine, km_iMaxLineLength );

		//Reset the string & stringstream
		m_cIss.clear();
		m_strLine = std::string( m_caLine );
		m_cIss.str( m_strLine );

		//Get String Value pair
		sStringPair sStringPair = ReadVariableFromLine();
		cStringPairValues.push_back( sStringPair );

		//Process string value pair into container.
		ProcessVariableStringIntoData( sStringPair );

		int iContainerLength = -1;
		( sStringPair.second[0] == '(' ) 
			? iContainerLength = m_cSpawnVectorVariables.size()
			: iContainerLength = m_cSpawnFloatVariables.size();

		//Add data to relevant struct
		if (	m_strCurrentSpawn == "[Patrol_Enemy]" 
			||	m_strCurrentSpawn == "[Pursuing_Enemy]"  
			||	m_strCurrentSpawn == "[Flying_Enemy]")
		{
			switch ( iLoop )
			{
			case 0:
			{
				sTempFloatPair = m_cSpawnVectorVariables[iContainerLength - 1];
				m_cSpawnVectorVariables.pop_back();
				sEnemyParams.sSpawnPosition = sTempFloatPair;
			}
				break;
			case 1:
			{
				sTempFloatPair = m_cSpawnVectorVariables[iContainerLength - 1];
				m_cSpawnVectorVariables.pop_back();
				sEnemyParams.sMovementDirection = sTempFloatPair;
			}
				break;
			case 2:
			{
				fTempFloat = m_cSpawnFloatVariables[iContainerLength - 1];
				m_cSpawnFloatVariables.pop_back();
				sEnemyParams.fMaximumMoveDistance = fTempFloat;

				//Push value/data into vector.
				if ( m_strCurrentSpawn == "[Patrol_Enemy]" )
				{
					m_cPatrolEnemySpawns.push_back( sEnemyParams );
				}
				else if ( m_strCurrentSpawn == "[Pursuing_Enemy]" )
				{
					m_cPursueEnemySpawns.push_back( sEnemyParams );
				}
				else if ( m_strCurrentSpawn == "[Flying_Enemy]" )
				{
					m_cFlyingEnemySpawns.push_back( sEnemyParams );
				}
			}
				break;
			default:
				//Error
				return;
			}
		}
		else if ( m_strCurrentSpawn == "[Moving_Platforms]" )
		{
			sTempFloatPair = m_cSpawnVectorVariables[iContainerLength - 1];
			m_cSpawnVectorVariables.pop_back();

			switch ( iLoop )
			{
			case 0:
			{
				sMovingPlatformParams.sStartingPosition = sTempFloatPair;
			}
				break;
			case 1:
			{
				sMovingPlatformParams.sEndingPosition = sTempFloatPair;
			}
				break;
			case 2:
			{
				sMovingPlatformParams.sMovementVelocity = sTempFloatPair;
				//Push value/data into vector.
				m_cMovingPlatformSpawns.push_back(sMovingPlatformParams);
			}
				break;
			default:
				//Error
				break;
			}
		}
		else if ( m_strCurrentSpawn == "[Bouncy_Platforms]" )
		{
			switch ( iLoop )
			{
			case 0:
				sTempFloatPair = m_cSpawnVectorVariables[iContainerLength - 1];
				m_cSpawnVectorVariables.pop_back();
				//sBouncyPlatformParams.sSpawnPosition = sTempFloatPair;
				break;
			case 1:
				fTempFloat = m_cSpawnFloatVariables[iContainerLength - 1];
				m_cSpawnFloatVariables.pop_back();
				//sBouncyPlatformParams.fPlatformRotationInDegrees = fTempFloat;

				//Push value/data into vector.
				//m_cBouncyPlatformSpawns.push_back(sBouncyPlatformParams);
				break;
			default:
				//Error
				break;
			}
		}
		else if ( m_strCurrentSpawn == "[Static_Platforms]"
			|| m_strCurrentSpawn == "[Crumbling_Platforms]" 
			|| m_strCurrentSpawn == "[Hazards]"
			|| m_strCurrentSpawn == "[Items]")
		{
			sTempFloatPair = m_cSpawnVectorVariables[iContainerLength - 1];
			m_cSpawnVectorVariables.pop_back();
			
			//Push value/data into vector.
			if ( m_strCurrentSpawn == "[Static_Platforms]" )
			{
				m_cStaticPlatformSpawns.push_back( sTempFloatPair );
			}
			else if ( m_strCurrentSpawn == "[Crumbling_Platforms]" )
			{
				m_cCrumblingPlatformSpawns.push_back( sTempFloatPair );
			}
			else if ( m_strCurrentSpawn == "[Hazards]" ) 
			{
				m_cHazardSpawns.push_back( sTempFloatPair );
			}
			else if ( m_strCurrentSpawn == "[Items]" )
			{
				m_cItemSpawns.push_back( sTempFloatPair );
			}
		}
	}	
}


//Check if line read result is empty
bool CConfigFileReader::IsLineDataEmpty( sStringPair sOutputValues )
{
	//Run a check on the line read result.
	bool bIsFirstEmpty = sOutputValues.first == "";
	bool bIsSecondEmpty = sOutputValues.second == "";
	return ( bIsFirstEmpty && bIsSecondEmpty );
}


//================ Data container accessors ==================

std::map<std::string, float>&					CConfigFileReader::GetUninstancedFloatVariables()
{
	return m_cUninstancedFloatVariables;
}
std::map<std::string, sFloatPair>&				CConfigFileReader::GetUninstancedVectorVariables()
{
	return m_cUninstancedVectorsVariables;
}
const std::vector<SEnemyParams>&				CConfigFileReader::GetPatrolEnemySpawns()  const
{
	return m_cPatrolEnemySpawns;
}
const std::vector<SEnemyParams>&				CConfigFileReader::GetPursueEnemySpawns()  const
{
	return m_cPursueEnemySpawns;
}
const std::vector<SEnemyParams>&				CConfigFileReader::GetFlyingEnemySpawns()  const
{
	return m_cFlyingEnemySpawns;
}
const std::vector<sFloatPair>&					CConfigFileReader::GetStaticPlatformSpawns()  const
{
	return m_cStaticPlatformSpawns;
}
const std::vector<SMovingPlatformParams>&		CConfigFileReader::GetMovingPlatformSpawns()  const
{
	return m_cMovingPlatformSpawns;
}
//const std::vector<SBouncyPlatformParams>&		CConfigFileReader::GetBouncyPlatformSpawns()  const
//{
//	return m_cBouncyPlatformSpawns;
//}
const std::vector<sFloatPair>&					CConfigFileReader::GetCrumblingPlatformSpawns()  const
{
	return m_cCrumblingPlatformSpawns;
}
const std::vector<sFloatPair>&					CConfigFileReader::GetHazardSpawns() const
{
	return m_cHazardSpawns;
}
const std::vector<sFloatPair>&					CConfigFileReader::GetItemSpawns() const
{
	return m_cItemSpawns;
}




//==================== DEBUG / TESTING =====================

void CConfigFileReader::PrintOutUninstancedVariables()
{
	std::cout << "\n\nPrinting Out Stored Values:\n" << std::endl;
	for ( auto& elem : m_cUninstancedFloatVariables )
	{
		std::cout << elem.first << "\t" << elem.second << std::endl;
	}

	for ( auto& elem : m_cUninstancedVectorsVariables )
	{
		std::cout << elem.first << "\t( " << elem.second.first << ", " << elem.second.second << " )" << std::endl;
	}
}

void CConfigFileReader::PrintOutSpawnVariables() 
{
	std::cout << "\n\nPrinting Out Stored Values:\n" << std::endl;
	std::cout << "\nCategory: [Patrol_Enemy]" << std::endl;
	for ( SEnemyParams& sParams : m_cPatrolEnemySpawns )
	{
		std::cout << "SpawnPosition: " 
			<< "( " << sParams.sSpawnPosition.first << ", " << sParams.sSpawnPosition.second << " )" << std::endl;

		std::cout << "MovementDirection: "
			<< "( " << sParams.sSpawnPosition.first << ", " << sParams.sSpawnPosition.second << " )" << std::endl;

		std::cout << "MaximumMoveDistance: " << sParams.fMaximumMoveDistance << std::endl;
	}

	std::cout << "\nCategory: [Pursuing_Enemy]" << std::endl;
	for ( SEnemyParams& sParams : m_cPatrolEnemySpawns )
	{
		std::cout << "SpawnPosition: "
			<< "( " << sParams.sSpawnPosition.first << ", " << sParams.sSpawnPosition.second << " )" << std::endl;

		std::cout << "MovementDirection: "
			<< "( " << sParams.sSpawnPosition.first << ", " << sParams.sSpawnPosition.second << " )" << std::endl;

		std::cout << "MaximumMoveDistance: " << sParams.fMaximumMoveDistance << std::endl;
	}

	std::cout << "\nCategory: [Flying_Enemy]" << std::endl;
	for ( SEnemyParams& sParams : m_cFlyingEnemySpawns )
	{
		std::cout << "SpawnPosition: "
			<< "( " << sParams.sSpawnPosition.first << ", " << sParams.sSpawnPosition.second << " )" << std::endl;

		std::cout << "MovementDirection: "
			<< "( " << sParams.sSpawnPosition.first << ", " << sParams.sSpawnPosition.second << " )" << std::endl;

		std::cout << "MaximumMoveDistance: " << sParams.fMaximumMoveDistance << std::endl;
	}


	std::cout << "\nCategory: [Static_Platforms]" << std::endl;
	for ( auto& fParam : m_cStaticPlatformSpawns )
	{
		std::cout << "SpawnPosition: "
			<< "( " << fParam.first << ", " << fParam.second << " )" << std::endl;
	}

	std::cout << "\nCategory: [Moving_Platforms]" << std::endl;
	for ( SMovingPlatformParams& sParams : m_cMovingPlatformSpawns )
	{
		std::cout << "StartingPosition: "
			<< "( " << sParams.sStartingPosition.first << ", " << sParams.sStartingPosition.second << " )" << std::endl;

		std::cout << "EndingPosition: "
			<< "( " << sParams.sEndingPosition.first << ", " << sParams.sEndingPosition.second << " )" << std::endl;

		std::cout << "MovementVelocity: "
			<< "( " << sParams.sMovementVelocity.first << ", " << sParams.sMovementVelocity.second << " )" << std::endl;
	}

	/*std::cout << "\nCategory: [Bouncy_Platforms]" << std::endl;
	for ( SBouncyPlatformParams& sParam : m_cBouncyPlatformSpawns )
	{
		std::cout << "SpawnPosition: "
			<< "( " << sParam.sSpawnPosition.first << ", " << sParam.sSpawnPosition.second << " )" << std::endl;
		std::cout << "PlatformAngle (degrees): " << sParam.fPlatformRotationInDegrees << std::endl;
	}*/

	std::cout << "\nCategory: [Crumbling_Platforms]" << std::endl;
	for ( auto& fParam : m_cCrumblingPlatformSpawns )
	{
		std::cout << "SpawnPosition: "
			<< "( " << fParam.first << ", " << fParam.second << " )" << std::endl;
	}

	std::cout << "\nCategory: [Hazards]" << std::endl;
	for ( auto& fParam : m_cHazardSpawns )
	{
		std::cout << "SpawnPosition: "
			<< "( " << fParam.first << ", " << fParam.second << " )" << std::endl;
	}

	std::cout << "\nCategory: [Items]" << std::endl;
	for ( auto& fParam : m_cItemSpawns )
	{
		std::cout << "SpawnPosition: "
			<< "( " << fParam.first << ", " << fParam.second << " )" << std::endl;
	}
}