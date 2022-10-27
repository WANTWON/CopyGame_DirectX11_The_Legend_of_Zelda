#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CBaseObj abstract : public CGameObject
{
public:
	enum DIRINPUT {DIR_X,DIR_Z, POS_END};

protected:
	CBaseObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBaseObj(const CBaseObj& rhs);
	virtual ~CBaseObj() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual _bool Picking(_float3* PickingPoint) { return false; };
	virtual void PickingTrue() {};
	void Change_Direction();

public:
	OBJID Get_ObjectID() { return m_eObjectID; }
	_float3 Get_Scale() { return m_vScale; };
	_vector Get_Position();
	void Set_State(CTransform::STATE eState, _fvector vState);
	void Set_Scale(_float3 vScale);

	
protected: /* For.Components */
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;


protected:
	virtual HRESULT Ready_Components(void* pArg) = 0;
	virtual HRESULT SetUp_ShaderID() { return S_OK; };
	virtual HRESULT SetUp_ShaderResources() = 0;

protected:
	SHADER_ID		m_eShaderID = SHADER_DEFAULT;
	OBJID			m_eObjectID = OBJ_END;
	_float3			m_vScale = _float3(1.f,1.f,1.f);
	_float3			m_vMousePickPos;
	_float				m_eDir[POS_END] = { 0 };
	_float				m_ePreDir[POS_END] = { 0 };


public:
	virtual void Free() override;
};

END