#pragma once
#include "Object_Handling.h"
#include "ASE.h"




class Mesh : public Object_Handling
{
public:
	AseMesh ASE;
	vector<Mesh> m_ObjectList;
	vector<Mesh> m_Object_SubObjectList;
	vector<SimpleVertex>  m_tmpVertexList; // 일단 여기에 다 넣을거에요. 임시니까, 인덱스 없이, 겹치는 것들도 다 그대로!
	int   m_iNumFaces;

public:
	vector<int> available_Sub_Number_Extract_Save;

public:
	bool GEO_0_HELPER_1_Classification;

public: // 애니메이션 Frame()을 위해

	float		m_fTickSpeed;
	Scene		m_Scene; // 씬 정보 저장을 위한
	float		m_fElapseTime;
	int			First_Frame_for_Calculration;// 시작 프레임
	int			Last_Frame_for_Calculration; // 마지막 프레임
	int			Frames_per_second_for_Calculration;// 1초당 프레임 개수(30)
	int			Tick_Per_Frame_for_Calculration;// 1프레임의 틱 값(160)

public:

	D3DXMATRIX    m_matWorld; // 월드 행렬
	D3DXMATRIX    m_matWorldRotate; // 월드 회전 행렬
	D3DXMATRIX    m_matWorldScale; // 월드 신축 행렬
	D3DXMATRIX    m_matWorldTrans; // 월드 이동 행렬
	D3DXMATRIX	  m_matCalculation; // 자식의 최종 애니메이션 계산 행렬


public:

	bool	 Load_ASEfile_Trans_VertexLists_to_all_meshes_SceneInfo_Insert();
	int		 IsEqulVertexList(vector<SimpleVertex>& list, SimpleVertex& v);
	float	 SetAnimationSpeed(float fSpeed);
	void	 Interpolate(Mesh* pMesh, D3DXMATRIX* matParents,
		float David_FrameTick,
		D3DXMATRIX& David_matCalculation,
		D3DXMATRIX& David_matWorldRotate, D3DXMATRIX& David_matWorldTrans, D3DXMATRIX& David_matWorldScale,
		vector<TAnimTrack>	David_pRotTrack, vector<TAnimTrack>	David_pPosTrack, vector<TAnimTrack>	David_pSclTrack, vector<TAnimTrack>	David_pVisTrack,
		Scene David_Scene,
		float& David_fVisibility);

public:
	bool   Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile);
	bool   update_ConstantBuffer();

public:
	Mesh()
	{
		GEO_0_HELPER_1_Classification = 1;
		m_iNumFaces = 0;
		m_fTickSpeed = 0;
		m_fElapseTime = 0;
		First_Frame_for_Calculration = 0;
		Last_Frame_for_Calculration = 0;
		Frames_per_second_for_Calculration = 0;
		Tick_Per_Frame_for_Calculration = 0;
		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixIdentity(&m_matWorldTrans);
		D3DXMatrixIdentity(&m_matWorldRotate);
		D3DXMatrixIdentity(&m_matWorldScale);
		D3DXMatrixIdentity(&m_matCalculation);
	}
	virtual ~Mesh() {}
};

