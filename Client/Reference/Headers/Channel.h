#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(class CModel* pModel, aiNodeAnim* pAIChannel);
	void Invalidate_TransformationMatrix(_float fCurrentTime);
	void Reset();

private:
	char					m_szName[MAX_PATH] = "";
	class CHierarchyNode*	m_pBoneNode = nullptr;

private:
	_uint					m_iNumKeyframes = 0;
	vector<KEYFRAME>		m_KeyFrames;

	_uint					m_iCurrentKeyFrameIndex = 0;
	_float3					m_vScale;
	_float4					m_vRotation;
	_float4					m_vPosition;

public:
	static CChannel* Create(class CModel* pModel, aiNodeAnim* pAIChannel);
	virtual void Free() override;
};

END