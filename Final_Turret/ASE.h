#pragma once
#include "xStd.h"
#define ID_TBASIS_AM 0   // ambient
#define ID_TBASIS_DI 1   // diffuse
#define ID_TBASIS_SP 2   // specular
#define ID_TBASIS_SH 3   // shininesNs
#define ID_TBASIS_SS 4   // shininess strength
#define ID_TBASIS_SI 5   // self-illumination
#define ID_TBASIS_OP 6   // opacity
#define ID_TBASIS_FI 7   // filter color
#define ID_TBASIS_BU 8   // bump 
#define ID_TBASIS_RL 9   // reflection
#define ID_TBASIS_RR 10  // refraction 
#define ID_TBASIS_DP 11  // displacement
#define ID_TBASIS_GN 12  // Generic
#define ID_TBASIS_EV 13  // EnvMap

enum AseSectionType { GEOMOBJECT = 0, HELPEROBJECT = 1 };


struct Scene
{
	int First_Frame;// ���� ������
	int Last_Frame; // ������ ������
	int Frames_per_second;// 1�ʴ� ������ ����(30)
	int Tick_Per_Frame;// 1�������� ƽ ��(160)
};

struct TextureMap // STL���� MAP�� �����ϸ�, NUMBER - �̸� ����
{
	int Type_number;
	T_STR name;//3. TCHAR �ڵ� : ��Ȳ�� �°� �����ڵ�, ��Ƽ����Ʈ �˾Ƽ� ��ȯ�ؼ� �����Ϸ��� ���ư���. 
};

struct Material
{
	vector<TextureMap> TextureMap_List;
	vector<Material>Sub_Material_List;
};

struct Index
{
	int ASE_Index[3];
	int Sub_Material_number;
};

struct TAnimTrack
{
	int     iTick;
	int		Track_type_S1_R2_T3;
	D3DXQUATERNION  qRotate;
	D3DXVECTOR3   Vector_in_Animation;

public:

	TAnimTrack()
	{
		iTick = 0;
		Track_type_S1_R2_T3 = 0;
	}
};



struct OBJECT_GEO_HELPER  // �̷��� ���� �� ���� ���̾�?
{
	bool GEO_0_HELPER_1_Classification;
	int Material_Reference;
	T_STR node_name;
	T_STR node_parent_name;
	OBJECT_GEO_HELPER* Parent_GEO_HELPER_ptr;
	
	vector<D3DXVECTOR3>  vertexList;
	vector<Index> Position_Face_List;
	vector<D3DXVECTOR3> Texture_List;
	vector<Index> Texture_Face_List;
	vector<D3DXVECTOR3> Color_List;
	vector<Index> Color_Face_List;
	vector<D3DXVECTOR3> normal_List;

public: //�ִϸ��̼��� ����. �ϴ��� �̵���.

	D3DXMATRIX    m_matWorld; // ���� ���
	D3DXMATRIX    m_matWorldRotate; // ���� ȸ�� ���
	D3DXMATRIX    m_matWorldScale; // ���� ���� ���
	D3DXMATRIX    m_matWorldTrans; // ���� �̵� ���

	vector<TAnimTrack>  m_pSclTrack; // ���� Ʈ��
	vector<TAnimTrack>  m_pRotTrack; // ȸ�� Ʈ��
	vector<TAnimTrack>  m_PosTrack;  // �̵� Ʈ�� 
	vector<TAnimTrack>	m_pVisTrack; // �̰� ����??

	D3DXMATRIX			m_matCalculation; // ���� ��� ���
	D3DXMATRIX			m_matCalculation_acculated; // ���� ��� ���

	D3DXVECTOR3			BOUNDINGBOX_MIN_VECTOR;
	D3DXVECTOR3			BOUNDINGBOX_MAX_VECTOR;


public: // �̰� ���� ������

	float			    m_fVisibility;

public:


	OBJECT_GEO_HELPER()
	{
		Material_Reference = 1004;
		m_fVisibility = 0;
		Parent_GEO_HELPER_ptr = NULL;

		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixIdentity(&m_matWorldTrans);
		D3DXMatrixIdentity(&m_matWorldRotate);
		D3DXMatrixIdentity(&m_matWorldScale);
		D3DXMatrixIdentity(&m_matCalculation);
		D3DXMatrixIdentity(&m_matCalculation_acculated);
	}


};

class AseMesh
{
public:
	
	vector <Material> Material_List;
	vector <OBJECT_GEO_HELPER> OBJECT_GEO_HELPER_in_Object_List;
	

	TCHAR Main_Buffer[256];
	TCHAR one_string_Buffer[256];
	TCHAR one_string_Buffer_2[256];
	int Integer_Buffer;
	FILE* Stream;


public: // �� ������ ����

	Scene		  m_Scene;


public: //  Load_Geometry_Object() �Լ� �ȿ� ��� ������ �־��. �̰�, �׳� �������� ���Կ� ������ 

