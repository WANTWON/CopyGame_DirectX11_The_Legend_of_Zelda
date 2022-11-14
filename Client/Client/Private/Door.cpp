#include "stdafx.h"
#include "..\Public\Door.h"
#include "GameInstance.h"
#include "Player.h"
#include "TreasureBox.h"
#include "Level_TailCave.h"
#include "UIButton.h"

CDoor::CDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

HRESULT CDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor::Initialize(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_DoorDesc, pArg, sizeof(DOORDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_eObjectID = OBJID::OBJ_BLOCK;
	Set_Scale(_float3(1.2f, 1.2f, 1.2f));

	_vector vecPostion = XMLoadFloat3(&m_DoorDesc.InitPosition);
	vecPostion = XMVectorSetW(vecPostion, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPostion);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_DoorDesc.fAngle));

	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_BLOCK, this);
	return S_OK;
}

int CDoor::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_BLOCK, this);
		return OBJ_DEAD;
	}

	switch (m_DoorDesc.eType)
	{
	case DOOR_CLOSED:
		Tick_ClosedDoor(fTimeDelta);
		break;
	case DOOR_KEY:
		Tick_LockDoor(fTimeDelta);
		break;
	case DOOR_BOSS:
		Tick_BossDoor(fTimeDelta);
		break;
	default:
		break;
	}

	
	return OBJ_NOEVENT;
}

void CDoor::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	SetUp_ShaderID();
}

HRESULT CDoor::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_eShaderID)))
			return E_FAIL;
	}

#ifdef _DEBUG
	//m_pAABBCom->Render();
	m_pOBBCom->Render();
	/*m_pSPHERECom->Render();*/
#endif

	return S_OK;
}

void CDoor::Change_Animation(_float fTimeDelta)
{
	switch (m_DoorDesc.eType)
	{
	case DOOR_CLOSED:
		Change_Animation_ClosedDoor(fTimeDelta);
		break;
	case DOOR_KEY:
		Change_Animation_LockDDoor(fTimeDelta);
		break;
	case DOOR_BOSS:
		Change_Animation_BossDoor(fTimeDelta);
		break;
	}
}

HRESULT CDoor::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 2.0f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	switch (m_DoorDesc.eType)
	{
	case DOOR_CLOSED:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_ClosedDoor"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case DOOR_KEY:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_LockDoor"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case DOOR_BOSS:
		break;
	default:
		break;
	}

	

	CCollider::COLLIDERDESC		ColliderDesc;
	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(3.f, 2.f, 0.7f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CDoor::SetUp_ShaderResources()
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

void CDoor::Tick_ClosedDoor(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CLevel* pLevel = pGameInstance->Get_CurrentLevel();
	m_bOpen = dynamic_cast<CLevel_TailCave*>(pLevel)->Get_OpenDoor();
	if (m_bOpen)
	{
		if (m_eState != OPEN_WAIT_CD)
		{
			m_eState = OPEN_CD;

			m_bPlay = true;
		}

	}
	else
		m_eState = CLOSE_CD;

	if (m_eState != m_ePreState)
	{
		m_pModelCom->Set_NextAnimIndex(m_eState);
		m_ePreState = m_eState;
	}

	if (m_bPlay)
		Change_Animation(fTimeDelta);

	RELEASE_INSTANCE(CGameInstance);
}

void CDoor::Tick_LockDoor(_float fTimeDelta)
{
	CBaseObj* pPlayer = nullptr;
	if (CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_PLAYER, m_pOBBCom, &pPlayer))
	{
		CUIButton*		pButton = dynamic_cast<CUIButton*>(CUI_Manager::Get_Instance()->Get_Button());
		pButton->Set_Visible(true);
		_float2 fPosition = pPlayer->Get_ProjPosition();
		fPosition.y = g_iWinSizeY - fPosition.y;
		fPosition.x += 50.f;
		fPosition.y -= 30.f;
		pButton->Set_Position(fPosition);

		if (CGameInstance::Get_Instance()->Key_Up(DIK_A))
		{
			pButton->Set_Visible(false);
			dynamic_cast<CPlayer*>(pPlayer)->Set_AnimState(CPlayer::KEY_OPEN);
			m_eState = OPEN_LD;
		}
		else
			m_eState = CLOSE_LD;
	}
	else
		m_eState = CLOSE_LD;
}

void CDoor::Tick_BossDoor(_float fTimeDelta)
{
}

void CDoor::Change_Animation_ClosedDoor(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CDoor::CLOSE_CD:
	case Client::CDoor::CLOSE2_CD:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
		{
			if (m_bPlay)
				CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_BLOCK, this);
			m_bPlay = false;
		}
		break;
	case Client::CDoor::OPEN_CD:
	case Client::CDoor::OPEN2_CD:
		if (m_bOpen)
		{
			m_bIsLoop = false;
			if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			{
				CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_BLOCK, this);
				m_eState = OPEN_WAIT_CD;
			}

		}
		break;
	case Client::CDoor::OPEN_WAIT_CD:
	case Client::CDoor::OPEN_WAIT2_CD:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop);
		break;
	default:
		break;
	}
}

void CDoor::Change_Animation_LockDDoor(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CDoor::CLOSE_LD:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop);
		break;
	case Client::CDoor::OPEN_LD:
	case Client::CDoor::OPEN2_LD:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			m_bDead = true;
		break;
	default:
		break;
	}
}

void CDoor::Change_Animation_BossDoor(_float fTimeDelta)
{
}

CDoor * CDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDoor*	pInstance = new CDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDoor::Clone(void * pArg)
{
	CDoor*	pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor::Free()
{
	__super::Free();

	CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_BLOCK, this);
	Safe_Release(m_pModelCom);
}
