#include "stdafx.h"
#include "..\Public\PlayerBullet.h"
#include "Weapon.h"
#include "Player.h"
#include "GameInstance.h"

CPlayerBullet::CPlayerBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

CPlayerBullet::CPlayerBullet(const CPlayerBullet & rhs)
	: CBaseObj(rhs)
{
}

HRESULT CPlayerBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerBullet::Initialize(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_BulletDesc, pArg, sizeof(BULLETDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	switch (m_BulletDesc.eBulletType)
	{
	case SLASH:
		Set_Scale(_float3(3.f, 3.f, 3.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_BulletDesc.vInitPositon);
		m_pTransformCom->LookDir(m_BulletDesc.vLook);
		m_eShaderID = SHADER_SLASH;
		m_fTexUV = 1.f;
		//m_bSocket = true;
		break;
	default:
		break;
	}

	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_PBULLET, this);
	
	return S_OK;
}

int CPlayerBullet::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_PBULLET, this);
		return OBJ_DEAD;
	}
		

	__super::Tick(fTimeDelta);

	switch (m_BulletDesc.eBulletType)
	{
	case SLASH:
		Moving_SwordBullet(fTimeDelta);
		break;
	case BOW:
		Moving_BowBullet(fTimeDelta);
		break;
	default:
		break;
	}


	return OBJ_NOEVENT;
}

void CPlayerBullet::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	Compute_CamDistance(Get_TransformState(CTransform::STATE_POSITION));
	SetUp_ShaderID();
	//SetUp_BillBoard();
}

HRESULT CPlayerBullet::Render()
{

	if (nullptr == m_pShaderCom )
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr)
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_eShaderID)))
				return E_FAIL;
		}

	}
	
	if (m_pTextureCom != nullptr)
	{
		if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV())))
			return E_FAIL;

		m_pShaderCom->Begin(m_eShaderID);

		m_pVIBufferCom->Render();

	}
		

	return S_OK;
}

HRESULT CPlayerBullet::Ready_Components(void * pArg)
{
	LEVEL iLevel = (LEVEL)CGameInstance::Get_Instance()->Get_DestinationLevelIndex();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.0f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Model"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	switch (m_BulletDesc.eBulletType)
	{
	case SLASH:
	{
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_RollCut"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_OBB*/
		CCollider::COLLIDERDESC		ColliderDesc;
		ColliderDesc.vScale = _float3(1.f, 0.2f, 0.4f);
		ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
		ColliderDesc.vPosition = _float3(0.f, 0.f, 0.5f);
		if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
			return E_FAIL;
		break;
	}
	case BOW:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), iLevel, TEXT("Prototype_Component_Model_Octorock"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	

	return S_OK;
}

HRESULT CPlayerBullet::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_TexUV", &m_fTexUV, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayerBullet::SetUp_ShaderID()
{
	return S_OK;
}

void CPlayerBullet::Change_Animation(_float fTimeDelta)
{
}


void CPlayerBullet::Moving_SwordBullet(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;
	m_fTexUV -= 0.07f;
	if (m_fTexUV <= 0.f)
		m_fTexUV = 0.f;
	
	if (m_BulletDesc.fDeadTime < m_fDeadtime)
		m_bDead = true;
}

void CPlayerBullet::Moving_BowBullet(_float fTimeDelta)
{
}

CPlayerBullet * CPlayerBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerBullet*	pInstance = new CPlayerBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayerBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerBullet::Clone(void * pArg)
{
	CPlayerBullet*	pInstance = new CPlayerBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayerBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerBullet::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom); 
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pSPHERECom);
}
