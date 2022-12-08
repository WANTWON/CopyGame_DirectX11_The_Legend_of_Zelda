#include "stdafx.h"
#include "..\Public\Player.h"
#include "PlayerBullet.h"
#include "GameInstance.h"
#include "MonsterBullet.h"
#include "Weapon.h"
#include "PipeLine.h"
#include "Cell.h"
#include "CameraManager.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CBaseObj(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(36.3f, m_fStartHeight, 46.8f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 4.2f, 10.f, 1.f));
	
	Compute_CurrentIndex(LEVEL_GAMEPLAY);

	m_fWalkingHeight = m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION), (Get_Scale().y * 0.5f));
	m_fStartHeight = m_fWalkingHeight;
	m_fEndHeight = m_fWalkingHeight;

	Set_Scale(_float3(1.2f, 1.2f, 1.2f));
	
	m_tInfo.iMaxHp = 52;
	m_tInfo.iDamage = 20;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_Parts.resize(PARTS_END);
	if (FAILED(Ready_Parts(CPrizeItem::BOW, PARTS_BOW)))
		return E_FAIL;

	m_pModelCom->Set_CurrentAnimIndex(m_eState);
	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_PLAYER, this);
	
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//	CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);
	//	char cName[MAX_PATH];
	//	ZeroMemory(cName, sizeof(char) * MAX_PATH);
	//	pData_Manager->TCtoC(TEXT("Link"), cName);
	//	pData_Manager->Conv_Bin_Model(m_pModelCom, cName, CData_Manager::DATA_ANIM);
	//	//ERR_MSG(TEXT("Save_Bin_Model"));
	//	RELEASE_INSTANCE(CData_Manager);

	//RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

int CPlayer::Tick(_float fTimeDelta)
{
	m_iCurrentLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	if (m_iCurrentLevel == LEVEL_LOADING)
		return OBJ_NOEVENT;

	if (CUI_Manager::Get_Instance()->Get_UI_Open() != true )
	{
		Key_Input(fTimeDelta);
		Change_Direction(fTimeDelta);
		Check_Navigation(fTimeDelta);
		Get_ProjPosition();
	}

	if (m_eState != m_ePreState)
	{
		m_pModelCom->Set_NextAnimIndex(m_eState);
		m_ePreState = m_eState;
	}

	if(!m_bDead)
		Change_Animation(fTimeDelta);

	for (auto& pParts : m_Parts)
	{
		if(pParts != nullptr)
			pParts->Tick(fTimeDelta);
	}
		

	if (m_dwHitTime + 100 < GetTickCount())
		m_bHit = false;

	return OBJ_NOEVENT;
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (m_iCurrentLevel == LEVEL_LOADING)
		return;


	for (auto& pParts : m_Parts)
	{
		if(pParts != nullptr)
			pParts->Late_Tick(fTimeDelta);
	}
		


	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_BOW]);

		if (m_Parts[PARTS_ITEM] != nullptr)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_ITEM]);
			dynamic_cast<CWeapon*>(m_Parts[PARTS_ITEM])->Set_Scale(_float3(2.f, 2.f, 2.f));
		}
			


#ifdef _DEBUG
		if (m_pAABBCom != nullptr)
			m_pRendererCom->Add_Debug(m_pAABBCom);
		if (m_pOBBCom != nullptr)
			m_pRendererCom->Add_Debug(m_pOBBCom);
		if (m_pSPHERECom != nullptr)
			m_pRendererCom->Add_Debug(m_pSPHERECom);
		if (m_pNavigationCom != nullptr)
			m_pRendererCom->Add_Debug(m_pNavigationCom);
#endif

	}

	CBaseObj* pCollisionBlock = nullptr;
	if (CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_BLOCK, m_pOBBCom, &pCollisionBlock))
	{
		if (m_eState == PUSH_LP)
			return;

		_vector vDirection = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pCollisionBlock->Get_TransformState(CTransform::STATE_POSITION);
		if (fabs(XMVectorGetX(vDirection)) > fabs(XMVectorGetZ(vDirection)))
			vDirection = XMVectorSet(XMVectorGetX(vDirection), 0.f, 0.f, 0.f);
		else
			vDirection = XMVectorSet(0.f, 0.f, XMVectorGetZ(vDirection), 0.f);
		m_pTransformCom->Go_PosDir(fTimeDelta, vDirection, m_pNavigationCom);
	}

	CBaseObj* pCollisionTrap = nullptr;
	if ( CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_TRAP, m_pOBBCom, &pCollisionTrap))
	{
		CMonsterBullet::BULLETDESC eBulletDesc;

		eBulletDesc.eOwner = CMonster::MONSTER_CUCCO;
		eBulletDesc.eBulletType = CMonsterBullet::DEFAULT;
		eBulletDesc.vLook = pCollisionTrap->Get_TransformState(CTransform::STATE_LOOK);

		if(m_eState != DMG_B && m_eState != DMG_F)
			Take_Damage(1.f, &eBulletDesc, nullptr);
	}
		
}

