#include "cbase.h"
#include "weapon_pz.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponPZ, DT_WeaponPZ)

BEGIN_NETWORK_TABLE(CWeaponPZ, DT_WeaponPZ)
#ifdef CLIENT_DLL
RecvPropTime(RECVINFO(m_flSoonestAttack)),
RecvPropTime(RECVINFO(m_flLastAttackTime)),
RecvPropFloat(RECVINFO(m_flAccuracyPenalty)),
RecvPropInt(RECVINFO(m_nNumShotsFired)),
#else
SendPropTime(SENDINFO(m_flSoonestAttack)),
SendPropTime(SENDINFO(m_flLastAttackTime)),
SendPropFloat(SENDINFO(m_flAccuracyPenalty)),
SendPropInt(SENDINFO(m_nNumShotsFired)),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeaponPZ)
DEFINE_PRED_FIELD(m_flSoonestAttack, FIELD_FLOAT, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_flLastAttackTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_flAccuracyPenalty, FIELD_FLOAT, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_nNumShotsFired, FIELD_INTEGER, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS(weapon_pz, CWeaponPZ);
PRECACHE_WEAPON_REGISTER(weapon_pz);

NEO_ACTTABLE(CWeaponPZ);

CWeaponPZ::CWeaponPZ()
{
	m_flSoonestAttack = gpGlobals->curtime;
	m_flAccuracyPenalty = 0;

	m_nNumShotsFired = 0;
}

void CWeaponPZ::DryFire()
{
	WeaponSound(EMPTY);
	SendWeaponAnim(ACT_VM_DRYFIRE);

	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}

void CWeaponPZ::Spawn()
{
	BaseClass::Spawn();
}

bool CWeaponPZ::Deploy(void)
{
	return BaseClass::Deploy();
}

void CWeaponPZ::PrimaryAttack()
{
	auto owner = ToBasePlayer(GetOwner());

	if (owner)
	{
		if (!m_iClip1 && !ClientWantsAutoReload(GetOwner()))
		{
			return;
		}
	}

	if ((gpGlobals->curtime - m_flLastAttackTime) > 0.5f)
	{
		m_nNumShotsFired = 0;
	}
	else
	{
		m_nNumShotsFired++;
	}

	m_flLastAttackTime = gpGlobals->curtime;

	if (owner)
	{
		owner->ViewPunchReset();
	}

	BaseClass::PrimaryAttack();

	m_flAccuracyPenalty += PZ_ACCURACY_SHOT_PENALTY_TIME;
}

void CWeaponPZ::UpdatePenaltyTime()
{
	auto owner = ToBasePlayer(GetOwner());

	if (!owner)
	{
		return;
	}

	if (((owner->m_nButtons & IN_ATTACK) == false) &&
		(m_flSoonestAttack < gpGlobals->curtime))
	{
		m_flAccuracyPenalty -= gpGlobals->frametime;
		m_flAccuracyPenalty = clamp(m_flAccuracyPenalty,
			0.0f, PZ_ACCURACY_MAXIMUM_PENALTY_TIME);
	}
}

void CWeaponPZ::ItemPreFrame()
{
	UpdatePenaltyTime();

	BaseClass::ItemPreFrame();
}

void CWeaponPZ::ItemBusyFrame()
{
	UpdatePenaltyTime();

	BaseClass::ItemBusyFrame();
}

void CWeaponPZ::ItemPostFrame()
{
	BaseClass::ItemPostFrame();

	if (m_bInReload)
	{
		return;
	}

	auto owner = ToBasePlayer(GetOwner());

	if (!owner)
	{
		return;
	}

	if (m_iClip1 <= 0)
	{
		return;
	}

	if (owner->m_nButtons & IN_ATTACK)
	{
		if (m_flSoonestAttack < gpGlobals->curtime)
		{
			if (m_iClip1 <= 0)
			{
				DryFire();

				m_flSoonestAttack = gpGlobals->curtime + PZ_FASTEST_DRY_REFIRE_TIME;
			}
			else
			{
				m_flSoonestAttack = gpGlobals->curtime + PZ_FASTEST_REFIRE_TIME;
			}
		}
	}
}

float CWeaponPZ::GetFireRate()
{
	return PZ_FASTEST_REFIRE_TIME;
}

Activity CWeaponPZ::GetPrimaryAttackActivity()
{
	if (m_nNumShotsFired < 1)
	{
		return ACT_VM_PRIMARYATTACK;
	}

	if (m_nNumShotsFired < 2)
	{
		return ACT_VM_RECOIL1;
	}

	if (m_nNumShotsFired < 3)
	{
		return ACT_VM_RECOIL2;
	}

	return ACT_VM_RECOIL3;
}

bool CWeaponPZ::Reload()
{
	bool fRet = BaseClass::Reload();

	if (fRet)
	{
		WeaponSound(RELOAD);
		m_flAccuracyPenalty = 0;
	}

	return fRet;
}

void CWeaponPZ::AddViewKick()
{
	auto owner = ToBasePlayer(GetOwner());

	if (!owner)
	{
		return;
	}

	QAngle viewPunch;

	viewPunch.x = SharedRandomFloat("pzx", 0.25f, 0.5f);
	viewPunch.y = SharedRandomFloat("pzy", -0.6f, 0.6f);
	viewPunch.z = 0;

	owner->ViewPunch(viewPunch);
}
