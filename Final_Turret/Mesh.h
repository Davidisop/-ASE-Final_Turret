#pragma once
#include "Object_Handling.h"
#include "ASE.h"




class Mesh : public Object_Handling
{
public:
	AseMesh ASE;
	vector<Mesh> m_ObjectList;
	vector<Mesh> m_Object_SubObjectList;
	vector<SimpleVertex>  m_tmpVertexList; // �ϴ� ���⿡ �� �����ſ���. �ӽôϱ�, �ε��� ����, ��ġ�� �͵鵵 �� �״��!
	int   m_iNumFaces;

public:
	vector<int> available_Sub_Number_Extract_Save;

public:
	bool GEO_0_HELPER_1_Classification;

public: // �ִϸ��̼� Frame()�� ����

	float		m_fTickSpeed;
	Scene		m_Scene; // �� ���� ������ ����
	float		m_fElapseTime;
	int			First_Frame_for_Calculration;// ���� ������
	int			Last_Frame_for_Calculration; // ������ ������
	int			Frames_per_second_for_Calculration;// 1�ʴ� ������ ����(30)
	int			Tick_Per_Frame_for_Calculration;// 1�������� ƽ ��(160)

public:

	D3DXMATRIX    m_matWorld; // ���� ���
	D3DXMATRIX    m_matWorldRotate; // ���� ȸ�� ���
	D3DXMATRIX    m_matWorldScale; // ���� ���� ���
	D3DXMATRIX    m_matWorldTrans; // ���� �̵� ���
	D3DXMATRIX	  m_matCalculation; // �ڽ��� ���� �ִϸ��̼� ��� ���


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

bool  Mesh::Load_ASEfile_Trans_VertexLists_to_all_meshes_SceneInfo_Insert() // �̰����� ���������Ʈ��� �� �ɰ�����?
{
	ASE.Load(_T("Turret_Deploy.ASE")); //���⼭ �̹� ���͸��� ����Ʈ, ������͸��� ����Ʈ ���� �Ϸ�.
	
	//this->m_Object_SubObjectList.resize(ASE.Material_List[0].Sub_Material_List.size());


	for (int iObj = 0; iObj < ASE.OBJECT_GEO_HELPER_in_Object_List.size(); iObj++) // ������Ʈ�� ���� �� �繰 ���� Circulation �մϴ�. ���� �� 2�� �Դϴ�. �׷���, ���갡 ������?
	{
		Mesh     yang_mesh;// ���⼭ �����ؼ�, �̰ſ��ٰ�, ASE��� �� ����ְ�, �� ���� �Ӻ��� �Ұǰ� ����.
		int iMtrl = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Material_Reference;

			if (ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Material_Reference != 1004) // ���۴� 1004��. �� ���� 8�� �ȿ� �� ������,
			{
				// subMaterial �� ������,
				int iSubMtrl = ASE.Material_List[iMtrl].Sub_Material_List.size();
				yang_mesh.m_Object_SubObjectList.resize(iSubMtrl); //  �׷��ϱ�, ������ ASE ����ü��� ���� ���ְ� �ִ�.
				yang_mesh.m_iNumFaces = ASE.OBJECT_GEO_HELPER_in_Object_List[iObj].Position_Face_List.size(); // ���� ���̽���
																										  //
																										  //CreateTriangleList(iObj, yang_mesh);

				for (int iFace = 0; iFace < yang_mesh.m_iNumFaces; iFace++)
				{
					for (int iVer = 0; iVer < 3; iVer++)
					{
						SimpleVertex yang_vector; // ���� �����ؼ�, �̰� ä����. �̰� �� ��� �����ϰ���?

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

						//if (mesh.m_SubList.size() > 0) �̴�.
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
						yang_mesh.m_tmpVertexList[iID] = David_vector;//�̰� ��� ���̴��� �ñ��ϴ�.
					}
				}


				// ������� ������Ʈ�� �� �о���.
				// �������� �����Ѵ�.
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
	// ���ʹϿ��� ���⼭ �����ߴ�.
	D3DXMatrixIdentity(&David_matCalculation);

	D3DXMATRIX matAnim; // �ٽ� ����� ���⿡�� �����ߴ�. �и��� �̰��� �ٽ����� ��Ȱ�� �Ѵ�.
	D3DXMATRIX matPos, matRotate, matScale; // ���⼭ ��ĵ��� �����ߴ�. ���ǰ� ����.

	matRotate = David_matWorldRotate;
	matPos = David_matWorldTrans;
	matScale = David_matWorldScale;

	D3DXQuaternionRotationMatrix(&qR, &matRotate);
	//Builds a quaternion from a rotation matrix.
	D3DXQuaternionRotationMatrix(&qS, &matScale); // �̰� ��Ư�ϳ�. 	�� Scale ���� �׷��� ����? �������� Scale�� ȸ���� ���� �׷���?


												  // David_FrameTick = m_Scene.iFirstFrame * m_Scene.iTickPerFrame + CurFame;
	float fStart_Tick = David_Scene.First_Frame * David_Scene.Tick_Per_Frame; // �� ���� �� ƽ���� �����ϴ���.
	float fEnd_Tick = 0.0f; // �̰��� ��������?

	TAnimTrack* pStartTrack; // �ٽ����� ��Ȱ�� �� �ž�.
	TAnimTrack* pEndTrack;


	// Roation Ʈ�� �ȿ� ��� ������� �ᱹ ��� "matRotate" �� ������Ʈ.

	pStartTrack = NULL;
	pEndTrack = NULL;

	if (David_pRotTrack.size()) // = David_pRotTrack�� ������
	{
		// pStartTrack�� ã���� ������

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
		}              //���纸�������ʹϿ�//��������//��������//(����ð� - ó�� Key �ð�) / (���� Key �ð� - ���� Key �ð�)
		D3DXMatrixRotationQuaternion(&matRotate, &qR); 
	}



	// Trans Ʈ�� �ȿ� ��� ������� �ᱹ ��� 'matPos' �� ������Ʈ.

	pStartTrack = NULL;
	pEndTrack = NULL; // �� ��͵��� ����ְ� �־�!  ���

	D3DXVECTOR3 Trans(matPos._41, matPos._42, matPos._43);// ������.

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
			// Ʈ���ȿ�, ���� �κ����� ������ �� ���� ã������, �װɷ� "matPos"�� �ٲ��ش�.
			Trans = pStartTrack->Vector_in_Animation;
			fStart_Tick = pStartTrack->iTick;
		}
		if (pEndTrack)// 
		{// pEndTrack�� ã���� ������

			fEnd_Tick = pEndTrack->iTick;
			D3DXVec3Lerp(&Trans, &Trans, &pEndTrack->Vector_in_Animation, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
		}                //���纸���麤��//��������//���ߺ���                        //  (����ð� - ó�� Key �ð�) / (���� Key �ð� - ���� Key �ð�)

		D3DXMatrixTranslation(&matPos, Trans.x, Trans.y, Trans.z); // �̷��� �ٲ�����.
	}


	// Scale Ʈ�� �ȿ� ��� ������� �ᱹ ��� 'matScale' �� ������Ʈ.

	pStartTrack = NULL;
	pEndTrack = NULL;

	D3DXMATRIX matScaleRot, matInvScaleRot;
	D3DXVECTOR3 vScale(matScale._11, matScale._22, matScale._33); // ������ ������ �Է�. ��Ȯ�� ������ ����� 11 = x�༺��, 22= y�༺��, 33=z�� ���� �̴�.

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


		if (pStartTrack)//NODE_TM ������ �ֽ��ΰɷ�. �ٽ��̳�.
		{ // pStartTrack�� ã���� ������  ������ ������ ������Ʈ�Ѵ�.
			vScale = pStartTrack->Vector_in_Animation;
			
			// ���ʹϿ�.
			qS = pStartTrack->qRotate; //D3DXQUATERNION  qRotate;
			fStart_Tick = pStartTrack->iTick;
		}

		if (pEndTrack)// pEndTrack�� ã���� ������
		{
			fEnd_Tick = pEndTrack->iTick;

			D3DXVec3Lerp(&vScale, &vScale, &pEndTrack->Vector_in_Animation, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
			//���纸���麤��//��������//���ߺ���                        //  (����ð� - ó�� Key �ð�) / (���� Key �ð� - ���� Key �ð�)

			D3DXQuaternionSlerp(&qS, &qS, &pEndTrack->qRotate, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
			//���纸��������//��������//��������        //  (����ð� - ó�� Key �ð�) / (���� Key �ð� - ���� Key �ð�)

			//Interpolates between two quaternions, using spherical linear interpolation.

			/*	D3DXQUATERNION* D3DXQuaternionSlerp(
			_Inout_       D3DXQUATERNION *pOut,
			_In_    const D3DXQUATERNION *pQ1,
			_In_    const D3DXQUATERNION *pQ2,
			_In_          FLOAT          t

			Parameter that indicates how far to interpolate between the quaternions.
			*/

		}
		D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z); // ���� ���� �� ���ͷ� matScale�� ������Ʈ.
		D3DXMatrixRotationQuaternion(&matScaleRot, &qS); // ���� ���� �� ���ʹϿ��� ��� matScaleRot���� ��ȯ 
		D3DXMatrixInverse(&matInvScaleRot, NULL, &matScaleRot); // �̰� �����ȭ��.
		matScale = matInvScaleRot * matScale * matScaleRot; // �̰� ������ �� Sclae�� �⺻��. ����� �ѹ��ϰ�, ������¿��� ������ �ϰ�, �ѹ��� ������ �� �������Ѵ�.
	}

	// Visible Ʈ�� �ȿ� ��� ������� �ᱹ "David_fVisibility"�� ������Ʈ.

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
		{// pStartTrack�� ã���� ������
			fCurrent_Alpha = pStartTrack->Vector_in_Animation.x;
			fStart_Tick = pStartTrack->iTick;
		}

		if (pEndTrack)
			// pEndTrack�� ã���� ������
		{
			fNext_Alpha = pEndTrack->Vector_in_Animation.x;
			fEnd_Tick = pEndTrack->iTick;

			fOffSet = (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick);
			//���� ������ s ��.
			fNext_Alpha = (fNext_Alpha - fCurrent_Alpha)*fOffSet;
			// ��ü ���� ������.
		}
		David_fVisibility = (fCurrent_Alpha + fNext_Alpha); // ������ ���� �Ϳ� ���� ���� ���� ���� ����.
	}
	else
	{
		David_fVisibility = 1.0f;
	}
	//

	// ���� ���


	D3DXMatrixMultiply(&matAnim, &matScale, &matRotate);

	matAnim._41 = matPos._41;
	matAnim._42 = matPos._42;
	matAnim._43 = matPos._43;      //S,R,T

								   // ���� ���̸��̼� ����� �ϼ��Ѵ�.	
	D3DXMatrixMultiply(&David_matCalculation, &matAnim, matParents); // �����߿��ϴ�. �ڽ� * �θ� : �� ���� ���� ���� �س���, �¾� �����ؾ��Ѵ�.

														 // �ι��� ��Ʈ���� Ȯ�� �ڵ�.
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




