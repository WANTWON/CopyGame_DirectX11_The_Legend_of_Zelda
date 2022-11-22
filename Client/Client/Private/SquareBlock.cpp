#include "stdafx.h"
#include "..\Public\SquareBlock.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI_Manager.h"
#include "UIButton.h"

CSquareBlock::CSquareBlock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CNonAnim(pDevice, pContext)
{
}

CSquareBlock::CSquareBlock(const CSquareBlock & rhs)
	: CNonAnim(rhs)
{
}

HRESULT CSquareBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSquareBlock::Initialize(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_BlockDesc, pArg, sizeof(BLOCKDESC));

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	m_eObjectID = OBJ_KEY;

	if (pArg != nullptr)
	{
		_vector vecPostion = XMLoadFloat3(&m_BlockDesc.vInitPosition);
		vecPostion = XMVectorSetW(vecPostion, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPostion);
	}

	Set_Scale(_float3(3.f, 3.f, 3.f));
	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_BLOCK, this);

	return S_OK;
}

int CSquareBlock::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		m_fAlpha -= 0.05f;

		if (m_fAlpha <= 0)
		{
			CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_BLOCK, this);
			return OBJ_DEAD;
		}
	}

	return OBJ_NOEVENT;
}

void CSquareBlock::Late_Tick(_float fTimeDelta)
{
	
	switch (m_BlockDesc.eType)
	{
	case SQUARE_BLOCK:
		Tick_SquareBlock( fTimeDelta);
		break;
	case LOCK_BLOCK:
		Tick_LockBlock( fTimeDelta);
		break;
	}

}

HRESULT CSquareBlock::Render()
{
	__super::Render();

	return S_OK;

}

HRESULT CSquareBlock::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 4.0f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	switch (m_BlockDesc.eType)
	{
	case SQUARE_BLOCK:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_SquareBlock"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case LOCK_BLOCK:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_LockBlock"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_AABB*/
	ColliderDesc.vScale = _float3(0.5f, 0.2f, 0.5f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_AABB"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SHPERE */
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CSquareBlock::Tick_SquareBlock(_float fTimeDelta)
{
	if (CGameInstance::Get_Instance()->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) == false)
		return;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	CBaseObj* pTarget = nullptr;
	if (CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_PLAYER, m_pSPHERECom, &pTarget))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);

		CPlayer::ANIM ePlayerState = pPlayer->Get_AnimState();

		if (CGameInstance::Get_Instance()->Key_Pressing(DIK_A))
		{
	
			if (ePlayerState != CPlayer::PUSH_WAIT && ePlayerState != CPlayer::PULL_LP && ePlayerState != CPlayer::PUSH_LP)
			{
				pPlayer->Set_AnimState(CPlayer::PUSH_WAIT);

			}

		}
		else
		{
			if (ePlayerState == CPlayer::PUSH_WAIT && ePlayerState != CPlayer::IDLE)
				pPlayer->Set_AnimState(CPlayer::IDLE);
		}


		if (ePlayerState == CPlayer::PUSH_LP)
		{
			if (CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_PLAYER, m_pAABBCom))
			{
				_vector vDirection = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pPlayer->Get_TransformState(CTransform::STATE_POSITION);
				if (fabs(XMVectorGetX(vDirection)) > fabs(XMVectorGetZ(vDirection)))
					vDirection = XMVectorSet(XMVectorGetX(vDirection), 0.f, 0.f, 0.f);
				else
					vDirection = XMVectorSet(0.f, 0.f, XMVectorGetZ(vDirection), 0.f);
				m_pTransformCom->Go_PosDir(fTimeDelta, vDirection);
			}

		}
		else if (ePlayerState == CPlayer::PULL_LP)
		{
			_vector vDirection = pPlayer->Get_TransformState(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (fabs(XMVectorGetX(vDirection)) > fabs(XMVectorGetZ(vDirection)))
				vDirection = XMVectorSet(XMVectorGetX(vDirection), 0.f, 0.f, 0.f);
			else
				vDirection = XMVectorSet(0.f, 0.f, XMVectorGetZ(vDirection), 0.f);
			m_pTransformCom->Go_PosDir(fTimeDelta*0.2f, vDirection);
		}

	}

}

void CSquareBlock::Tick_LockBlock(_float fTimeDelta)
{
	if (CGameInstance::Get_Instance()->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) == false)
		return;

	if (nullptr != m_pRendererCom)
	{
#ifdef _DEBUG
		if (m_pAABBCom != nullptr)
			m_pRendererCom->Add_Debug(m_pAABBCom);
		if (m_pOBBCom != nullptr)
			m_pRendererCom->Add_Debug(m_pOBBCom);
		if (m_pSPHERECom != nullptr)
			m_pRendererCom->Add_Debug(m_pSPHERECom);
#endif

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
	

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	CBaseObj* pTarget = nullptr;
	if (CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_PLAYER, m_pSPHERECom, &pTarget))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);

		CUIButton*		pButton = dynamic_cast<CUIButton*>(CUI_Manager::Get_Instance()->Get_Button());
		if(m_bDead == false)
			pButton->Set_Visible(true);
		_float2 fPosition = pPlayer->Get_ProjPosition();
		fPosition.y = g_iWinSizeY - fPosition.y;
		fPosition.x += 50.f;
		fPosition.y -= 30.f;
		pButton->Set_Position(fPosition);

		CPlayer::ANIM ePlayerState = pPlayer->Get_AnimState();

		if (CGameInstance::Get_Instance()->Key_Pressing(DIK_A))
		{
			if (CUI_Manager::Get_Instance()->Get_KeySize() != 0)
			{
				pButton->Set_Visible(false);
				dynamic_cast<CPlayer*>(pPlayer)->Set_AnimState(CPlayer::KEY_OPEN);
				m_bDead = true;
			}

		}
	}
}


CSquareBlock * CSquareBlock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSquareBlock*	pInstance = new CSquareBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSquareBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSquareBlock::Clone(void * pArg)
{
	CSquareBlock*	pInstance = new CSquareBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSquareBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSquareBlock::Free()
{
	__super::Free();

}
