#pragma once
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	CPipeLine();
	virtual ~CPipeLine() = default;


public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_PipeLineMatrix[eState], TransformMatrix);
	}

	_matrix Get_TransformMatrix(TRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_PipeLineMatrix[eState]);
	}
	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATE eState) {
		return m_PipeLineMatrix[eState];
	}
	_float4x4 Get_TransformFloat4x4_TP(TRANSFORMSTATE eState) {
		return m_PipeLineMatrix_TP[eState];
	}

	_float4 Get_CamPosition() {
		return m_vCamPosition;
	}


public:
	void Update();

private:
	_float4x4				m_PipeLineMatrix[D3DTS_END];
	_float4x4				m_PipeLineMatrix_TP[D3DTS_END];
	_float4					m_vCamPosition;

public:
	virtual void Free() override;
};

END