HRESULT CPlayer::Render()
{
	if (m_iCurrentLevel == LEVEL_LOADING)
		return E_FAIL;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		Render_Model((MESH_NAME)i);
	}

	/*Render_Model(MESH_HAIR);
	Render_Model(MESH_CLOTHES);
	Render_Model(MESH_EAR);
	Render_Model(MESH_HAT);
	Render_Model(MESH_FACE);
	Render_Model(MESH_SHOES);
	Render_Model(MESH_MOUSE);
	Render_Model(MESH_BELT);

	Render_Model(m_eLeftHand);
	Render_Model(m_eRightHand);
	*/

	return S_OK;
}

_uint CPlayer::Get_PartsItemType()
{
	if (m_Parts[PARTS_ITEM] == nullptr)
		return CWeapon::NONE;

	return dynamic_cast<CWeapon*>(m_Parts[PARTS_ITEM])->Get_PartsType();
}

_bool CPlayer::Set_RubyUse(_int iCoin)
{
	if (m_tInfo.iCoin < iCoin)
		return false;
		
	m_tInfo.iCoin -= iCoin;
	return true;
}

void CPlayer::Set_2DMode(_bool type)
{
	 m_b2D = type;  
	 m_pNavigationCom->Set_2DNaviGation(type); 
	 m_pNavigationCom->Compute_CurrentIndex_byDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}



