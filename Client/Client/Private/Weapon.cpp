#include "stdafx.h"
#include "..\Public\Weapon.h"
#include "Player.h"
#include "GameInstance.h"
#include "PlayerBullet.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CBaseObj(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(WEAPONDESC));

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, 2.f);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, 2.f);
	m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, 2.f);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.0f));

	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_PBULLET, this);
	m_bSocket = true;
	return S_OK;
}

int CWeapon::Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = m_WeaponDesc.pSocket->Get_OffsetMatrix() * m_WeaponDesc.pSocket->Get_CombinedTransformationMatrix() * 
		XMLoadFloat4x4(&m_WeaponDesc.SocketPivotMatrix) * XMLoadFloat4x4(m_WeaponDesc.pParentWorldMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);


	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);
	m_pOBBCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	return S_OK;
}

void CWeapon::Late_Tick(_float fTimeDelta)
{

#ifdef _DEBUG
	if (m_pOBBCom != nullptr)
		m_pRendererCom->Add_Debug(m_pOBBCom);
#endif
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	
	//if (pPlayer->Get_AnimState() == CPlayer::SLASH)
	//{
	////	if (!m_bFirst)
	//	//{
	//	//	m_BulletLook = *(_vector*)m_CombinedWorldMatrix.m[2];
	//	//	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 90.f);
	//	//	m_BulletLook = XMVector3TransformNormal(m_BulletLook, RotationMatrix);
	//	//	m_bFirst = true;
	//	//}
	//	CPlayerBullet::BULLETDESC BulletDesc;

	//	BulletDesc.eBulletType = CPlayerBullet::SWORD;
	//	BulletDesc.vInitPositon = *(_vector*)m_CombinedWorldMatrix.m[3] + XMVector3Normalize(m_BulletLook) + XMVectorSet(0.f, m_Ypos, 0.f,0.f);
	////	m_Ypos += 0.001f;
	////	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -0.25f);
	////	m_BulletLook = XMVector3TransformNormal(m_BulletLook, RotationMatrix);
	//	BulletDesc.vLook = *(_vector*)m_CombinedWorldMatrix.m[2];
	////	BulletDesc.vLook = XMVectorSetY(BulletDesc.vLook, 0.f);

	//	BulletDesc.fDeadTime = 0.15f;
	//	CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_PlayerBullet"), LEVEL_STATIC, TEXT("Layer_PlayerBullet"), &BulletDesc);
	//	//m_fEffectTime = 0.f;
	//}
	//else
	//{
	//	m_bFirst = false;
	//	m_Ypos = 0.f;
	//}
	//	


	Compute_CamDistance(Get_TransformState(CTransform::STATE_POSITION));
}

HRESULT CWeapon::Render()
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

		if (m_WeaponDesc.eType == SLASH)
			m_eShaderID = SHADER_SLASH;
		else
			m_eShaderID = SHADER_DEFAULT;
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_eShaderID)))
			return E_FAIL;
	}


	
	return S_OK;
}

HRESULT CWeapon::Ready_Components(void* pArg)
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

	switch (m_WeaponDesc.eType)
	{
	case SLASH:
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Model"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	default:
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	}
	

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	switch (m_WeaponDesc.eType)
	{
	case ARROW:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_ArrowParts"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_OBB*/
		ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vPosition = _float3(0.0f, 0.2f, 0.2f);
		if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
			return E_FAIL;
		break;
	case DOGFOOD:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_DogFoodParts"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_OBB*/
		ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vPosition = _float3(0.0f, 0.2f, 0.2f);
		if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
			return E_FAIL;
		break;
	case HEART_CONTAINER:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_HeartContainerParts"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_OBB*/
		ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
		ColliderDesc.vPosition = _float3(0.0f, 0.2f, 0.2f);
		if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
			return E_FAIL;
		break;
	case MAGIC_ROD:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_MagicRodParts"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_OBB*/
		ColliderDesc.vScale = _float3(0.7f, 0.2f, 0.2f);
		ColliderDesc.vPosition = _float3(0.0f, 0.2f, 0.2f);
		if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
			return E_FAIL;
		break;
	case BOW:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Bow"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_OBB*/
		ColliderDesc.vScale = _float3(0.9f, 0.2f, 0.2f);
		ColliderDesc.vPosition = _float3(0.0f, 0.2f, 0.2f);
		if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
			return E_FAIL;
		break;
	case TELEPHONE:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_TelephoneParts"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_OBB*/
		ColliderDesc.vScale = _float3(0.9f, 0.2f, 0.2f);
		ColliderDesc.vPosition = _float3(0.0f, 0.2f, 0.2f);
		if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
			return E_FAIL;
		break;
	case SLASH:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_SwordSlash"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_OBB*/
		ColliderDesc.vScale = _float3(0.9f, 0.2f, 0.2f);
		ColliderDesc.vPosition = _float3(0.0f, 0.2f, 0.2f);
		if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
			return E_FAIL;
		break;
	default:
		break;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	


	
	///* For.Com_SHPERE */
	//ColliderDesc.vScale = _float3(0.7f, 0.2f, 0.2f);
	//ColliderDesc.vPosition = _float3(0.0f, 0.2f, 0.2f);
	//if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));	

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWeapon * CWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeapon*	pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CWeapon::Clone(void * pArg)
{
	CWeapon*	pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_PBULLET, this);
	Safe_Release(m_WeaponDesc.pSocket);

	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pSPHERECom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
