#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		float		fSpeedPerSec;
		float		fRotationPerSec;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) const { return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);}	
	_matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	_matrix Get_WorldMatrixInverse() const { return XMMatrixInverse(nullptr, Get_WorldMatrix()); }
	_float4x4 Get_World4x4() const { return m_WorldMatrix; }
	_float4x4 Get_World4x4_TP() const {
		_float4x4	TransposeMatrix;
		XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return TransposeMatrix; }
	_float Get_Scale(STATE eState) { return XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[eState])); }
	TRANSFORMDESC Get_TransformDesc() { return m_TransformDesc;  }

	void Set_State(STATE eState, _fvector vState) {
		_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		WorldMatrix.r[eState] = vState;
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);	
	}
	void Set_Scale(STATE eState, _float fScale);
	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc ) {m_TransformDesc = TransformDesc; }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);	
	void Jump(_float fTimeDelta, _float fVelocity, _float fGravity,_float iHeight);
	void Go_PosTarget(_float fTimeDelta, _vector TargetPos, _vector distance = XMVectorSet(0.f,0.f,0.f,0.f));
	void Go_PosDir(_float fTimeDelta, _vector vecDir);


public:
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Follow_Target(_float fTimeDelta, _vector TargetPos, _vector distance = XMVectorSet(0.f,0.f,0.f,0.f));

	void LookAt(_fvector vAt);
	void Change_Direction(_float UpDown, _float RightLeft);

private:			
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END