void CPlayer::Compute_CurrentIndex(LEVEL eLevel)
{
	m_pNavigationCom->Compute_CurrentIndex_byDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

_uint CPlayer::Take_Damage(float fDamage, void * DamageType, CBaseObj * DamageCauser)
{
	if (m_eState == DMG_B || m_eState == DMG_F ||
		m_eState == DMG_PRESS || m_eState == DMG_QUAKE ||
		m_eState == DEAD  || m_bHit)
		return 0;

	if (fDamage <= 0 || m_bDead)
		return 0;

	m_tInfo.iCurrentHp -= (int)fDamage;
	m_bHit = true;
	m_dwHitTime = GetTickCount();

	if (m_tInfo.iCurrentHp <= 0)
	{
		m_tInfo.iCurrentHp = 0;
		m_eState = DEAD;
	}
	

	if (DamageType == nullptr)
	{
		m_eState = CPlayer::DMG_F;
		return 0;
	}
		

	CMonsterBullet::BULLETDESC BulletDesc;
	memcpy(&BulletDesc, DamageType, sizeof(CMonsterBullet::BULLETDESC));

	if (BulletDesc.eBulletType == CMonsterBullet::DEFAULT && BulletDesc.eOwner == CMonster::MONSTER_ROLA)
	{
		m_eState = DMG_PRESS;
	}
	else
	{
		_vector BulletLook = BulletDesc.vLook;
		_vector PlayerLook = Get_TransformState(CTransform::STATE_LOOK);
		_vector fDot = XMVector3Dot(BulletLook, PlayerLook);
		_float fAngleRadian = acos(XMVectorGetX(fDot));
		_float fAngleDegree = XMConvertToDegrees(fAngleRadian);
		_vector vCross = XMVector3Cross(BulletLook, PlayerLook);

		if (fAngleDegree > 0.f && fAngleDegree <= 90.f)
			m_eState = CPlayer::DMG_B;
		else if (fAngleDegree > 90.f && fAngleDegree <= 180.f)
			m_eState = CPlayer::DMG_F;

	}


	return 0;
}

HRESULT CPlayer::Ready_Parts(CPrizeItem::TYPE eType, PARTS PartsIndex)
{
	if (m_Parts[PartsIndex] != nullptr)
		return E_FAIL;

	/* For.Weapon */
	CHierarchyNode*		pSocket = m_pModelCom->Get_BonePtr("itemA_L");
	if (nullptr == pSocket)
		return E_FAIL;


	CWeapon::WEAPONDESC m_WeaponDesc;
	m_WeaponDesc.pSocket = pSocket;
	m_WeaponDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	m_WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	switch (eType)
	{
	case Client::CPrizeItem::ARROW:
		m_WeaponDesc.eType = CWeapon::ARROW;
		break;
	case Client::CPrizeItem::DOGFOOD:
		m_WeaponDesc.eType = CWeapon::DOGFOOD;
		break;
	case Client::CPrizeItem::HEART_CONTAINER:
		m_WeaponDesc.eType = CWeapon::HEART_CONTAINER;
		break;
	case Client::CPrizeItem::MAGIC_ROD:
		m_WeaponDesc.eType = CWeapon::MAGIC_ROD;
		break;
	case Client::CPrizeItem::BOW:
		m_WeaponDesc.eType = CWeapon::BOW;
		break;
	default:
		break;
	}

	Safe_AddRef(pSocket);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_Parts[PartsIndex] = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &m_WeaponDesc);
	if (nullptr == m_Parts[PartsIndex])
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CPlayer::Set_PlayerState_Defaut()
{

	CCamera* pCamera = CCameraManager::Get_Instance()->Get_CurrentCamera();
	if (m_iCurrentLevel == LEVEL_GAMEPLAY)
		dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_CamMode(CCamera_Dynamic::CAM_PLAYER);
	if (m_iCurrentLevel == LEVEL_TAILCAVE)
		dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_CamMode(CCamera_Dynamic::CAM_TERRAIN);
	if (m_iCurrentLevel == LEVEL_ROOM)
		dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_CamMode(CCamera_Dynamic::CAM_ROOM);
	if (m_eState == ITEM_GET_LP)
		m_eState = ITEM_GET_ED;
	if (m_bStop)
		m_bStop = false;
	if (m_bCarry)
	{
		m_bCarry = false;
		Safe_Release(m_Parts[PARTS_ITEM]);
		m_eState = IDLE;
	}
	CUI_Manager::Get_Instance()->Open_Message(false);
}


void CPlayer::Key_Input(_float fTimeDelta)
{

	if (CUI_Manager::Get_Instance()->Get_Talking() == true)
		return;


	if (m_eState == DMG_B || m_eState == DMG_F || m_eState == FALL_FROMTOP ||
		m_eState == FALL_HOLE || m_eState == FALL_ANTLION || m_eState == KEY_OPEN ||
		m_eState == STAIR_DOWN || m_eState == STAIR_UP || m_bStop)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_bDead)
	{
		if (pGameInstance->Key_Up(DIK_SPACE))
		{
			m_eState = IDLE;
			m_bDead = false;
			m_tInfo.iCurrentHp = m_tInfo.iMaxHp;
		}

		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	if (pGameInstance->Key_Pressing(DIK_0))
		m_tInfo.iCoin++;
	if (pGameInstance->Key_Pressing(DIK_MINUS))
	{
		m_tInfo.iCoin--;
		if(m_tInfo.iCoin < 0)
			m_tInfo.iCoin = 0;
	}
	if (pGameInstance->Key_Up(DIK_8))
	{
		m_tInfo.iCurrentHp--;
		if (m_tInfo.iCurrentHp < 0)
			m_tInfo.iCurrentHp = 0;
	}



	if (m_eState == ITEM_GET_ST || m_eState == ITEM_GET_LP)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


	/* Move Left and Right*/
	if (pGameInstance->Key_Down(DIK_LEFT))
	{
		if (m_eState != DASH_LP)
		{
			m_iDash[DIR_X]--;
			m_dwDashTime = GetTickCount();
		}

	}
	else if (pGameInstance->Key_Down(DIK_RIGHT))
	{
		if (m_eState != DASH_LP)
		{
			m_iDash[DIR_X]++;
			m_dwDashTime = GetTickCount();
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_LEFT))
		m_eDir[DIR_X] = -1;
	else if (pGameInstance->Key_Pressing(DIK_RIGHT))
		m_eDir[DIR_X] = 1;
	else
		m_eDir[DIR_X] = 0;


	/* Move Up And Down*/
	if (pGameInstance->Key_Down(DIK_DOWN))
	{
		if (m_eState != DASH_LP)
		{
			m_iDash[DIR_Z]--;
			m_dwDashTime = GetTickCount();
		}
	}
	else if (pGameInstance->Key_Down(DIK_UP))
	{
		if (m_eState != DASH_LP)
		{
			m_iDash[DIR_Z]++;
			m_dwDashTime = GetTickCount();
		}
	}
	if (pGameInstance->Key_Pressing(DIK_DOWN))
		m_eDir[DIR_Z] = -1;
	else if (pGameInstance->Key_Pressing(DIK_UP))
		m_eDir[DIR_Z] = 1;
	else
		m_eDir[DIR_Z] = 0;


	/* Use X Key & Y Key (Attack and Use Item)*/
	if (pGameInstance->Key_Up(DIK_Z))
	{
		/* Special Sword Attack */
		if (m_eState == ANIM::SLASH_HOLD_LP || m_eState == ANIM::SLASH_HOLD_F ||
			m_eState == ANIM::SLASH_HOLD_L || m_eState == ANIM::SLASH_HOLD_B || m_eState == ANIM::SLASH_HOLD_R)
			m_eState = ANIM::SLASH_HOLD_ED;
		else
		{
			if(m_b2D)
				m_eState = S_SLASH;
			else
				m_eState = SLASH;
			m_pModelCom->Set_AnimationReset();
		}
	}
	else if (pGameInstance->Key_Pressing(DIK_Z))
	{
		if (m_eState == IDLE)
			m_eState = ANIM::SLASH_HOLD_ST;
		else if (m_eState == SLASH_HOLD_B || m_eState == SLASH_HOLD_F || m_eState == SLASH_HOLD_R || m_eState == SLASH_HOLD_L)
			m_eState = SLASH_HOLD_LP;
	}
	else if (pGameInstance->Key_Up(DIK_Y))
	{
		if (m_eState == ANIM::SHIELD_HOLD_LP)
			m_eState = SHIELD_HOLD_ED;
		else if (m_eState != ANIM::SHIELD_HOLD_LP)
			m_eState = ANIM::SHIELD;
	}


	/*Jump_Key*/
	if (pGameInstance->Key_Down(DIK_LCONTROL) && m_eState != DASH_LP)
	{
		if (m_eState == JUMP)
		{
			m_eState = ANIM::D_JUMP;
			m_fTime = 0.f;
			m_fDoubleHeight = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}

		if (m_eState != JUMP && m_eState != D_JUMP && m_eState != D_FALL)
		{
			m_eState = ANIM::JUMP;
			m_fTime = 0.f;
			m_fStartHeight = m_fEndHeight;
		}
	}


	if (m_dwDashTime + 150 < GetTickCount())
	{
		if (m_eState != DASH_ST && m_eState != DASH_LP && m_eState != DASH_ED)
		{
			m_iDash[DIR_Z] = 0;
			m_iDash[DIR_X] = 0;
		}

	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Change_Navigation(LEVEL eLevel)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (eLevel)
	{
	case Client::LEVEL_GAMEPLAY:
		m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Navigation_Field")));
		break;
	case Client::LEVEL_ROOM:
	{
		CUI_Manager::ROOMTYPE eRoomType = CUI_Manager::Get_Instance()->Get_RoomType();
		if (eRoomType == CUI_Manager::MARINHOUSE)
			m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Navigation_Room")));
		else if (eRoomType == CUI_Manager::SHOP)
			m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Navigation_Shop")));
		else if (eRoomType == CUI_Manager::CRANEGAME)
			m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Navigation_CraneGame")));
		break;
	}
	case Client::LEVEL_TAILCAVE:
		m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Navigation_TailCave")));
		break;
	case Client::LEVEL_TOWER:
		m_pNavigationCom = dynamic_cast<CNavigation*>(pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Navigation_Tower")));
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}


HRESULT CPlayer::Ready_Components(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Link"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_OBB*/
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vScale = _float3(1.f, 1.5f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		//return E_FAIL;

		/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation_TailCave"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_TailCave"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;
	m_vecNavigaitions.push_back(m_pNavigationCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Room"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Room"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;
	m_vecNavigaitions.push_back(m_pNavigationCom);
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Shop"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Shop"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;
	m_vecNavigaitions.push_back(m_pNavigationCom);
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Field"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Field"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;
	m_vecNavigaitions.push_back(m_pNavigationCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation_CraneGame"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_CraneGame"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;
	m_vecNavigaitions.push_back(m_pNavigationCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Tower"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Tower"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;
	m_vecNavigaitions.push_back(m_pNavigationCom);

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderID()
{
	return S_OK;
}


void CPlayer::Render_Model(MESH_NAME eMeshName)
{
	if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", eMeshName, aiTextureType_DIFFUSE)))
		return;

	if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_NormalTexture", eMeshName, aiTextureType_NORMALS)))
		return;

	if (FAILED(m_pModelCom->Render(m_pShaderCom, eMeshName, m_eShaderID)))
		return;
}

void CPlayer::Change_Direction(_float fTimeDelta)
{
	if (m_eState == DMG_B || m_eState == DMG_F || m_eState == DMG_PRESS || m_eState == DMG_QUAKE ||
		m_eState == ITEM_GET_ST || m_eState == ITEM_GET_LP || m_eState == FALL_FROMTOP || m_eState == FALL_HOLE ||
		m_eState == FALL_ANTLION || m_eState == KEY_OPEN || m_eState == STAIR_DOWN || m_eState == STAIR_UP || 
		m_eState == LADDER_UP_ED || m_eState == ITEM_CARRY || m_bDead )
		return;

	if (m_eState == SLASH_HOLD_ED || m_eState == DASH_ST || m_eState == DASH_ED)
		return;

	if (m_eState == PUSH_LP || m_eState == PUSH_WAIT || m_eState == PULL_LP)
		SetDirection_Pushing(fTimeDelta);
	else if (m_eState == SLASH_HOLD_LP || m_eState == SLASH_HOLD_ST || m_eState == SLASH_HOLD_B ||
		m_eState == SLASH_HOLD_F || m_eState == SLASH_HOLD_L || m_eState == SLASH_HOLD_R)
		SetDirection_byPosition(fTimeDelta);
	else
		SetDirection_byLook(fTimeDelta);
}

void CPlayer::SetDirection_byLook(_float fTimeDelta)
{
	CTransform::TRANSFORMDESC TransformDesc = m_pTransformCom->Get_TransformDesc();
	TransformDesc.fSpeedPerSec = m_eState == DASH_LP ? 7.f : 5.f;
	m_pTransformCom->Set_TransformDesc(TransformDesc);


	__super::Change_Direction();

	if (m_eDir[DIR_X] == 0 && m_eDir[DIR_Z] == 0)
	{
		if (m_bUpDown)
		{
			m_eState = LADDER_WAIT;
			m_pTransformCom->LookDir(XMVectorSet(0.f, 0.f, 1.f, 0.f));
		}
		else if (m_bCarry)
		{
			m_eState = IDLE_CARRY;
		}
		else
		{
			if (m_eState == RUN) m_eState = IDLE;
			if (m_eState == DASH_LP) m_eState = DASH_ED;
		}
		
	}
	else
	{
		if (m_eState != JUMP && m_eState != D_JUMP && m_eState != D_FALL)
		{
			if (abs(m_iDash[DIR_X]) > 1 || abs(m_iDash[DIR_Z]) > 1)
			{
				if (m_eState != DASH_LP)
					m_eState = DASH_ST;
			}
			else if (m_bCarry)
				m_eState = WALK_CARRY;
			else
				m_eState = RUN;
		}

		if (m_b2D)
		{
			if (m_bUpDown)
			{
				if (m_eDir[DIR_X] == 0 && m_eDir[DIR_Z] != 0)
				{
					m_eState = LADDER_UP;
					m_pTransformCom->LookDir(XMVectorSet(0.f, 0.f, 1.f, 0.f));
				}

				_vector vDir = XMVectorSet(m_eDir[DIR_X], m_eDir[DIR_Z], 0.f, 0.f);
				m_pTransformCom->Go_PosDir(fTimeDelta, vDir, m_pNavigationCom);
			}
			else
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}	
		else
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}

}

void CPlayer::SetDirection_byPosition(_float fTimeDelta)
{

	CTransform::TRANSFORMDESC TransformDesc = m_pTransformCom->Get_TransformDesc();
	TransformDesc.fSpeedPerSec = 2.5f;
	m_pTransformCom->Set_TransformDesc(TransformDesc);

	///////////////// 이 부분에서 나의 룩벡터와 Direction Vector를 통해 각도로 방향 세팅 예정
	if (m_eDir[DIR_X] > 0)
		m_eState = SLASH_HOLD_R;
	else if (m_eDir[DIR_X] < 0)
		m_eState = SLASH_HOLD_L;

	if (m_eDir[DIR_Z] > 0)
		m_eState = SLASH_HOLD_F;
	else if (m_eDir[DIR_Z] < 0)
		m_eState = SLASH_HOLD_B;

	_vector vDirection = XMVectorSet((_float)m_eDir[DIR_X], 0.f, (_float)m_eDir[DIR_Z], 0.f);
	m_pTransformCom->Go_PosDir(fTimeDelta, vDirection);


}

void CPlayer::SetDirection_Pushing(_float fTimeDelta)
{

	if (m_eDir[DIR_X] == 0 && m_eDir[DIR_Z] == 0)
	{
		m_eState = PUSH_WAIT;
	}
	else
	{
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		if (fabs(XMVectorGetX(vLook)) > fabs(XMVectorGetZ(vLook)))
			vLook = XMVectorGetX(vLook) > 0 ? XMVectorSet(1.f, 0.f, 0.f, 0.f) : XMVectorSet(-1.f, 0.f, 0.f, 0.f);
	
		else
			vLook = XMVectorGetZ(vLook) > 0 ? XMVectorSet(0.f, 0.f, 1.f, 0.f) : XMVectorSet(0.f, 0.f, -1.f, 0.f);

		_vector vNewLook = XMVectorSet(m_eDir[DIR_X], 0.f, m_eDir[DIR_Z], 0.f);

		if (0 > XMVectorGetX(XMVector3Dot(vLook, vNewLook)))
		{
			m_eState = PULL_LP;
			m_pTransformCom->Go_Backward(fTimeDelta*0.1f, m_pNavigationCom);
		}
		else
		{
			m_eState = PUSH_LP;
			m_pTransformCom->Go_Straight(fTimeDelta*0.2f, m_pNavigationCom);
		}
		
	}
}

void CPlayer::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CPlayer::IDLE:
	case Client::CPlayer::IDLE_CARRY:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop);
		break;
	case Client::CPlayer::WALK_CARRY:
	case Client::CPlayer::RUN:
	case Client::CPlayer::LADDER_UP:
	case Client::CPlayer::LADDER_WAIT:
	case Client::CPlayer::DASH_LP:
	case Client::CPlayer::SLASH_HOLD_F:
	case Client::CPlayer::SLASH_HOLD_B:
	case Client::CPlayer::SLASH_HOLD_L:
	case Client::CPlayer::SLASH_HOLD_R:
	case Client::CPlayer::SLASH_HOLD_LP:
	case Client::CPlayer::ITEM_GET_LP:
	case Client::CPlayer::PULL_LP:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop);
		break;
	case Client::CPlayer::PUSH_LP:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop);
		break;
	case Client::CPlayer::PUSH_WAIT:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop);
		break;
	case Client::CPlayer::JUMP:
	{
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		m_fTime += 0.1f;
		m_pTransformCom->Jump(m_fTime, 3.f, 2.0f, m_fStartHeight, m_fEndHeight);
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
		{
			m_eState = LAND;
			_vector vPosition = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fEndHeight);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
		break;
	}
	case Client::CPlayer::D_JUMP:
	{
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		m_fTime += 0.1f;
		m_pTransformCom->Jump(m_fTime, 3.f, 2.0f, m_fDoubleHeight, m_fEndHeight);
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = D_FALL;
		break;
	}
	case Client::CPlayer::D_FALL:
	{
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		m_fTime += 0.1f;
		m_pTransformCom->Jump(m_fTime, 3.f, 2.0f, m_fStartHeight, m_fEndHeight);
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = D_LAND;
		if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) <= m_fEndHeight)
			m_eState = D_LAND;
		break;
	}
	case Client::CPlayer::FALL:
	{
		m_eAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop);
		m_pTransformCom->Go_PosDir(fTimeDelta, XMVectorSet(0.f, -1.f, 0.f, 0.f), m_pNavigationCom);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	/*	if (m_fWalkingHeight >= XMVectorGetY(vPosition))
		{
			vPosition = XMVectorSetY(vPosition, m_fWalkingHeight);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
			m_eState = LAND;
		}*/
		break;
	}
	case Client::CPlayer::SLASH_HOLD_ST:
		m_bIsLoop = false;
		m_eAnimSpeed = 4.f;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = SLASH_HOLD_LP;
		break;
	case Client::CPlayer::SHIELD:
	case Client::CPlayer::SHIELD_HIT:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = SHIELD_HOLD_LP;
		break;
	case Client::CPlayer::LAND:
	case Client::CPlayer::D_LAND:
		m_eAnimSpeed = 3.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = IDLE;
		break;
	case Client::CPlayer::DMG_PRESS:
		m_eAnimSpeed = 2.f;
		if (m_fPressedScale > 0.1f && !m_bPressed)
		{
			m_fPressedScale -= 0.1f;
			Set_Scale(_float3(1.f, m_fPressedScale, 1.f));

			if (m_fPressedScale <= 0.1f)
			{
				m_bPressed = true;
				m_dwPressedTime = GetTickCount();
			}
		}

		if (m_bPressed && m_dwPressedTime + 1000 < GetTickCount())
		{
			if (m_fPressedScale < 1.f)
			{
				m_fPressedScale += 0.1f;
				Set_Scale(_float3(1.f, m_fPressedScale, 1.f));
			}
		}
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
		{
			m_fPressedScale = 1.f;
			m_bPressed = false;
			m_eState = IDLE;
			Set_Scale(_float3(1.f, m_fPressedScale, 1.f));
		}

		break;
	case Client::CPlayer::DMG_B:
		m_eAnimSpeed = 1.5f;
		m_bIsLoop = false;
		m_pTransformCom->Go_Straight(fTimeDelta*0.4f);
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = IDLE;
		break;
	case Client::CPlayer::DMG_F:
	case Client::CPlayer::DMG_QUAKE:
		m_eAnimSpeed = 1.5f;
		m_bIsLoop = false;
		m_pTransformCom->Go_Backward(fTimeDelta*0.1f);
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = IDLE;
		break;
	case Client::CPlayer::SLASH:
	case Client::CPlayer::S_SLASH:
	case Client::CPlayer::KEY_OPEN:
		m_eAnimSpeed = 2.5f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = IDLE;
		break;
	case Client::CPlayer::DASH_ST:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = DASH_LP;
		break;
	case Client::CPlayer::ITEM_GET_ST:
	{
		CCamera* pCamera =  CCameraManager::Get_Instance()->Get_CurrentCamera();
		dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_CamMode(CCamera_Dynamic::CAM_ITEMGET);
		m_pTransformCom->LookDir(XMVectorSet(0.f, 0.f, -1.f ,0.f));
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = ITEM_GET_LP;
		break;
	}
	case Client::CPlayer::DASH_ED:
	case Client::CPlayer::SHIELD_HOLD_ED:
	case Client::CPlayer::SLASH_HOLD_ED:
		m_eAnimSpeed = 4.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = IDLE;
		break;
	case Client::CPlayer::ITEM_GET_ED:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = IDLE;
		break;
	case Client::CPlayer::FALL_HOLE:
	case Client::CPlayer::FALL_ANTLION:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		//m_pTransformCom->Go_PosDir(fTimeDelta, XMVectorSet(0.f, -0.1f, 0.f, 0.f), m_pNavigationCom[m_iCurrentLevel]);
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
		{
			m_eState = FALL_FROMTOP;
			m_pTransformCom->Go_Backward(fTimeDelta * 20, m_pNavigationCom);
		}
		break;
	case Client::CPlayer::FALL_FROMTOP:
	case Client::CPlayer::STAIR_UP:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
		{
			m_eState = IDLE;

		}
		break;
	case Client::CPlayer::STAIR_DOWN:
	case Client::CPlayer::LADDER_UP_ED:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
		{
			_vector vPortalPos = XMLoadFloat3(&m_vPortalPos);
			vPortalPos = XMVectorSetW(vPortalPos, 1.f);

			if (!m_b2D)
			{
				m_eState = FALL_FROMTOP;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPortalPos);
				if (CCameraManager::Get_Instance()->Get_CamState() != CCameraManager::CAM_DYNAMIC)
				{
					CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_DYNAMIC);
					CCamera* pCamera = CCameraManager::Get_Instance()->Get_CurrentCamera();
					dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_Position(vPortalPos);
				}
					
				m_pNavigationCom->Set_2DNaviGation(false);
			}
			else
			{
				m_eState = IDLE;
				vPortalPos = XMVectorSetY(vPortalPos, XMVectorGetY(vPortalPos) - 1.f);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPortalPos);
				CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_2D);
				m_pNavigationCom->Set_2DNaviGation(true);
				m_pNavigationCom->Compute_CurrentIndex_byDistance(vPortalPos);
			}
			
		}
		break;
	case Client::CPlayer::ITEM_CARRY:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
		{
			m_eState = IDLE_CARRY;
			m_bCarry = true;
		}
		break;
	case Client::CPlayer::DEAD:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
		{
			m_bDead = true;
		}
		break;
	default:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop);
		break;
	}

	m_eAnimSpeed = 1.f;
}

