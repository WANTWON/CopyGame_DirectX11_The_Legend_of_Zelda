#pragma once

/* 월드스페이스에서의 상태를 보관한다.(월드행렬) */
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORMDESC;
private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) const {		
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_matrix Get_WorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, Get_WorldMatrix());
	}

	_float4x4 Get_WorldFloat4x4_TP() {
		_float4x4	WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return WorldMatrix;
	}

	_float3 Get_Scale(); 

	void Set_State(STATE eState, _fvector vState);

	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc) {
		m_TransformDesc = TransformDesc;
	}

	void Set_Scaled(_fvector vScale);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void LookAt(_fvector vPoint);

private:
	_float4x4			m_WorldMatrix;
	TRANSFORMDESC		m_TransformDesc;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END