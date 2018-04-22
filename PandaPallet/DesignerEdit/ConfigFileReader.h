#ifndef __CConfigFileReader_H_
#define __CConfigFileReader_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

//Typedefs for readability.
typedef std::pair<float, float>					sFloatPair;
typedef std::pair<std::string, float>			sStringFloatPair;
typedef std::pair<std::string, std::string>		sStringPair;

//////////////////////////////////////////////////////////////////////////
/// \struct SEnemyParams
/// \brief
/// Holds Enemy class type instance parameters.
//////////////////////////////////////////////////////////////////////////
struct SEnemyParams
{
	sFloatPair	sSpawnPosition			= { 0.0f, 0.0f }; ///< The position in game where an enemy appears
	sFloatPair	sMovementDirection		= { 0.0f, 0.0f }; ///< The direction in which an enemy moves
	float		fMaximumMoveDistance	= 0.0f;			  ///< The distance that an enemy walks before turning back
};

//////////////////////////////////////////////////////////////////////////
/// \struct SMovingPlatformParams 
/// \brief
/// Holds MovingPlatform class type instance parameters.
//////////////////////////////////////////////////////////////////////////
struct SMovingPlatformParams
{
	sFloatPair sStartingPosition		= { 0.0f, 0.0f }; ///< The position in game where a platform appears
	sFloatPair sEndingPosition			= { 0.0f, 0.0f }; ///< The position in game where a platform will go
	sFloatPair sMovementVelocity		= { 0.0f, 0.0f }; ///< The speed of a platform
};

//////////////////////////////////////////////////////////////////////////
/// \struct SBouncyPlatformParams
/// \brief
/// Holds BouncyPlatform class type instance parameters.
//////////////////////////////////////////////////////////////////////////
//struct SBouncyPlatformParams 
//{
//	sFloatPair sSpawnPosition			= { 0.0f, 0.0f }; ///< The position in game where a platform appears
//	float fPlatformRotationInDegrees	= 0.0f;			  ///< A rotation that is applied to a body on start
//};

//////////////////////////////////////////////////////////////////////////
/// \class CConfigFileReader 
/// \brief
/// This class is responsible for reading level files and 
/// for passing the data to the game level.
//////////////////////////////////////////////////////////////////////////
class CConfigFileReader 
{
public:
	/// \enum EInputState
	/*! 
		The configuration state of the file reader used in Init(), 
		Undefined is a default for uninitialised class
	*/
	enum EInputState
	{
		EState_Undefined = 0,	///< Default state
		EState_Uninstanced,		///< State in which Variables file is read
		EState_Spawn			///< State in which SpawningList file is read
	}; 

	/// Default Constructor.
	CConfigFileReader()
	{
	}

	/// Default Destructor.
	~CConfigFileReader() 
	{
	}

	/// Resets all class members and free memory
	void Reset();

	/// Setup the input file for reading, setup mode and reset variables.
	/*! 
		@param strFilename file name that will be read
		@param EState_InputMode state in which file will be read
	*/
	void Init( std::string strFilename, EInputState EState_InputMode );

	/// Input loop for reading in variables that don't belong to entity instances.
	void ReadVariablesFile();

	/// Input loop for reading data that belongs to entity instances.
	void ReadSpawningListFile();

	/// Returns maps of Uninstantiated float variables
	std::map<std::string, float>&				GetUninstancedFloatVariables();
	/// Returns maps of Uninstantiated vector variables
	std::map<std::string, sFloatPair>&			GetUninstancedVectorVariables();
	/// Returns vector of all patrol enemies spawn data
	const std::vector<SEnemyParams>&			GetPatrolEnemySpawns()				const;
	/// Returns vector of all pursuing enemies spawn data
	const std::vector<SEnemyParams>&			GetPursueEnemySpawns()				const;
	/// Returns vector of all flying enemies spawn data
	const std::vector<SEnemyParams>&			GetFlyingEnemySpawns()				const;
	/// Returns vector of all static platforms spawn data
	const std::vector<sFloatPair>&				GetStaticPlatformSpawns()			const;
	/// Returns vector of all moving  platforms spawn data
	const std::vector<SMovingPlatformParams>&	GetMovingPlatformSpawns()			const;
	/// Returns vector of all bouncy platforms spawn data
	//const std::vector<SBouncyPlatformParams>&	GetBouncyPlatformSpawns()			const;
	/// Returns vector of all crumbling platforms spawn data
	const std::vector<sFloatPair>&				GetCrumblingPlatformSpawns()		const;
	/// Returns vector of all hazards spawn data
	const std::vector<sFloatPair>&				GetHazardSpawns()					const;
	/// Returns vector of all items spawn data
	const std::vector<sFloatPair>&				GetItemSpawns()						const;
	/// Prints out all Uninstantiated Variables
	void PrintOutUninstancedVariables();
	/// Prints out all spawn data
	void PrintOutSpawnVariables();


private:
	/// \enum EEntityLoopCount
	/*! 
		Defines the amount of variables that has to be read
		depending on an Object (enemy, platform etc.)
	*/
	enum EEntityLoopCount
	{
		ELoops_Patrol_Enemy			= 3,
		ELoops_Pursue_Enemy			= 3,
		ELoops_Flying_Enemy			= 3,
		ELoops_Static_Platform		= 1,
		ELoops_Moving_Platform		= 3,
		ELoops_Bouncy_Platform		= 2,
		ELoops_Crumbling_Platform	= 1,
		ELoops_Hazard				= 1,
		ELoops_Item					= 1,
		ELoops_Undefined			= 0
	};

