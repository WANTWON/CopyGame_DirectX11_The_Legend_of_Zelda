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

	Set_Scale(_float3(0.5, 0.5, 0.5));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 4.2f, 10.f, 1.f));

	return S_OK;
}

int CPlayer::Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Get_UI_Open() != true)
		Key_Input(fTimeDelta);

	
	
	
	if (m_eState != m_ePreState)
	{
		m_pModelCom->Set_CurrentAnimIndex(m_eState);
		m_ePreState = m_eState;
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
		if (m_eState == ANIM::SLASH_HOLD_LP)
			m_eState = ANIM::SLASH_HOLD_ED;
		else
		{
			m_eState = ANIM::SLASH;
			m_pModelCom->Set_AnimationReset();
		}
	}
	else if (pGameInstacne->Key_Pressing(DIK_X))
	{
		if (m_eState == ANIM::SLASH)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		
		if (m_eState != ANIM::SLASH_HOLD_LP)
			m_eState = ANIM::SLASH_HOLD_ST;
	}
	else if (pGameInstacne->Key_Up(DIK_Y))
	{
		if (m_eState == ANIM::SHIELD_LP)
			m_eState = SHIELD_ED;
		else if(m_eState != ANIM::SHIELD_LP)
			m_eState = ANIM::SHIELD_ST;
	}


	if (pGameInstacne->Key_Pressing(DIK_LCONTROL))
	{
		if (m_eState != JUMP)
		{
			m_eState = ANIM::JUMP;
			m_fTime = 0.f;
		}
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

	TransformDesc.fSpeedPerSec = 1.f;
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
		if (m_eState == RUN)
			m_eState = IDLE;
	}
	else if (m_eDir[DIR_X] == 0 || m_eDir[DIR_Z] == 0)
	{
		CTransform::TRANSFORMDESC pTransformDesc = m_pTransformCom->Get_TransformDesc();
		pTransformDesc.fSpeedPerSec = 3.0f;
		m_pTransformCom->Set_TransformDesc(pTransformDesc);

		if (m_eState != JUMP)
				m_eState = RUN;
	}
	else
	{
		CTransform::TRANSFORMDESC pTransformDesc = m_pTransformCom->Get_TransformDesc();
		pTransformDesc.fSpeedPerSec = 1.5f;
		m_pTransformCom->Set_TransformDesc(pTransformDesc);
		if (m_eState != JUMP)
			m_eState = RUN;
	}
}

void CPlayer::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CPlayer::IDLE:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop);
		break;
	case Client::CPlayer::RUN:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop);
		break;
	case Client::CPlayer::JUMP:
	{
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_fTime += 0.1f;
		m_pTransformCom->Jump(m_fTime, 3.f, 2.0f, 4.2f);
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = LAND;
		break;
	}
	case Client::CPlayer::SLASH_HOLD_ST:
		m_bIsLoop = false;
		m_eAnimSpeed = 4.f;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = SLASH_HOLD_LP;
		break;
	case Client::CPlayer::SHIELD_ST:
	case Client::CPlayer::SHIELD_HIT:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = SHIELD_LP;
		break;
	case Client::CPlayer::LAND:
		m_eAnimSpeed = 3.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = IDLE;
		break;
	case Client::CPlayer::SLASH:
		m_eAnimSpeed = 1.5f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = IDLE;
		break;
	case Client::CPlayer::SHIELD_ED:
	case Client::CPlayer::SLASH_HOLD_ED:
		m_eAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop))
			m_eState = IDLE;
		break;
	default:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_eAnimSpeed, m_bIsLoop);
		break;
	}
	
	m_eAnimSpeed = 1.f;
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