bool  Mesh::Load_ASEfile_Trans_VertexLists_to_all_meshes_SceneInfo_Insert() // 이곳에서 서브오브젝트들로 다 쪼개갰지?
{
	ASE.Load(_T("Turret_Deploy.ASE")); //여기서 이미 메터리얼 리스트, 서브메터리얼 리스트 제작 완료.
	
	//this->m_Object_SubObjectList.resize(ASE.Material_List[0].Sub_Material_List.size());


	for (int iObj = 0; iObj < ASE.OBJECT_GEO_HELPER_in_Object_List.size(); iObj++) // 지오메트리 마다 즉 사물 마다 Circulation 합니다. 지금 총 2개 입니다. 그러나, 서브가 있지요?
	{
		Mesh     yang_mesh;// 여기서 정의해서, 이거에다가, ASE대로 다 집어넣고, 그 다음 뿐빠이 할건가 보다.
		int iMtrl = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Material_Reference;

			if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Material_Reference != 1004) // 헬퍼는 1004다. 즉 지오 8개 안에 들어가 있으면,
			{
				// subMaterial 이 있으면,
				int iSubMtrl = ASE.Material_List[iMtrl].Sub_Material_List.size();
				yang_mesh.m_Object_SubObjectList.resize(iSubMtrl); //  그러니까, 기존의 ASE 구조체대로 지금 해주고 있다.
				yang_mesh.m_iNumFaces = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Position_Face_List.size(); // 기준 페이스수
																										  //
																										  //CreateTriangleList(iObj, yang_mesh);

				for (int iFace = 0; iFace < yang_mesh.m_iNumFaces; iFace++)
				{
					for (int iVer = 0; iVer < 3; iVer++)
					{
						SimpleVertex yang_vector; // 새로 정의해서, 이걸 채워서. 이걸 또 어디에 저장하겠지?

						int iID = iFace * 3 + iVer;
						// pos
						if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].vertexList.size() > 0)
						{
							int iIndex = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Position_Face_List[iFace].ASE_Index[iVer];
							yang_vector.p = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].vertexList[iIndex];
						}
						// normal
						if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].normal_List.size() > 0)
						{
							yang_vector.n =
								ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].normal_List[iID];
						}
						// color
						yang_vector.c = D3DXVECTOR4(1, 1, 1, 1);
						if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_Face_List.size() > 0)
						{
							int iIndex = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_Face_List[iFace].ASE_Index[iVer];
							yang_vector.c.x = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_List[iIndex].x;
							yang_vector.c.y = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_List[iIndex].y;
							yang_vector.c.z = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_List[iIndex].z;
							yang_vector.c.w = 1.0f;
						}
						// tex
						if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Texture_Face_List.size() > 0)
						{
							int iIndex = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Texture_Face_List[iFace].ASE_Index[iVer];
							yang_vector.t.x = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Texture_List[iIndex].x;
							yang_vector.t.y = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Texture_List[iIndex].y;
						}

						//if (mesh.m_SubList.size() > 0) 이니.
						iID = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Position_Face_List[iFace].Sub_Material_number;
						yang_mesh.GEO_0_HELPER_1_Classification = 0;
						yang_mesh.m_Object_SubObjectList[iID].GEO_0_HELPER_1_Classification = 0;
						yang_mesh.m_Object_SubObjectList[iID].m_VertexList.push_back(yang_vector);

					}
				}
				this->m_ObjectList.push_back(yang_mesh);
			}

			else 
			{

				yang_mesh.m_iNumFaces = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Position_Face_List.size();
				yang_mesh.m_tmpVertexList.resize(yang_mesh.m_iNumFaces * 3);

				//CreateTriangleList(iObj, mesh, mesh.m_tmpVertexList);

				for (int iFace = 0; iFace < yang_mesh.m_iNumFaces; iFace++)
				{
					for (int iVer = 0; iVer < 3; iVer++)
					{
						SimpleVertex David_vector;
						int iID = iFace * 3 + iVer;
						// pos
						if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].vertexList.size() > 0)
						{
							int iIndex = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Position_Face_List[iFace].ASE_Index[iVer];

							David_vector.p = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].vertexList[iIndex];
						}
						// normal
						if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].normal_List.size() > 0)
						{
							David_vector.n = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].normal_List[iID];
						}
						// color
						David_vector.c = D3DXVECTOR4(1, 1, 1, 1);
						if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_Face_List.size() > 0)
						{
							int iIndex = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_Face_List[iFace].ASE_Index[iVer];
							David_vector.c.x = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_List[iIndex].x;
							David_vector.c.y = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_List[iIndex].y;
							David_vector.c.z = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Color_List[iIndex].z;
							David_vector.c.w = 1.0f;
						}
						// tex
						if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Texture_Face_List.size() > 0)
						{
							int iIndex = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Texture_Face_List[iFace].ASE_Index[iVer];
							David_vector.t.x = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Texture_List[iIndex].x;
							David_vector.t.y = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Texture_List[iIndex].y;
						}
						yang_mesh.m_tmpVertexList[iID] = David_vector;//이게 어디에 쓰이는지 궁금하다.
					}
				}


				// 여기까지 지오메트리 다 읽었다.
				// 이제부터 진행한다.
				// create ib + vb
				// xMesh.m_tmpVertexList

				for (int ivb = 0; ivb < yang_mesh.m_tmpVertexList.size(); ivb++)
				{
					SimpleVertex isop_vector = yang_mesh.m_tmpVertexList[ivb];
					int David_iPos = IsEqulVertexList(yang_mesh.m_VertexList, isop_vector);
					if (David_iPos < 0)
					{
						yang_mesh.m_VertexList.push_back(isop_vector);
						David_iPos = yang_mesh.m_VertexList.size() - 1;
					}
					yang_mesh.m_IndexList.push_back(David_iPos);
				}

				yang_mesh.GEO_0_HELPER_1_Classification = 1;
				yang_mesh.m_icount_Indexes = yang_mesh.m_IndexList.size();
				yang_mesh.m_icount_Vertexs = yang_mesh.m_VertexList.size();

				/*	int iRef = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Material_Reference;
				T_STR texName = ASE.Material_List[iRef].TextureMap_List[0].name;
				yang_mesh.Create(g_pd3dDevice, L"../../data/shader/map.hlsl", texName);
				*/
				this->m_ObjectList.push_back(yang_mesh);

			}


	}


	for (int i=0; i < this->m_ObjectList.size(); i++)
	{
		if(this->m_ObjectList[i].m_Object_SubObjectList.size()==0)
		{
			continue;
		}
		else if(this->m_ObjectList[i].m_Object_SubObjectList.size()!=0)
		{
			for (int j=0; j < 14; j++)
			{
				if (this->m_ObjectList[i].m_Object_SubObjectList[j].m_VertexList.size()!= 0)
				{			
					this->m_ObjectList[i].available_Sub_Number_Extract_Save.push_back(j);
				}
			}
		}
	}



	m_Scene.First_Frame = ASE.m_Scene.First_Frame;
	m_Scene.Last_Frame = ASE.m_Scene.Last_Frame;
	m_Scene.Frames_per_second = ASE.m_Scene.Frames_per_second;
	m_Scene.Tick_Per_Frame = ASE.m_Scene.Tick_Per_Frame;





	return true;
}


