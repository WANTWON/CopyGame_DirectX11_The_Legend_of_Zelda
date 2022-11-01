#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "GameInstance.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext) 
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
{
	Safe_AddRef(m_pInputLayout);
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void*			pShaderbyteCode = nullptr;
	size_t				iShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderbyteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderbyteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_ColliderDesc, pArg, sizeof(COLLIDERDESC));

	_matrix			ScaleMatrix, RotationMatrix, TranslationMatrix;

	ScaleMatrix = XMMatrixScaling(m_ColliderDesc.vScale.x, m_ColliderDesc.vScale.y, m_ColliderDesc.vScale.z);
	_matrix RotationXMatrix = XMMatrixRotationX(m_ColliderDesc.vRotation.x);
	_matrix RotationYMatrix = XMMatrixRotationY(m_ColliderDesc.vRotation.y);
	_matrix RotationZMatrix = XMMatrixRotationZ(m_ColliderDesc.vRotation.z);
	RotationMatrix = RotationXMatrix * RotationYMatrix * RotationZMatrix;

	TranslationMatrix = XMMatrixTranslation(m_ColliderDesc.vPosition.x, m_ColliderDesc.vPosition.y, m_ColliderDesc.vPosition.z);

	for (_uint i = 0; i < BOUNDING_END; ++i)
	{
		switch (m_eType)
		{
		case TYPE_AABB:
			m_pAABB[i] = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
			m_pAABB[i]->Transform(*m_pAABB[i], ScaleMatrix * TranslationMatrix);
			break;
		case TYPE_OBB:
			m_pOBB[i] = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
			m_pOBB[i]->Transform(*m_pOBB[i], ScaleMatrix * RotationMatrix * TranslationMatrix);
			break;
		case TYPE_SPHERE:
			m_pSphere[i] = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
			m_pSphere[i]->Transform(*m_pSphere[i], ScaleMatrix * RotationMatrix * TranslationMatrix);
			break;
		}
	}

	

	return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB[BOUNDING_ORIGINAL]->Transform(*m_pAABB[BOUNDING_WORLD], Remove_Rotation(WorldMatrix));
		break;
	case TYPE_OBB:
		m_pOBB[BOUNDING_ORIGINAL]->Transform(*m_pOBB[BOUNDING_WORLD], WorldMatrix);
		break;
	case TYPE_SPHERE:
		m_pSphere[BOUNDING_ORIGINAL]->Transform(*m_pSphere[BOUNDING_WORLD], WorldMatrix);
		break;
	}
}

HRESULT CCollider::Render()
{
	m_pBatch->Begin();

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->SetWorld(XMMatrixIdentity());

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pEffect->SetView(pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	
	RELEASE_INSTANCE(CGameInstance);

	m_pEffect->Apply(m_pContext);

	_vector		vColor = m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f);
	
	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB[BOUNDING_WORLD], vColor);
		break;
	case CCollider::TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB[BOUNDING_WORLD], vColor);
		break;
	case CCollider::TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere[BOUNDING_WORLD], vColor);
		break;
	}

	m_pBatch->End();
	
	return S_OK;
}

_matrix CCollider::Remove_Rotation(_fmatrix Matrix)
{
	_matrix		TransformMatrix = Matrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f ,0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[0]));
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[1]));
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[2]));

	return TransformMatrix;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider*	pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		ERR_MSG(TEXT("Failed to Created : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*	pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);

	for (_uint i = 0; i < BOUNDING_END; ++i)
	{
		Safe_Delete(m_pAABB[i]);
		Safe_Delete(m_pOBB[i]);
		Safe_Delete(m_pSphere[i]);
	}
	

}
