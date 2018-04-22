#ifndef _CREATIONPARAMS_H_
#define _CREATIONPARAMS_H_

#include "GamerCamp/GCCocosInterface/CreationParams/SGCFactoryCreationParams.h"

struct SPlayerParams 
{
	float m_fAddedXVelocity;
	float m_fVelocityLimit;
	float m_fVelocityDecay;
	float m_fJumpAddedYVelocity;
	float m_fGravityScale;
};
struct SCrumblingParams 
{
	float m_fCrumblingTime;
};
struct SBouncyParams 
{
	float	m_fForce;
	int		m_iAngle;
};
struct SFlyingParams 
{
	float m_fMoveSpeed;
	int m_fMoveDistanceX;
	int m_fMoveDistanceY;
};
struct SMovingParams
{
	float m_fMoveSpeed;
	int m_fMoveDistanceX;
	int m_fMoveDistanceY;
};
struct SPatrolParams 
{
	float	m_fMoveSpeed;
	bool	m_bStartsMovingRight;
};
struct SPursueParams 
{
	float	m_fMoveSpeed;
	bool	m_bStartsMovingRight;
	float	m_fChasingSpeed;
};
struct STurnbackParams
{
	bool m_bTurnRight;
	bool m_bStrongTurn;
};
struct SSpawnerParams 
{
	int Width;
	float Frequency;
};
struct SCookingMachineParams
{
	float CookingTime;
};

#define CREATE_CREATION_STRUCT( ClassName, ParamsName )											\
	struct SCreationParams_##ClassName : SGCFactoryCreationParams								\
	{																							\
	public:																						\
		S##ParamsName Data;																		\
		SCreationParams_##ClassName()															\
		{																						\
		}																						\
		SCreationParams_##ClassName( const SGCFactoryCreationParams* psFactoryCreationParams )	\
		{																						\
			strClassName = psFactoryCreationParams->strClassName;								\
			strPlistFile = psFactoryCreationParams->strPlistFile;								\
			strImageName = psFactoryCreationParams->strImageName;								\
			strPhysicsShape = psFactoryCreationParams->strPhysicsShape;							\
			eB2dBody_BodyType = psFactoryCreationParams->eB2dBody_BodyType;						\
			bB2dBody_FixedRotation = psFactoryCreationParams->bB2dBody_FixedRotation;			\
		}																						\
	};

// Struct instantiations.
CREATE_CREATION_STRUCT( Player, PlayerParams )
CREATE_CREATION_STRUCT( CrumblingPlatform, CrumblingParams )
CREATE_CREATION_STRUCT( BouncyPlatform, BouncyParams )
CREATE_CREATION_STRUCT( FlyingEnemy, FlyingParams )
CREATE_CREATION_STRUCT( MovingPlatform, MovingParams )
CREATE_CREATION_STRUCT( PatrolEnemy, PatrolParams )
CREATE_CREATION_STRUCT( PursuingEnemy, PursueParams )
CREATE_CREATION_STRUCT( TurnbackSensor, TurnbackParams )
CREATE_CREATION_STRUCT( EntitySpawner, SpawnerParams )
CREATE_CREATION_STRUCT( CookingMachine, CookingMachineParams )

#endif //_CREATIONPARAMS_H_