	/// A map that contains connection between Name In file and and EEntityLoopCount
	std::map<std::string, EEntityLoopCount> m_cStringToEntityEnumMap =
	{
		{ "[Patrol_Enemy]" , ELoops_Patrol_Enemy },
		{ "[Pursuing_Enemy]", ELoops_Pursue_Enemy },
		{ "[Flying_Enemy]", ELoops_Flying_Enemy },
		{ "[Static_Platforms]", ELoops_Static_Platform },
		{ "[Moving_Platforms]", ELoops_Moving_Platform },
		{ "[Bouncy_Platforms]", ELoops_Bouncy_Platform },
		{ "[Crumbling_Platforms]", ELoops_Crumbling_Platform },
		{ "[Hazards]", ELoops_Hazard },
		{ "[Items]", ELoops_Item }
	};

	static const int km_iMaxLineLength = 512;	///< Max characters is read from 1 line
	std::ifstream m_cIFile;						///< File reader that is used as input stream
	char m_caLine[km_iMaxLineLength];			///< The read line
	std::istringstream m_cIss;					///< A string stream where string from file is stored
	std::stringstream ssFloatCoverter;			///< A string stream where all floats are stored
	std::string m_strLine;						
	std::string m_strVariableName;				
	std::string m_strValue;						

	bool m_bIsValue2DVector;
	bool m_bIsFirstCharacter;
	bool m_bSkipUntilEqualSign;
	bool m_bVariableNameFound;
	bool m_bValueFound;
	bool m_bEndSymbolEncountered;

	EInputState			m_eReaderLoadState;		///< Current state of a file reader
	EEntityLoopCount	m_eCurrentSpawn;		///< Current object data that is being read
	std::string			m_strCurrentSpawn;		///< Name of the current object data that is being read 
	
	//Uninstanced / Globals data containers.
	std::map<std::string, float>			m_cUninstancedFloatVariables;	///< Map that associates all the floats that are read 
																			///< from file with appropriate string
	std::map<std::string, sFloatPair>		m_cUninstancedVectorsVariables; ///< Map that associates all float pairs that are read 
																			///< from file with appropriate string
	
	std::vector<float>						m_cSpawnFloatVariables;			///< Unsorted float parameter containers for spawn data.
	std::vector<sFloatPair>					m_cSpawnVectorVariables;		///< Unsorted float pair parameter containers for spawn data.

	std::vector<SEnemyParams>				m_cPatrolEnemySpawns;			///< Unsorted patrol enemy data containers for spawn data.
	std::vector<SEnemyParams>				m_cPursueEnemySpawns;			///< Unsorted pursuing enemy data containers for spawn data.
	std::vector<SEnemyParams>				m_cFlyingEnemySpawns;			///< Unsorted flying enemy data containers for spawn data.
	std::vector<sFloatPair>					m_cStaticPlatformSpawns;		///< Unsorted static platform data containers for spawn data.
	std::vector<SMovingPlatformParams>		m_cMovingPlatformSpawns;		///< Unsorted moving platform data containers for spawn data.
	//std::vector<SBouncyPlatformParams>		m_cBouncyPlatformSpawns;		///< Unsorted bouncy platform data containers for spawn data.
	std::vector<sFloatPair>					m_cCrumblingPlatformSpawns;		///< Unsorted crumbling platform data containers for spawn data.
	std::vector<sFloatPair>					m_cHazardSpawns;				///< Unsorted hazard data containers for spawn data.
	std::vector<sFloatPair>					m_cItemSpawns;					///< Unsorted item data containers for spawn data.

	/// Reset the string to float stringstream
	void ResetFloatConverter();

	/// Reset the line reader booleans.
	void ResetLineReader();

	/// Read the line to find the name and associated value in text file
	/*! 
		@returns pair of two strings <name, value>
	*/
	sStringPair ReadVariableFromLine();

	/// Take the string pair and process them into the relevant data container.
	void ProcessVariableStringIntoData( sStringPair cDataPair );

	/// Make a new instance of data for current spawn type.
	void SetNewInstanceData();

	/// Check if line read result is empty
	/*! 
		@param sOutputValues pair of two strings <name, value>
	*/
	bool IsLineDataEmpty( sStringPair sOutputValues );
};

#endif //__CConfigFileReader_H_