int   Mesh::IsEqulVertexList(vector<SimpleVertex>& list, SimpleVertex& v)
{
	for (int ilist = 0; ilist < list.size(); ilist++)
	{
		if (v == list[ilist])
		{
			return ilist;
		}
	}
	return -1;
}
float Mesh::SetAnimationSpeed(float fSpeed)
{
	m_fTickSpeed = fSpeed;

	return m_fTickSpeed;
}

void  Mesh::Interpolate(Mesh* pMesh, D3DXMATRIX* matParents,
			float David_FrameTick,
			D3DXMATRIX& David_matCalculation,
			D3DXMATRIX& David_matWorldRotate, D3DXMATRIX& David_matWorldTrans, D3DXMATRIX& David_matWorldScale,
			vector<TAnimTrack>	David_pRotTrack, vector<TAnimTrack>	David_pPosTrack, vector<TAnimTrack>	David_pSclTrack, vector<TAnimTrack>	David_pVisTrack,
			Scene David_Scene,
			float& David_fVisibility)
{
	// TM		= AnimMat * ParentTM;
	// AaniMat	= TM * Inverse(ParentTM)
	D3DXQUATERNION qR;
	D3DXQUATERNION qS;
	// 쿼터니온을 여기서 정의했다.
	D3DXMatrixIdentity(&David_matCalculation);

	D3DXMATRIX matAnim; // 핵심 행렬을 여기에서 정의했다. 분명히 이것이 핵심적인 역활을 한다.
	D3DXMATRIX matPos, matRotate, matScale; // 여기서 행렬들을 정의했다. 쓸건가 보다.

	matRotate = David_matWorldRotate;
	matPos = David_matWorldTrans;
	matScale = David_matWorldScale;

	D3DXQuaternionRotationMatrix(&qR, &matRotate);
	//Builds a quaternion from a rotation matrix.
	D3DXQuaternionRotationMatrix(&qS, &matScale); // 이거 독특하네. 	왜 Scale 까지 그렇게 했지? 임의의축 Scale은 회전이 들어가서 그런가?


												  // David_FrameTick = m_Scene.iFirstFrame * m_Scene.iTickPerFrame + CurFame;
	float fStart_Tick = David_Scene.First_Frame * David_Scene.Tick_Per_Frame; // 이 씬이 몇 틱부터 시작하느냐.
	float fEnd_Tick = 0.0f; // 이것이 나오겠지?

	TAnimTrack* pStartTrack; // 핵심적인 역활을 할 거야.
	TAnimTrack* pEndTrack;


	// Roation 트랙 안에 담긴 정보들로 결국 행렬 "matRotate" 을 업데이트.

	pStartTrack = NULL;
	pEndTrack = NULL;

	if (David_pRotTrack.size()) // = David_pRotTrack이 있으면
	{
		// pStartTrack를 찾을수 있으면

		for (DWORD dwTrack = 0; dwTrack < David_pRotTrack.size(); dwTrack++)
		{
			TAnimTrack *pTrack = &David_pRotTrack[dwTrack];
			_ASSERT(pTrack);

			if (pTrack->iTick > David_FrameTick)
			{			
				pEndTrack = pTrack;		
				break;
			}
			pStartTrack = pTrack;
		}
	
		if (pStartTrack)
		{	qR = pStartTrack->qRotate;
			fStart_Tick = pStartTrack->iTick;}

		if (pEndTrack)
		{	fEnd_Tick = pEndTrack->iTick;
			D3DXQuaternionSlerp(&qR, &qR, &pEndTrack->qRotate, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
		}              //현재보간中쿼터니온//기존쿼터//나중쿼터//(현재시간 - 처음 Key 시간) / (나중 Key 시간 - 현재 Key 시간)
		D3DXMatrixRotationQuaternion(&matRotate, &qR); 
	}



	// Trans 트랙 안에 담긴 정보들로 결국 행렬 'matPos' 을 업데이트.

	pStartTrack = NULL;
	pEndTrack = NULL; // 이 빈것들을 집어넣고 있어!  대박

	D3DXVECTOR3 Trans(matPos._41, matPos._42, matPos._43);// 벡터임.

	if (David_pPosTrack.size())
	{
	
		
		for (DWORD dwTrack = 0; dwTrack < David_pPosTrack.size(); dwTrack++)
		{
			TAnimTrack *pTrack = &David_pPosTrack[dwTrack];
			_ASSERT(pTrack);

			if (pTrack->iTick > David_FrameTick)
			{
				pEndTrack = pTrack;
				break;
			}
			pStartTrack = pTrack;
		}


		if (pStartTrack)
		{
			// 트랙안에, 시작 부분으로 쓸만한 前 것을 찾았으면, 그걸로 "matPos"를 바꿔준다.
			Trans = pStartTrack->Vector_in_Animation;
			fStart_Tick = pStartTrack->iTick;
		}
		if (pEndTrack)// 
		{// pEndTrack를 찾을수 있으면

			fEnd_Tick = pEndTrack->iTick;
			D3DXVec3Lerp(&Trans, &Trans, &pEndTrack->Vector_in_Animation, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
		}                //현재보간中벡터//기존벡터//나중벡터                        //  (현재시간 - 처음 Key 시간) / (나중 Key 시간 - 현재 Key 시간)

		D3DXMatrixTranslation(&matPos, Trans.x, Trans.y, Trans.z); // 이렇게 바뀌어야지.
	}


	// Scale 트랙 안에 담긴 정보들로 결국 행렬 'matScale' 을 업데이트.

	pStartTrack = NULL;
	pEndTrack = NULL;

	D3DXMATRIX matScaleRot, matInvScaleRot;
	D3DXVECTOR3 vScale(matScale._11, matScale._22, matScale._33); // 기존의 정보를 입력. 정확히 스케일 행렬은 11 = x축성분, 22= y축성분, 33=z축 성분 이다.

	if (David_pSclTrack.size())
	{

		for (DWORD dwTrack = 0; dwTrack < David_pSclTrack.size(); dwTrack++)
		{
			TAnimTrack *pTrack = &David_pSclTrack[dwTrack];
			_ASSERT(pTrack);

			if (pTrack->iTick > David_FrameTick)
			{
				pEndTrack = pTrack;
				break;
			}
			pStartTrack = pTrack;
		}


		if (pStartTrack)//NODE_TM 값보다 최신인걸로. 핵심이네.
		{ // pStartTrack를 찾을수 있으면  기존의 정보를 업데이트한다.
			vScale = pStartTrack->Vector_in_Animation;
			
			// 쿼터니온.
			qS = pStartTrack->qRotate; //D3DXQUATERNION  qRotate;
			fStart_Tick = pStartTrack->iTick;
		}

		if (pEndTrack)// pEndTrack를 찾을수 있으면
		{
			fEnd_Tick = pEndTrack->iTick;

			D3DXVec3Lerp(&vScale, &vScale, &pEndTrack->Vector_in_Animation, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
			//현재보간中벡터//기존벡터//나중벡터                        //  (현재시간 - 처음 Key 시간) / (나중 Key 시간 - 현재 Key 시간)

			D3DXQuaternionSlerp(&qS, &qS, &pEndTrack->qRotate, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
			//현재보간中쿼터//기존쿼터//나중쿼터        //  (현재시간 - 처음 Key 시간) / (나중 Key 시간 - 현재 Key 시간)

			//Interpolates between two quaternions, using spherical linear interpolation.

			/*	D3DXQUATERNION* D3DXQuaternionSlerp(
			_Inout_       D3DXQUATERNION *pOut,
			_In_    const D3DXQUATERNION *pQ1,
			_In_    const D3DXQUATERNION *pQ2,
			_In_          FLOAT          t

			Parameter that indicates how far to interpolate between the quaternions.
			*/

		}
		D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z); // 현재 보간 中 벡터로 matScale을 업데이트.
		D3DXMatrixRotationQuaternion(&matScaleRot, &qS); // 현재 보간 中 쿼터니온을 행렬 matScaleRot으로 반환 
		D3DXMatrixInverse(&matInvScaleRot, NULL, &matScaleRot); // 이걸 역행렬화함.
		matScale = matInvScaleRot * matScale * matScaleRot; // 이게 임의의 축 Sclae의 기본임. 역행렬 한번하고, 평범상태에서 스케일 하고, 한번더 임의의 축 스케일한다.
	}

	// Visible 트랙 안에 담긴 정보들로 결국 "David_fVisibility"를 업데이트.

	pStartTrack = NULL;
	pEndTrack = NULL;

	float fCurrent_Alpha, fNext_Alpha, fOffSet;
	fCurrent_Alpha = 0.0f;
	fNext_Alpha = 0.0f;

	if (David_pVisTrack.size())
	{

		for (DWORD dwTrack = 0; dwTrack < David_pVisTrack.size(); dwTrack++)
		{
			TAnimTrack *pTrack = &David_pVisTrack[dwTrack];
			_ASSERT(pTrack);

			if (pTrack->iTick > David_FrameTick)
			{
				pEndTrack = pTrack;
				break;
			}
			pStartTrack = pTrack;
		}



		if (pStartTrack)
		{// pStartTrack를 찾을수 있으면
			fCurrent_Alpha = pStartTrack->Vector_in_Animation.x;
			fStart_Tick = pStartTrack->iTick;
		}

		if (pEndTrack)
			// pEndTrack를 찾을수 있으면
		{
			fNext_Alpha = pEndTrack->Vector_in_Animation.x;
			fEnd_Tick = pEndTrack->iTick;

			fOffSet = (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick);
			//선형 보간의 s 값.
			fNext_Alpha = (fNext_Alpha - fCurrent_Alpha)*fOffSet;
			// 자체 선형 벡터임.
		}
		David_fVisibility = (fCurrent_Alpha + fNext_Alpha); // 저절로 前의 것에 지금 현재 보간 값을 더함.
	}
	else
	{
		David_fVisibility = 1.0f;
	}
	//

	// 최종 결과


	D3DXMatrixMultiply(&matAnim, &matScale, &matRotate);

	matAnim._41 = matPos._41;
	matAnim._42 = matPos._42;
	matAnim._43 = matPos._43;      //S,R,T

								   // 최종 에미메이션 행렬을 완성한다.	
	D3DXMatrixMultiply(&David_matCalculation, &matAnim, matParents); // 순서중요하다. 자식 * 부모 : 달 자전 지구 공전 해놓고, 태양 공전해야한다.

														 // 인버스 매트릭스 확인 코드.
	D3DXVECTOR3 v0, v1, v2, v3;
	v0 = David_matCalculation.m[0];
	v1 = David_matCalculation.m[1];
	v2 = David_matCalculation.m[2];
	D3DXVec3Cross(&v3, &v1, &v2);
	if (D3DXVec3Dot(&v3, &v0) < 0.0f)
	{
		D3DXMATRIX matW;
		D3DXMatrixScaling(&matW, -1.0f, -1.0f, -1.0f);
		D3DXMatrixMultiply(&David_matCalculation,
			&David_matCalculation, &matW);
	}
	David_matWorldRotate= matRotate;
	David_matWorldTrans= matPos;
	David_matWorldScale= matScale;
}

bool Mesh::Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile)
{
	PipeLineSetup.LoadTexture(pd3dDevice, pTexFile);
	PipeLineSetup.SetBlendState(pd3dDevice);
	PipeLineSetup.CreateVertextBuffer(pd3dDevice, &m_VertexList.at(0), m_icount_Vertexs);

	if (m_icount_Indexes != 0)	PipeLineSetup.CreateIndexBuffer(pd3dDevice, &m_IndexList.at(0), m_icount_Indexes);

	PipeLineSetup.CreateConstantBuffer(pd3dDevice, &m_Constant_Data);
	PipeLineSetup.CreateVertexShader(pd3dDevice, pVsFile);
	PipeLineSetup.CreateLayout(pd3dDevice);
	PipeLineSetup.CreatePixelShader(pd3dDevice, pPsFile);



	PipeLineSetup.ScreenViewPort_setting();
	PipeLineSetup.Create_RasterizerState();
	PipeLineSetup.CreateTexture2D_DepthStencilView();
	PipeLineSetup.CreateDepthStencilState();

	return true;
}
bool Mesh::update_ConstantBuffer()
{
	g_pContext->UpdateSubresource(PipeLineSetup.m_pConstantBuffer, 0, NULL, &m_Constant_Data, 0, 0);
	return true;
}




