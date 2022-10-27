#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"

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

	return S_OK;
}

int CPlayer::Tick(_float fTimeDelta)
{
	Key_Input(fTimeDelta);
	
	m_pModelCom->Set_CurrentAnimIndex(m_iAnimNum);
	m_pModelCom->Play_Animation(fTimeDelta);

	
	return OBJ_NOEVENT;
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	CGameInstance* pGameInstacne = GET_INSTANCE(CGameInstance);

	if (pGameInstacne->Key_Pressing(DIK_LEFT))
	{
		m_eDir[DIR_X] = -1.f;
		Change_Direction();
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else if (pGameInstacne->Key_Pressing(DIK_RIGHT))
	{
		m_eDir[DIR_X] = 1.f;
		Change_Direction();
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else
		m_eDir[DIR_X] = 0.f;




	if (pGameInstacne->Key_Pressing(DIK_DOWN))
	{
		m_eDir[DIR_Z] = -1.f;
		Change_Direction();
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else if (pGameInstacne->Key_Pressing(DIK_UP))
	{
		m_eDir[DIR_Z] = 1.f;
		Change_Direction();
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else
		m_eDir[DIR_Z] = 0.f;




	if (m_eDir[DIR_X] == 0 && m_eDir[DIR_Z] == 0)
	{
		m_iAnimNum = IDLE;
	}
	else if (m_eDir[DIR_X] == 0 || m_eDir[DIR_Z] == 0)
	{
		CTransform::TRANSFORMDESC pTransformDesc = m_pTransformCom->Get_TransformDesc();
		pTransformDesc.fSpeedPerSec = 5.f;
		m_pTransformCom->Set_TransformDesc(pTransformDesc);
		m_iAnimNum = RUN;
	}
	else
	{
		CTransform::TRANSFORMDESC pTransformDesc = m_pTransformCom->Get_TransformDesc();
		pTransformDesc.fSpeedPerSec = 2.5f;
		m_pTransformCom->Set_TransformDesc(pTransformDesc);
		m_iAnimNum = RUN;
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

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Link"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

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

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
