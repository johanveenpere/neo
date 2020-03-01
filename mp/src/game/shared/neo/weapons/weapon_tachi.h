#ifndef NEO_WEAPON_TACHI_H
#define NEO_WEAPON_TACHI_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
	#include "c_neo_player.h"
#else
	#include "neo_player.h"
#endif

#include "weapon_neobasecombatweapon.h"

#define	TACHI_FASTEST_MANUAL_REFIRE_TIME 0.2f
#define TACHI_FASTEST_AUTO_REFIRE_TIME 0.1f

#define	TACHI_FASTEST_DRY_REFIRE_TIME	0.2f

#define TACHI_FASTEST_FIREMODE_SWITCH_TIME 0.2f

#define	TACHI_ACCURACY_SHOT_PENALTY_TIME		0.2f	
#define	TACHI_ACCURACY_MAXIMUM_PENALTY_TIME	1.5f	// Maximum penalty to deal out

#ifdef CLIENT_DLL
#define CWeaponTachi C_WeaponTachi
#endif

class CWeaponTachi : public CNEOBaseCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponTachi, CNEOBaseCombatWeapon );

	CWeaponTachi(void);

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void	Precache( void );
	void	ItemPostFrame( void );
	void	ItemPreFrame( void );
	void	ItemBusyFrame( void );
	void	PrimaryAttack( void );
	void	AddViewKick( void );
	void	DryFire( void );

	void	UpdatePenaltyTime( void );

	Activity	GetPrimaryAttackActivity( void );

    virtual void SwitchFireMode( void );
    virtual void ForceSetFireMode( bool bPrimaryMode,
        bool bPlaySound = false, float flSoonestSwitch = 0.0f );

	virtual bool Reload( void );

	virtual int GetNeoWepBits(void) const { return NEO_WEP_TACHI; }
	virtual int GetNeoWepXPCost(const int neoClass) const { return 0; }

	virtual const Vector& GetBulletSpread( void )
	{		
		static Vector cone;

		float ramp = RemapValClamped(	m_flAccuracyPenalty, 
											0.0f, 
											TACHI_ACCURACY_MAXIMUM_PENALTY_TIME, 
											0.0f, 
											1.0f ); 

			// We lerp from very accurate to inaccurate over time
		VectorLerp( VECTOR_CONE_1DEGREES, VECTOR_CONE_6DEGREES, ramp, cone );

		return cone;
	}
	
	virtual int	GetMinBurst() 
	{ 
		return 1; 
	}

	virtual int	GetMaxBurst() 
	{ 
		return 3; 
	}

	virtual float GetFireRate( void );

    virtual bool IsPrimaryFireMode( void )
    {
        return m_bIsPrimaryFireMode;
    }
	
#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

private:
	CNetworkVar( float,	m_flSoonestPrimaryAttack );
    CNetworkVar( float, m_flSoonestFiremodeSwitch );
	CNetworkVar( float,	m_flLastAttackTime );
	CNetworkVar( float,	m_flAccuracyPenalty );
	
    CNetworkVar( int,	m_nNumShotsFired );

    CNetworkVar( bool, m_bIsPrimaryFireMode );

private:
	CWeaponTachi( const CWeaponTachi & );
};

#endif // NEO_WEAPON_TACHI_H