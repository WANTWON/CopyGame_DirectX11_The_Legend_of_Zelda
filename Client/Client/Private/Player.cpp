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
	if (CUI_Manager::Get_Instance()->Get_UI_Open() != true)
		Key_Input(fTimeDelta);

	
	
	
	if (m_eAnim != m_ePreAnim)
	{
		m_pModelCom->Set_CurrentAnimIndex(m_eAnim);
		m_ePreAnim = m_eAnim;
	}
	
	Change_Animation(fTimeDelta);
	
		

	
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
	{
		m_eDir[DIR_X] = 0.f;
		Change_Direction();
	}
		


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
	{
		m_eDir[DIR_Z] = 0.f;
		Change_Direction();
	}
		
	

	if (pGameInstacne->Key_Up(DIK_X))
	{
		if (m_eAnim == ANIM::SLASH_HOLD_LP)
			m_eAnim = ANIM::SLASH_HOLD_ED;
		else
			m_eAnim = ANIM::SLASH;
	}
	if (pGameInstacne->Key_Pressing(DIK_X))
	{
		if (m_eAnim != ANIM::SLASH_HOLD_LP)
			m_eAnim = ANIM::SLASH_HOLD_ST;
	}
	else if (pGameInstacne->Key_Up(DIK_Y))
	{
		if (m_eAnim == ANIM::SHIELD_LP)
			m_eAnim = SHIELD_ED;
		else if(m_eAnim != ANIM::SHIELD_LP)
			m_eAnim = ANIM::SHIELD_ST;
	}


	if (pGameInstacne->Key_Down(DIK_LCONTROL))
	{
		m_eAnim = ANIM::JUMP;
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
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
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

void CPlayer::Render_Model(MESH_NAME eMeshName)
{
	if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", eMeshName, aiTextureType_DIFFUSE)))
		return;

	if (FAILED(m_pModelCom->Render(m_pShaderCom, eMeshName, m_eShaderID)))
		return;
}

void CPlayer::Change_Direction()
{
	__super::Change_Direction();

	if (m_eDir[DIR_X] == 0 && m_eDir[DIR_Z] == 0)
	{
		if (m_eAnim == RUN)
			m_eAnim = IDLE;
	}
	else if (m_eDir[DIR_X] == 0 || m_eDir[DIR_Z] == 0)
	{
		CTransform::TRANSFORMDESC pTransformDesc = m_pTransformCom->Get_TransformDesc();
		pTransformDesc.fSpeedPerSec = 5.f;
		m_pTransformCom->Set_TransformDesc(pTransformDesc);

		if (m_eAnim != JUMP)
				m_eAnim = RUN;
	}
	else
	{
		CTransform::TRANSFORMDESC pTransformDesc = m_pTransformCom->Get_TransformDesc();
		pTransformDesc.fSpeedPerSec = 2.5f;
		m_pTransformCom->Set_TransformDesc(pTransformDesc);
		if (m_eAnim != JUMP)
			m_eAnim = RUN;
	}
}

void CPlayer::Change_Animation(_float fTimeDelta)
{
	switch (m_eAnim)
	{
	case Client::CPlayer::IDLE:
	case Client::CPlayer::RUN:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop);
		break;
	case Client::CPlayer::JUMP:
		m_bIsLoop = false;
		m_pTransformCom->Jump(fTimeDelta, 1.f, 4.5);
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			m_eAnim = IDLE;
		break;
	case Client::CPlayer::SLASH_HOLD_ST:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			m_eAnim = SLASH_HOLD_LP;
		break;
	case Client::CPlayer::SHIELD_ST:
	case Client::CPlayer::SHIELD_HIT:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			m_eAnim = SHIELD_LP;
		break;
	case Client::CPlayer::LAND:
	case Client::CPlayer::SLASH:
	case Client::CPlayer::SHIELD_ED:
	case Client::CPlayer::SLASH_HOLD_ED:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			m_eAnim = IDLE;
		break;
	default:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop);
		break;
	}
	
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