void CPlayer::Check_Navigation(_float fTimeDelta)
{
	if (m_pNavigationCom->Get_CurrentCellIndex() == -1)
		return;


	if (m_pNavigationCom->Get_CurrentCelltype() == CCell::DROP)
	{
		if (m_eState != FALL_ANTLION)
			m_pTransformCom->Go_Straight(fTimeDelta * 10, m_pNavigationCom);
		m_eState = FALL_ANTLION;
	}
	else if (m_pNavigationCom->Get_CurrentCelltype() == CCell::ACCESSIBLE)
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float m_fWalkingHeight = m_pNavigationCom->Compute_Height(vPosition, 0.f);
		
		if(m_eState != D_FALL &&  m_eState != JUMP)
				m_fStartHeight = m_fWalkingHeight;
		m_fEndHeight = m_fWalkingHeight;

		if (m_eState != JUMP && m_eState != D_JUMP && m_eState != D_FALL && m_eState != LAND && m_eState != D_LAND)
		{
			if (m_fWalkingHeight < XMVectorGetY(vPosition) && m_b2D)
				m_eState = FALL;
			else if (m_fWalkingHeight < XMVectorGetY(vPosition))
			{
				vPosition = XMVectorSetY(vPosition, m_fWalkingHeight);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
			}

		}
	
		if (m_fWalkingHeight > XMVectorGetY(vPosition))
		{
			if (m_eState == FALL)
				m_eState = LAND;
			vPosition = XMVectorSetY(vPosition, m_fWalkingHeight);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}

	}
	else if (m_pNavigationCom->Get_CurrentCelltype() == CCell::ONLYJUMP)
	{
		
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float m_fWalkingHeight = m_pNavigationCom->Compute_Height(vPosition, 0.f);
		
		m_fStartHeight = m_fWalkingHeight;
		m_fEndHeight = m_fWalkingHeight -2;

		
		if (m_eState == LAND)
			m_eState = FALL;

		if (m_eState != JUMP && m_eState != D_JUMP && m_eState != D_FALL && m_eState != LAND && m_eState != D_LAND)
		{
			if (m_fWalkingHeight <= XMVectorGetY(vPosition))
				m_eState = FALL;

		}
		
		if (m_fWalkingHeight > XMVectorGetY(vPosition))
		{
			m_pNavigationCom->Compute_CurrentIndex_byHeight(vPosition);
		}
	}


	if (m_pNavigationCom->Get_CurrentCelltype() == CCell::UPDOWN)
	{
		m_bUpDown = true;

		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pNavigationCom->Compute_CurrentIndex_byHeight(vPosition);
		_float m_fWalkingHeight = m_pNavigationCom->Compute_Height(vPosition, 0.f);
		m_fStartHeight = m_fWalkingHeight;
		m_fEndHeight = m_fWalkingHeight;

		
		if (m_iCurrentLevel == LEVEL_TOWER)
		{
			
			if (m_fWalkingHeight > XMVectorGetY(vPosition))
			{
				if (XMVectorGetY(vPosition)< 5.6)
				{
					vPosition = XMVectorSetY(vPosition, 5.4f);
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
				}
				else
				{
					m_pNavigationCom->Compute_CurrentIndex_byHeight(vPosition);
				}

			}
			 if (XMVectorGetY(vPosition) > 16.1)
			{
				m_fWalkingHeight = 16.1f;
				vPosition = XMVectorSetY(vPosition, m_fWalkingHeight);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
			}
			
		}
		else 
		{
			if (m_fWalkingHeight > XMVectorGetY(vPosition))
				m_pNavigationCom->Compute_CurrentIndex_byHeight(vPosition);
		}

	}	
	else
		m_bUpDown = false;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();


	for (auto& iter : m_Parts)
		Safe_Release(iter);

	m_Parts.clear();


	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pSPHERECom);

	for (auto& iter : m_vecNavigaitions)
		Safe_Release(iter);
	m_vecNavigaitions.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);


}