	int iNumFaces = 0; // �̰� ���� ����� ����
	int iNumVertex = 0;

public:
	bool Load(T_STR name) // ��ü ����
	{
		Stream = NULL;
		Stream = _tfopen(name.c_str(), _T("rt"));
		if (Stream == NULL) return false;

		if (Find_one_specific_string(_T("*SCENE")) == false) return false;
		Load_Scene();

		if (Find_one_specific_string(_T("*MATERIAL_LIST")) == false) return false;
		Load_Material();

		
		Load_Object();



		fclose(Stream);
		Help_meet_Parent_Child();
		return true;
	}

	bool Help_meet_Parent_Child()
	{
		if (this->OBJECT_GEO_HELPER_in_Object_List.size())
		{
			for (int i = 0; i < this->OBJECT_GEO_HELPER_in_Object_List.size(); i++)
			{
				for (int j = 0; j < this->OBJECT_GEO_HELPER_in_Object_List.size(); j++)
				{
					if (this->OBJECT_GEO_HELPER_in_Object_List[i].node_name
						== this->OBJECT_GEO_HELPER_in_Object_List[j].node_parent_name)
					{
						this->OBJECT_GEO_HELPER_in_Object_List[j].Parent_GEO_HELPER_ptr = &this->OBJECT_GEO_HELPER_in_Object_List[i];
					
						continue;
					}
				}
			}
		}
		return true;
	}


	bool Find_one_specific_string(T_STR string_aimed)
	{
		while (!feof(Stream))
		{
			_fgetts(Main_Buffer, 256, Stream);
			_stscanf(Main_Buffer, _T("%s"), one_string_Buffer);

			if (!_tcsicmp(one_string_Buffer, string_aimed.c_str()))  //������ 0��ȯ.
			{
				return true;
			}
		}
		return false; //�ٸ���,
	}

	bool Find_new_Object_starting_point()
	{
		while (!feof(Stream))
		{
			_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_SAMPLE 0 0.0000 0.0000 0.0000
			if (!_tcsicmp(Main_Buffer, _T("}\n")))  //������ 0��ȯ.
			{
				return true;
			}
		}
		return false; //�ٸ���,
	}

	void Get_POS_TRACK(OBJECT_GEO_HELPER& Geo_Helper_Object)
	{
		for (;;)
		{
			TAnimTrack David_Track; David_Track.Track_type_S1_R2_T3 = 3;
			_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_SAMPLE 0 0.0000 0.0000 0.0000
			if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //������ 0��ȯ.
			{
				break;
			}
			_stscanf(Main_Buffer, _T("%s%d%f%f%f"), one_string_Buffer, &David_Track.iTick,
				&David_Track.Vector_in_Animation.x, &David_Track.Vector_in_Animation.z, &David_Track.Vector_in_Animation.y);

			Geo_Helper_Object.m_PosTrack.push_back(David_Track);
		}

	}

	void Get_ROT_TRACK(OBJECT_GEO_HELPER& Geo_Helper_Object)
	{
		for (;;)
		{
			TAnimTrack David_Track; David_Track.Track_type_S1_R2_T3 = 2;
			_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_SAMPLE 0 0.0000 0.0000 0.0000
			if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //������ 0��ȯ.
			{
				break;
			}
			_stscanf(Main_Buffer, _T("%s%d%f%f%f%f"), one_string_Buffer, &David_Track.iTick,
				&David_Track.qRotate.x, &David_Track.qRotate.z, &David_Track.qRotate.y, &David_Track.qRotate.w);

			Geo_Helper_Object.m_pRotTrack.push_back(David_Track);
		}

	}

	void Get_SCALE_TRACK(OBJECT_GEO_HELPER& Geo_Helper_Object)
	{
		for (;;)
		{
			TAnimTrack David_Track; David_Track.Track_type_S1_R2_T3 = 1;

			_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_SAMPLE 0 0.0000 0.0000 0.0000
			if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //������ 0��ȯ.
			{
				break;
			}
			_stscanf(Main_Buffer, _T("%s%d %f%f%f %f%f%f%f"), one_string_Buffer, &David_Track.iTick, &David_Track.Vector_in_Animation.x, &David_Track.Vector_in_Animation.z, &David_Track.Vector_in_Animation.y,
				&David_Track.qRotate.x, &David_Track.qRotate.z, &David_Track.qRotate.y, &David_Track.qRotate.w);

			Geo_Helper_Object.m_pSclTrack.push_back(David_Track);
		}
	}


	
	
	void Load_Scene()
	{
		_fgetts(Main_Buffer, 256, Stream); //*SCENE_FILENAME "Box.max"�� �д´�

		_fgetts(Main_Buffer, 256, Stream); //*SCENE_FIRSTFRAME 0�� �д´�.
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &m_Scene.First_Frame);

