#include "stdafx.h"
#include "..\Public\TreasureBox.h"
#include "GameInstance.h"
#include "Player.h"
#include "UIButton.h"

CTreasureBox::CTreasureBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

HRESULT CTreasureBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTreasureBox::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_eObjectID = OBJID::OBJ_BLOCK;
	//Set_Scale(_float3(0.5, 0.5, 0.5));

	_vector vecPostion = XMLoadFloat3((_float3*)pArg);
	vecPostion = XMVectorSetW(vecPostion, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPostion);


	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_INTERACT, this);
	return S_OK;
}

int CTreasureBox::Tick(_float fTimeDelta)
{
	if (m_eState != m_ePreState)
	{
		m_pModelCom->Set_NextAnimIndex(m_eState);
		m_ePreState = m_eState;
	}

	Change_Animation(fTimeDelta);

	return OBJ_NOEVENT;
}

void CTreasureBox::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	SetUp_ShaderID();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBaseObj* pTarget = dynamic_cast<CBaseObj*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	CUIButton* pButton = dynamic_cast<CUIButton*>(CUI_Manager::Get_Instance()->Get_Button());
	if (m_pOBBCom->Collision(pTarget->Get_Collider()))
	{
		pButton->Set_Visible(true);
		_float2 fPosition = pTarget->Get_ProjPosition();
		fPosition.x += 50.f;
		fPosition.y += 20.f;
		pButton->Set_Position(fPosition);
		if (pGameInstance->Key_Down(DIK_A))
		{
			pButton->Set_Visible(false);
			if(m_eState == CLOSE_WAIT)
				m_eState = OPEN;
			else if(m_eState == OPEN_WAIT)
				m_eState = CLOSE;
		}
	}
	else
		pButton->Set_Visible(false);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CTreasureBox::Render()
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
	//m_pSPHERECom->Render();
#endif

	return S_OK;
}

void CTreasureBox::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CTreasureBox::APPEAR:
		break;
	case Client::CTreasureBox::CLOSE:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			m_eState = CLOSE_WAIT;
		break;
	case Client::CTreasureBox::CLOSE_WAIT:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta * 2, m_bIsLoop);
		break;
	case Client::CTreasureBox::NOT_OPEN:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta * 2, m_bIsLoop);
		break;
	case Client::CTreasureBox::OPEN:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
		{
			m_eState = OPEN_WAIT;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
			pPlayer->Set_AnimState(CPlayer::ITEM_GET_ST);
			RELEASE_INSTANCE(CGameInstance);
		}
			
		break;
	case Client::CTreasureBox::OPEN_WAIT:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta * 2, m_bIsLoop);
		break;
	default:
		break;
	}
}

HRESULT CTreasureBox::Ready_Components(void * pArg)
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

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_TreasureBox"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;


	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.0f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasureBox::SetUp_ShaderResources()
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

CTreasureBox * CTreasureBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTreasureBox*	pInstance = new CTreasureBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTreasureBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTreasureBox::Clone(void * pArg)
{
	CTreasureBox*	pInstance = new CTreasureBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTreasureBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreasureBox::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
}