		_fgetts(Main_Buffer, 256, Stream); //	*SCENE_LASTFRAME 100
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &m_Scene.Last_Frame);

		_fgetts(Main_Buffer, 256, Stream); //	*SCENE_FRAMESPEED 30
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &m_Scene.Frames_per_second);

		_fgetts(Main_Buffer, 256, Stream); //	*SCENE_TICKSPERFRAME 30
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &m_Scene.Tick_Per_Frame);
	}

	void Load_Material() 
	{
		int i_MATERIAL_COUNT = 0;

		_fgetts(Main_Buffer, 256, Stream);//*MATERIAL_COUNT 2 �о�� �մϴ�.
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &i_MATERIAL_COUNT);//*MATERIAL_COUNT 2 �о�� �մϴ�.


		for (int iCnt = 0; iCnt < i_MATERIAL_COUNT; iCnt++) // ���⼭�� �� 2�� ���ϴ�
		{
			Material yang_Material;// ���⼭ ���������, �� ���� ���Ϳ� ����Ǿ� ����.
			TextureMap yang_TextureMap; // ���⼭ ����. �� ���Ϳ� �����.
		
			if (Find_one_specific_string(_T("*MATERIAL_CLASS")) == false) return; // �̹� �̰ɷ� ��� ���ι���, ����Ʈ���� ����Ǿ� �ִ�.

			TCHAR szClass[256] = { 0, };

			_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, szClass); //*MATERIAL_CLASS "Standard"
			
			int iSubMtrl = 0;

			if (!_tcsicmp(szClass, _T("\"Multi/Sub-Object\""))) // ������ ������ ���ϴ�. ó���� ���� �ʽ��ϴ�.//������ 0 ��ȯ, �ٸ��� 0�� �ƴ� ������ ��ȯ.	
			{
				if (Find_one_specific_string(_T("*NUMSUBMTLS")) == false) return; // �� �̰� ������,
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer,&iSubMtrl); //iSubMtrl�� 5�� ����ȴ�.
			}

			if (iSubMtrl == 0) // ó�� �̰ͺ��� �մϴ�.
			{
				if (Find_one_specific_string(_T("*MAP_SUBNO")) == false) return; // *MATERIAL 0 { �� �ڽ����� *MAP_SUBNO 1 �̰� �־��.

				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &yang_TextureMap.Type_number); // ���� ���� ���̶���? �׷������ε�.
				_fgetts(Main_Buffer, 256, Stream);//*MAP_AMOUNT 1.0000
				_fgetts(Main_Buffer, 256, Stream);//*BITMAP "D:\00_TBasisTutorials\Lecture_010\data\cncr25S.bmp"
				_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);

				TCHAR drive[256] = { 0, };
				TCHAR dir[256] = { 0, };
				TCHAR fname[256] = { 0, };
				TCHAR ext[256] = { 0, };

				_tsplitpath(one_string_Buffer_2, drive, dir, fname, ext);

				yang_TextureMap.name = fname; // �̰� �� �� ���� �ؽ��� ���̿���. �Ϻη� �����߾��.
				ext[4] = 0;
				yang_TextureMap.name += ext;
				yang_Material.TextureMap_List.push_back(yang_TextureMap);
			}

			else // 5�� ����Ǿ� �����ϱ�,
			{
				for (int iSub = 0; iSub < iSubMtrl; iSub++) //5���� ��ȯ�ϴ�.   // *SUBMATERIAL 0 { �� �̹� ���ٱ��� �Ծ��.                                
				{
					Material    Sub_Material;
					TextureMap  Sub_Map;

					if (Find_one_specific_string(_T("*MAP_DIFFUSE")) == false) return;
					if (Find_one_specific_string(_T("*MAP_SUBNO")) == false) return;

					//*SUBMATERIAL 0 { �ȿ�	*MAP_SUBNO 1 �־��.
					//*SUBMATERIAL 1 { �ȿ� *MAP_SUBNO 1 �־��.

					_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &Sub_Map.Type_number);
					_fgetts(Main_Buffer, 256, Stream);//*MAP_AMOUNT 1.0000
					_fgetts(Main_Buffer, 256, Stream);//*BITMAP "D:\test\STAGE_02\st02_sc00\st02_sc00_map\0_st02_sc00_g00.dds"
					_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);

					TCHAR drive[256] = { 0, };
					TCHAR dir[256] = { 0, };
					TCHAR fname[256] = { 0, };
					TCHAR ext[256] = { 0, };
					_tsplitpath(one_string_Buffer_2, drive, dir, fname, ext);

					Sub_Map.name = fname;
					ext[4] = 0;
					Sub_Map.name += ext;
					Sub_Material.TextureMap_List.push_back(Sub_Map);
					yang_Material.Sub_Material_List.push_back(Sub_Material);
				}

			}
			Material_List.push_back(yang_Material);
		}
	}



	void Load_Object()
	{
		while (!feof(Stream))
		{
			if (_tcsicmp(one_string_Buffer, _T("}")))
			{ 
			Find_new_Object_starting_point();
			}

			OBJECT_GEO_HELPER Geo_Helper_Object;

			_fgetts(Main_Buffer, 256, Stream);//*HELPEROBJECT {
			_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);// *HELPEROBJECT | {

			if (!_tcsicmp(one_string_Buffer, _T("*GEOMOBJECT")))
			{
				Geo_Helper_Object.GEO_0_HELPER_1_Classification = 0;
			}

			else if (!_tcsicmp(one_string_Buffer, _T("*HELPEROBJECT")))
			{	
				Geo_Helper_Object.GEO_0_HELPER_1_Classification = 1;
			}

			switch (Geo_Helper_Object.GEO_0_HELPER_1_Classification)
			{
				case GEOMOBJECT: //Geo
				{
					Load_Geometry_Object(Geo_Helper_Object);
				}break;

				case HELPEROBJECT:
				{
					Load_Helper_Object(Geo_Helper_Object);
				}break;
			}
		}
	}


	void Load_Geometry_Object(OBJECT_GEO_HELPER& Geo_Helper_Object)
	{

		_fgetts(Main_Buffer, 256, Stream); //*NODE_NAME "st02_sc00_g" // �� ������ *NODE_NAME "Box01"
		_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);//"st02_sc00_g" // "Box01"
		Geo_Helper_Object.node_name = one_string_Buffer_2; //"st02_sc00_g"

		_fgetts(Main_Buffer, 256, Stream);//*NODE_PARENT "Box001"
		_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);//*NODE_PARENT "Box001"
		
		if (!_tcsicmp(one_string_Buffer, _T("*NODE_PARENT")))//������ 0��ȯ.
		{
			Geo_Helper_Object.node_parent_name = one_string_Buffer_2; //*NODE_PARENT "Box001"
		}

		D3DXMatrixIdentity(&Geo_Helper_Object.m_matWorld); // �� ������Ʈ�� ������Ʈ ���� ����� ����������Ѵ�

		if (Find_one_specific_string(_T("*TM_ROW0")) == false) return; //
		Get_Data(&Geo_Helper_Object.m_matWorld.m[0]); //*TM_ROW0 1.0000	0.0000	0.0000 �ɰ� ����.

		_fgetts(Main_Buffer, 256, Stream); //*TM_ROW1 0.0000	1.0000	0.0000
		Get_Data(&Geo_Helper_Object.m_matWorld.m[2]);

		_fgetts(Main_Buffer, 256, Stream);//	*TM_ROW2 0.0000	0.0000	1.0000
		Get_Data(&Geo_Helper_Object.m_matWorld.m[1]);

		_fgetts(Main_Buffer, 256, Stream); //	*TM_ROW3 -0.0773 -0.1949 0.0000
		Get_Data(&Geo_Helper_Object.m_matWorld.m[3]);

	
		D3DXVECTOR3 David_Vector;
		float  David_Angle;
		D3DXQUATERNION qRotation;
		_fgetts(Main_Buffer, 256, Stream); 
		Get_Data(&David_Vector); //*TM_POS 0.0000 0.0000 0.0000�ɰ� ����.
		D3DXMatrixTranslation(&Geo_Helper_Object.m_matWorldTrans, David_Vector.x, David_Vector.y, David_Vector.z);
		_fgetts(Main_Buffer, 256, Stream); 
		Get_Data(&David_Vector); // *TM_ROTAXIS 0.0000 0.0000 0.0000
		_fgetts(Main_Buffer, 256, Stream); 
		Get_Float_Data(&David_Angle); //*TM_ROTANGLE 0.0000
									  // ������ ��� ���� ���ʹϾ����� ��ȯ
		D3DXQuaternionRotationAxis(&qRotation, &David_Vector, David_Angle);
		D3DXMatrixRotationQuaternion(&Geo_Helper_Object.m_matWorldRotate, &qRotation);
		//���������� �߽����� ȸ���ϴ� ���� ������ ���̹Ƿ� 
		//���������� ȸ����ŭ�� ���� �ݴ�� ȸ���� ����
		//�����ϰ��� �����Ű�� �� ������ 
		//�ٽ� �������ุŭ�� ȸ������ ���󺹱� ��Ų��. = ������ �� �����ϸ�.
		D3DXMATRIX matScl;
		_fgetts(Main_Buffer, 256, Stream);
		Get_Data(&David_Vector); // *TM_SCALE 1.0000 1.0000 1.0000
		D3DXMatrixScaling(&Geo_Helper_Object.m_matWorldScale, David_Vector.x, David_Vector.y, David_Vector.z);
		D3DXVECTOR3 David_Axis;
		_fgetts(Main_Buffer, 256, Stream); 
		Get_Data(&David_Axis);// *TM_SCALEAXIS 0.0000 0.0000 0.0000
		_fgetts(Main_Buffer, 256, Stream);
		Get_Float_Data(&David_Angle); // *TM_SCALEAXISANG 0.0000

									  // ���������� ��İ� �� ������� ���Ѵ�.
		D3DXMATRIX matRotation, matRotationInv;
		D3DXMatrixRotationAxis(&matRotation, &David_Axis, David_Angle);
		D3DXMatrixInverse(&matRotationInv, NULL, &matRotation);
		Geo_Helper_Object.m_matWorldScale = matRotationInv * Geo_Helper_Object.m_matWorldScale  * matRotation;


		if (Find_one_specific_string(_T("*MESH")) == false) return;

		{
			_fgetts(Main_Buffer, 256, Stream);//*TIMEVALUE 0
			_fgetts(Main_Buffer, 256, Stream); //*MESH_NUMVERTEX 2656
			_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumVertex);//��ġ ���� ������ = 2656


			if (iNumVertex > 0)//��ġ ���� ������ = 2656 �̴�
			{
				_fgetts(Main_Buffer, 256, Stream); //*MESH_NUMFACES 4158 // ��Ȯ�� ���̽��� = 4158
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumFaces);  // ��Ȯ�� ���̽��� = 4158

				_fgetts(Main_Buffer, 256, Stream);//*MESH_VERTEX_LIST {

				Geo_Helper_Object.vertexList.resize(iNumVertex);//��ġ ���� ������ = 2656

																// �߿���. Vertex���� ���ư�.

				for (int iVertex = 0; iVertex < iNumVertex; iVertex++) // ���ؽ� 0���� �� ���ư�.
				{
					LoadVertexData(Geo_Helper_Object.vertexList[iVertex], true);
				} // 	*MESH_VERTEX   0 -5.0773	-5.1949	0.0000 ���� *MESH_VERTEX 7 4.9227	4.8051	10.0000 �� ��.
				  //���� vertexList
				_fgetts(Main_Buffer, 256, Stream); // }
				_fgetts(Main_Buffer, 256, Stream); // 	*MESH_FACE_LIST {

				Geo_Helper_Object.Position_Face_List.resize(iNumFaces); //�տ��� ���̽��� = 4158  ����.

																		//���� Position_Face_List. ���⼭ �� ������

				for (int iFace = 0; iFace < iNumFaces; iFace++)
				{
					_fgetts(Main_Buffer, 256, Stream);		// *MESH_FACE    0:    A:    0 B: 2 C: 3 AB : 1 BC : 1 CA : 0 * MESH_SMOOTHING 2 * MESH_MTLID 1
					_stscanf(Main_Buffer, _T("%s%s %s%d %s%d %s%d %s%d %s%d %s %d %s%d %s%d"),

						one_string_Buffer, one_string_Buffer, one_string_Buffer, //*MESH_FACE    0:    A: 
						&Geo_Helper_Object.Position_Face_List[iFace].ASE_Index[0], // ù��° ���ؽ� //0
						one_string_Buffer,//B:
						&Geo_Helper_Object.Position_Face_List[iFace].ASE_Index[2], // �ð� �ݽð� ������ ��¤�����ϴ� // 1
						one_string_Buffer,//C:
						&Geo_Helper_Object.Position_Face_List[iFace].ASE_Index[1], //2
						one_string_Buffer, &Integer_Buffer,//AB:    1 
						one_string_Buffer, &Integer_Buffer,//BC:    1
						one_string_Buffer, &Integer_Buffer,//CA:   1
						one_string_Buffer, &Integer_Buffer,//*MESH_SMOOTHING 
						one_string_Buffer, &Geo_Helper_Object.Position_Face_List[iFace].Sub_Material_number);// �̰� ���� �ǹ��ִ� ������. �ٽ�������. 
				}
			}

			//�ϴ� �� ������. �׸��� Sub_Material_number��, ���߿� ���������Ʈ��� �����ǰ���.


			_fgetts(Main_Buffer, 256, Stream); // }
			_fgetts(Main_Buffer, 256, Stream);//	*MESH_NUMTVERTEX 3803
			_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumVertex); //�Ʊ� 8�̿��ݾ�. ������ 12���?

			if (iNumVertex > 0)
			{
				_fgetts(Main_Buffer, 256, Stream); // *MESH_NUMTVERTEX 3803 *MESH_TVERTLIST { // ���� �̰� ó��!
				Geo_Helper_Object.Texture_List.resize(iNumVertex); // 3803. // vector<D3DXVECTOR3> Texture_List;

																   //Texture_List ���� �̰� ó��!
				for (int iVertex = 0; iVertex < iNumVertex; iVertex++) //iNumVertex=3803 ���� ����.
				{
					LoadVertexData(Geo_Helper_Object.Texture_List[iVertex], false); //_fgetts(Main_Buffer, 256, Stream); _stscanf(Main_Buffer, _T("%s%d%f%f%f"),	one_string_Buffer,	&Integer_Buffer,	&D_Vec.x,	&D_Vec.y,	&D_Vec.z);
					Geo_Helper_Object.Texture_List[iVertex].y = 1.0f - Geo_Helper_Object.Texture_List[iVertex].y; // �� 1.0���� �P��? MAX�� �Ʒ����ʿ��� �ؽ��İ� �����ϹǷ�.
				}
				//*MESH_NUMTVFACES 4158
				_fgetts(Main_Buffer, 256, Stream);//	}
				_fgetts(Main_Buffer, 256, Stream);//	*MESH_NUMTVFACES 4158
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumFaces);//���̽��� = 4158 �տ��� ����.
				_fgetts(Main_Buffer, 256, Stream);//	*MESH_TFACELIST {

												  //vector<Index> Texture_Face_List;�� ó����.
				Geo_Helper_Object.Texture_Face_List.resize(iNumFaces);////���̽��� = 4158 �տ��� ����.

				for (int iFace = 0; iFace < iNumFaces; iFace++)//���̽��� = 4158 �տ��� ����. ���̽� 0���� ���鼭.
				{ //	vector<Index> Texture_Face_List;
					LoadIndexData(Geo_Helper_Object.Texture_Face_List[iFace]);//&D_Index.ASE_Index[0],	&D_Index.ASE_Index[2],	&D_Index.ASE_Index[1]) �� ����.
				}
			}

			//�������ʹ� Į��

			// *MESH_NUMCVERTEX 2656
			_fgetts(Main_Buffer, 256, Stream);//	}
			_fgetts(Main_Buffer, 256, Stream);//*MESH_NUMCVERTEX 2656

											  // Į�� �ִٸ�, �̷��� ����� ��.

			_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumVertex); //2656�� ��. // Į�� �ִٸ�, �̷��� ����� ��.
			Geo_Helper_Object.Color_List.resize(iNumVertex);//2656

			if (iNumVertex > 0) // ����� 0 ��.
			{

				_fgetts(Main_Buffer, 256, Stream);
				for (int iVertex = 0; iVertex < iNumVertex; iVertex++)
				{
					LoadVertexData(Geo_Helper_Object.Color_List[iVertex], false); //vector<D3DXVECTOR3> Color_List;// RGB�ϱ� ���� �״�� ����
				}
				//*MESH_NUMCVFACES 4158			
				_fgetts(Main_Buffer, 256, Stream);//}
				_fgetts(Main_Buffer, 256, Stream);//*MESH_NUMCVFACES 4158
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumFaces);//iNumFaces=4158
																				 //*MESH_CFACELIST 0

																				 //�Ϻη� �߸����ǰ�? ���� ���� ���.

				Geo_Helper_Object.Color_Face_List.resize(iNumFaces);

				for (int iFace = 0; iFace < iNumFaces; iFace++)
				{
					/**MESH_CFACE 0	0	1	2
					* MESH_CFACE 1	0	3	1
					* MESH_CFACE 2	0	4	3*/
					LoadIndexData(Geo_Helper_Object.Color_Face_List[iFace]);
				}
			}
			//�������ʹ� ���
			//*MESH_NORMALS{

			Geo_Helper_Object.normal_List.resize(iNumFaces * 3); // ������ֵ鸸 ���� ���̱⿡ *3

			_fgetts(Main_Buffer, 256, Stream);//*MESH_NORMALS {
			for (int iFace = 0; iFace < iNumFaces; iFace++) // 12��.
			{
				_fgetts(Main_Buffer, 256, Stream); // ���̽� ��� ���� �ſ���.
				LoadVertexData(Geo_Helper_Object.normal_List[iFace * 3 + 0]); //���̽��� ������ֵ� 3��.
				LoadVertexData(Geo_Helper_Object.normal_List[iFace * 3 + 2]); // �� �ٲ�ĸ�, �ð� �ݽð�
				LoadVertexData(Geo_Helper_Object.normal_List[iFace * 3 + 1]);
			}
		}
		
		// �ִϸ��̼� Ȥ�� ���͸��� ���۷��� �߰�.

		_fgetts(Main_Buffer, 256, Stream); //		}
		_fgetts(Main_Buffer, 256, Stream); //  	}
		_fgetts(Main_Buffer, 256, Stream); //*PROP_MOTIONBLUR 0
		_fgetts(Main_Buffer, 256, Stream); //*PROP_CASTSHADOW 1
		_fgetts(Main_Buffer, 256, Stream); //*PROP_RECVSHADOW 1
		_fgetts(Main_Buffer, 256, Stream); // *MATERIAL_REF 0 Ȥ��

		_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);

		// ���߿� �ϳ���.
		// *TM_ANIMATION {
		// *MATERIAL_REF 0

		if (!_tcsicmp(one_string_Buffer, _T("*TM_ANIMATION")))	
		{
			if (Find_one_specific_string(_T("*CONTROL_ROT_TRACK")) == true)
			{
				for (;;)
				{
					TAnimTrack David_Track; David_Track.Track_type_S1_R2_T3 = 2;
					_fgetts(Main_Buffer, 256, Stream); //   *CONTROL_ROT_SAMPLE 160 0.0000 0.0000 -1.0000 0.0051

					if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //������ 0��ȯ.
					{
						break;
					}
					_stscanf(Main_Buffer, _T("%s%d%f%f%f%f"), one_string_Buffer, &David_Track.iTick,
					&David_Track.qRotate.x, &David_Track.qRotate.z, &David_Track.qRotate.y, &David_Track.qRotate.w);

					Geo_Helper_Object.m_pRotTrack.push_back(David_Track);
				}
			}

			if (Find_one_specific_string(_T("*MATERIAL_REF")) == false)
				return; // ���ι���, ����Ʈ�� ���� ��.
			_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &Geo_Helper_Object.Material_Reference); // ������Ʈ�� �޽� ������Ʈ�� �Լ� ������ ���ǵƴٰ�, ���� ���Ϳ� �����!
		}

		else if (!_tcsicmp(one_string_Buffer, _T("*MATERIAL_REF")))
		{
			_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &Geo_Helper_Object.Material_Reference);
			//Geo_Helper_Object.Material_Reference = *one_string_Buffer_2;
		}
		_fgetts(Main_Buffer, 256, Stream);
		_stscanf(Main_Buffer, _T("%s"), one_string_Buffer);

		OBJECT_GEO_HELPER_in_Object_List.push_back(Geo_Helper_Object);
	}
	

	void Load_Helper_Object(OBJECT_GEO_HELPER& Geo_Helper_Object)
	{

		_fgetts(Main_Buffer, 256, Stream); //	*NODE_NAME "Dummy02"
		_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);
		Geo_Helper_Object.node_name = one_string_Buffer_2; //"Dummy02"

		_fgetts(Main_Buffer, 256, Stream);//*NODE_PARENT "Dummy01"
		_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);//*NODE_PARENT "Box001"

		if (!_tcsicmp(one_string_Buffer, _T("*NODE_PARENT")))//������ 0��ȯ.
		{
			Geo_Helper_Object.node_parent_name = one_string_Buffer_2; //"Dummy01"
		}

		D3DXMatrixIdentity(&Geo_Helper_Object.m_matWorld); // �� ������Ʈ�� ������Ʈ ���� ����� ����������Ѵ�

		if (Find_one_specific_string(_T("*TM_ROW0")) == false) return; //
		Get_Data(&Geo_Helper_Object.m_matWorld.m[0]); //*TM_ROW0 1.0000	0.0000	0.0000 �ɰ� ����.

		_fgetts(Main_Buffer, 256, Stream); //*TM_ROW1 0.0000	1.0000	0.0000
		Get_Data(&Geo_Helper_Object.m_matWorld.m[2]);

		_fgetts(Main_Buffer, 256, Stream);//	*TM_ROW2 0.0000	0.0000	1.0000
		Get_Data(&Geo_Helper_Object.m_matWorld.m[1]);

		_fgetts(Main_Buffer, 256, Stream); //	*TM_ROW3 -0.0773 -0.1949 0.0000
		Get_Data(&Geo_Helper_Object.m_matWorld.m[3]);


		D3DXVECTOR3 David_Vector;
		float  David_Angle;
		D3DXQUATERNION qRotation;
		_fgetts(Main_Buffer, 256, Stream);
		Get_Data(&David_Vector); //*TM_POS 0.0000 0.0000 0.0000�ɰ� ����.
		D3DXMatrixTranslation(&Geo_Helper_Object.m_matWorldTrans, David_Vector.x, David_Vector.y, David_Vector.z);
		_fgetts(Main_Buffer, 256, Stream);
		Get_Data(&David_Vector); // *TM_ROTAXIS 0.0000 0.0000 0.0000
		_fgetts(Main_Buffer, 256, Stream);
		Get_Float_Data(&David_Angle); //*TM_ROTANGLE 0.0000
									  // ������ ��� ���� ���ʹϾ����� ��ȯ
		D3DXQuaternionRotationAxis(&qRotation, &David_Vector, David_Angle);
		D3DXMatrixRotationQuaternion(&Geo_Helper_Object.m_matWorldRotate, &qRotation);
		//���������� �߽����� ȸ���ϴ� ���� ������ ���̹Ƿ� 
		//���������� ȸ����ŭ�� ���� �ݴ�� ȸ���� ����
		//�����ϰ��� �����Ű�� �� ������ 
		//�ٽ� �������ุŭ�� ȸ������ ���󺹱� ��Ų��. = ������ �� �����ϸ�.
		D3DXMATRIX matScl;
		_fgetts(Main_Buffer, 256, Stream);
		Get_Data(&David_Vector); // *TM_SCALE 1.0000 1.0000 1.0000
		D3DXMatrixScaling(&Geo_Helper_Object.m_matWorldScale, David_Vector.x, David_Vector.y, David_Vector.z);
		D3DXVECTOR3 David_Axis;
		_fgetts(Main_Buffer, 256, Stream);
		Get_Data(&David_Axis);// *TM_SCALEAXIS 0.0000 0.0000 0.0000
		_fgetts(Main_Buffer, 256, Stream);
		Get_Float_Data(&David_Angle); // *TM_SCALEAXISANG 0.0000

									  // ���������� ��İ� �� ������� ���Ѵ�.
		D3DXMATRIX matRotation, matRotationInv;
		D3DXMatrixRotationAxis(&matRotation, &David_Axis, David_Angle);
		D3DXMatrixInverse(&matRotationInv, NULL, &matRotation);
		Geo_Helper_Object.m_matWorldScale = matRotationInv * Geo_Helper_Object.m_matWorldScale  * matRotation;


		_fgetts(Main_Buffer, 256, Stream); //	/t}
		_fgetts(Main_Buffer, 256, Stream); //*BOUNDINGBOX_MIN - 3.4462 - 3.4462 - 5.1063
		Get_Data(&Geo_Helper_Object.BOUNDINGBOX_MIN_VECTOR); //*TM_POS 0.0000 0.0000 0.0000�ɰ� ����.

		_fgetts(Main_Buffer, 256, Stream); //*BOUNDINGBOX_MAX  3.4462	3.4462	1.7861
		Get_Data(&Geo_Helper_Object.BOUNDINGBOX_MAX_VECTOR); //*TM_POS 0.0000 0.0000 0.0000�ɰ� ����.

		_fgetts(Main_Buffer, 256, Stream);
		_stscanf(Main_Buffer, _T("%s"), one_string_Buffer);

		if (!_tcsicmp(Main_Buffer, _T("}\n")))  //������ 0��ȯ.
		{
			OBJECT_GEO_HELPER_in_Object_List.push_back(Geo_Helper_Object);
			return;
		}
		// �ִϸ��̼� �ִ�. Reading

		_fgetts(Main_Buffer, 256, Stream); //*NODE_NAME "Bone18"
		_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_TRACK { Ȥ�� *CONTROL_ROT_TRACK {
		_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, &one_string_Buffer_2);

		if (!_tcsicmp(one_string_Buffer, _T("*CONTROL_POS_TRACK")))  //������ 0��ȯ.
		{
			Get_POS_TRACK(Geo_Helper_Object);
		}

		else if (!_tcsicmp(one_string_Buffer, _T("*CONTROL_ROT_TRACK")))  //������ 0��ȯ.
		{
			Get_ROT_TRACK(Geo_Helper_Object);
		}

		else if (!_tcsicmp(one_string_Buffer, _T("*CONTROL_SCALE_TRACK")))
		{
			Get_SCALE_TRACK(Geo_Helper_Object);
		}

		_fgetts(Main_Buffer, 256, Stream); //	�ִϸ��̼� ������. *CONTROL_ROT_TRACK { �ٸ� ���� Ʈ���� �� �ֳ�. ������
		_stscanf(Main_Buffer, _T("%s"), one_string_Buffer);

		if(!_tcsicmp(one_string_Buffer, _T("}")))  //������ 0��ȯ.
		{
			_fgetts(Main_Buffer, 256, Stream); 
			_stscanf(Main_Buffer, _T("%s"), one_string_Buffer);
			OBJECT_GEO_HELPER_in_Object_List.push_back(Geo_Helper_Object);
			return;
		}

		else if (!_tcsicmp(one_string_Buffer, _T("*CONTROL_ROT_TRACK")))  //������ 0��ȯ.
		{
			Get_ROT_TRACK(Geo_Helper_Object);
		}

		else if (!_tcsicmp(one_string_Buffer, _T("*CONTROL_SCALE_TRACK")))
		{
			Get_SCALE_TRACK(Geo_Helper_Object);
		}

		_fgetts(Main_Buffer, 256, Stream); //	�ִϸ��̼� ������. *Get_SCALE_TRACK { �ٸ� ���� Ʈ���� �� �ֳ�. ������
		_stscanf(Main_Buffer, _T("%s"), one_string_Buffer);


		if (!_tcsicmp(one_string_Buffer, _T("*CONTROL_SCALE_TRACK")))
		{
			Get_SCALE_TRACK(Geo_Helper_Object);
		}
		_fgetts(Main_Buffer, 256, Stream); //	�ִϸ��̼� ������. *Get_SCALE_TRACK { �ٸ� ���� Ʈ���� �� �ֳ�. ������
		_stscanf(Main_Buffer, _T("%s"), one_string_Buffer);

		OBJECT_GEO_HELPER_in_Object_List.push_back(Geo_Helper_Object);

		return;
	}





	void Get_Data(void* pData) // ���� ���ۿ� ��� �ִ� ������, ���� ��� *TM_ROW0 1.0000	0.0000	0.0000 �� ���� �ɰ� �ִ´�. ��, ���� ���� �迭�� �־����, �װ� ���е� ������ �ִ´�
	{

		_stscanf(Main_Buffer, _T("%s%f%f%f"),
			one_string_Buffer,
			&((D3DXVECTOR3*)pData)->x,
			&((D3DXVECTOR3*)pData)->z,
			&((D3DXVECTOR3*)pData)->y);

	}



	void Get_Float_Data(void* pData) // ���� ���ۿ� ��� �ִ� ������, ���� ��� *TM_ROW0 1.0000	0.0000	0.0000 �� ���� �ɰ� �ִ´�. ��, ���� ���� �迭�� �־����, �װ� ���е� ������ �ִ´�
	{

		_stscanf(Main_Buffer, _T("%s%f"),one_string_Buffer, pData);

	}


	void LoadVertexData(D3DXVECTOR3& D_Vec, bool bSwap=true)
	{
		_fgetts(Main_Buffer, 256, Stream); //*MESH_VERTEX    0	-5.0773	-5.1949	0.0000

		if (bSwap)
		{
			_stscanf(Main_Buffer, _T("%s%d%f%f%f"),	one_string_Buffer,	&Integer_Buffer,	&D_Vec.x,	&D_Vec.z,	&D_Vec.y); //Y�� Z�� �ٲ��.
		}
		else
		{
			_stscanf(Main_Buffer, _T("%s%d%f%f%f"),	one_string_Buffer,	&Integer_Buffer,	&D_Vec.x,	&D_Vec.y,	&D_Vec.z); // �ȹٲٰ� �״�δ�. Į���̴�.
		}


	}
	void LoadIndexData(Index& D_Index)
	{
		_fgetts(Main_Buffer, 256, Stream);
		_stscanf(Main_Buffer, _T("%s%d%d%d%d"),	one_string_Buffer,&Integer_Buffer,	&D_Index.ASE_Index[0],	&D_Index.ASE_Index[2],	&D_Index.ASE_Index[1]); // �̰� YZ���� �ƴ϶�, �ð�ݽð�ƴϾ�?
	}

public:
	AseMesh() {};
	virtual ~AseMesh() {}
};




