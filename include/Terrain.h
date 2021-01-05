#pragma once
#include<iostream>
#include<glad/include/glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm/glm.hpp>
#include<vector>
#include"Ray.h"
#include"jepgfunc.h"

class Terrain
{
public:
	Terrain(const char* path, float* HighMap, int map_size, float step_size, glm::vec2 backleft)
	{
		MAP_SIZE = map_size;
		STEP_SIZE = step_size;
		BACK = backleft.x;
		LEFT = backleft.y;
		HIGH_MAX = 0;

		int row, col, channel;
		vector<uint8_t> temp_high_uint;
		readJPEG(path, temp_high_uint, row, col, channel);

		vector<float> temp_high_float;
		for (int i = 0; i < temp_high_uint.size(); i++)
		{
			if (i % 3 == 0)
			{
				temp_high_float.push_back((float)temp_high_uint[i]);
			}
		}

		vector<float> temp_high_fliter = fliter(temp_high_float);

		for (int i = 0; i < temp_high_fliter.size(); i++)
		{
			HighMap[i] = temp_high_fliter[i];
		}

	}

	void ChangeTerrain(const char* path, float* HighMap, int map_size, float step_size, glm::vec2 backleft)
	{
		MAP_SIZE = map_size;
		STEP_SIZE = step_size;
		BACK = backleft.x;
		LEFT = backleft.y;
		HIGH_MAX = 0;

		int row, col, channel;
		vector<uint8_t> temp_high_uint;
		readJPEG(path, temp_high_uint, row, col, channel);

		vector<float> temp_high_float;
		for (int i = 0; i < temp_high_uint.size(); i++)
		{
			if (i % 3 == 0)
			{
				temp_high_float.push_back((float)temp_high_uint[i]);
			}
		}

		vector<float> temp_high_fliter = fliter(temp_high_float);

		for (int i = 0; i < temp_high_fliter.size(); i++)
		{
			HighMap[i] = temp_high_fliter[i];
		}

	}

	vector<float> fliter(vector<float > temp_high_float)
	{
		int row, col, mask_size = 9;
		int mask_half = (mask_size - 1) / 2;
		float res_temp;
		vector<float> res;
		for (int i = 0; i < temp_high_float.size(); i++)
		{
			row = i / MAP_SIZE;
			col = i % MAP_SIZE;
			if (row < mask_half || col < mask_half || row >= MAP_SIZE - mask_half || col >= MAP_SIZE - mask_half)
			{
				res_temp = temp_high_float[i];
			}
			else
			{
				res_temp = 0;
				for (int j = row - mask_half; j <= row + mask_half; j++)
				{
					for (int k = col - mask_half; k <= col + mask_half; k++)
					{
						res_temp += temp_high_float[j * MAP_SIZE + k];
					}
				}
				res_temp = res_temp / (mask_size * mask_size);
			}
			res.push_back(res_temp);
		}
		return res;
	}

	float getHeight(float HighMap[], int px, int pz) {
		int x = px % MAP_SIZE;
		int z = pz % MAP_SIZE;

		float y = 0;
		if (px >= 0 && pz >= 0)
		{
			y = HighMap[x + (z * MAP_SIZE)];
			if (y > HIGH_MAX)
			{
				HIGH_MAX = y;
			}
		}

		return y / 5; ///                              
	}

	void getNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3& result) {
		const GLfloat l1[] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
		const GLfloat l2[] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
		//���������
		GLfloat n[] = {
				l1[1] * l2[2] - l1[2] * l2[1],
				l1[2] * l2[0] - l1[0] * l2[2],
				l1[0] * l2[1] - l1[1] * l2[0]
		};
		//��һ��
		GLfloat abs = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		n[0] /= abs;
		n[1] /= abs;
		n[2] /= abs;
		//??/ 2
		result.x = n[0] / 2;
		result.y = n[1] / 2;
		result.z = n[2] / 2;
	}

	void putinVN(float HighMap[])//��������Ϣ(��ƽ�淨����)��������
	{
		int i, j, count = 0;
		for (i = 0; i < (MAP_SIZE - 1); i++)
		{
			for (j = 0; j < (MAP_SIZE - 1); j++)
			{
				glm::vec3 p1;
				p1.x = BACK + j * STEP_SIZE;
				p1.z = LEFT + i * STEP_SIZE;
				p1.y = getHeight(HighMap, j, i);

				glm::vec3 p2;
				p2.x = BACK + (j + 1) * STEP_SIZE;
				p2.z = LEFT + i * STEP_SIZE;
				p2.y = getHeight(HighMap, j + 1, i);

				glm::vec3 p3;
				p3.x = BACK + j * STEP_SIZE;
				p3.z = LEFT + (i + 1) * STEP_SIZE;
				p3.y = getHeight(HighMap, j, i + 1);

				glm::vec3 nor;
				getNormal(p1, p2, p3, nor);

				glm::vec3 p4;
				p4.x = BACK + (j + 1) * STEP_SIZE;
				p4.z = LEFT + (i + 1) * STEP_SIZE;
				p4.y = getHeight(HighMap, j + 1, i + 1);

				//p5=p3;p6=p2
				getNormal(p4, p3, p2, nor);
				//ע������õ�����ƽ�淨������������Ҫ���Ƕ��㷨����
				vertics.push_back(p1);
				vertics.push_back(p2);
				vertics.push_back(p3);
				vertics.push_back(p4);
				vertics.push_back(p3);
				vertics.push_back(p2);
				count = i * MAP_SIZE + j;
				vertics_index.push_back(count);
				vertics_index.push_back(count + 1);
				vertics_index.push_back(count + MAP_SIZE);
				vertics_index.push_back(count + MAP_SIZE + 1);
				vertics_index.push_back(count + MAP_SIZE);
				vertics_index.push_back(count + 1);

			}
		}
		int ind;
		float ind_row, ind_col;
		for (int k = 0; k < vertics.size(); k++) {
			ind = vertics_index[k];
			ind_row = (float)(ind / MAP_SIZE);
			ind_col = (float)(ind % MAP_SIZE);
			glm::vec2 tmp(ind_col / MAP_SIZE, ind_row / MAP_SIZE);
			texCoords.push_back(tmp);
		}

	}

	void normalize(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4,
		glm::vec3 v5, glm::vec3 v6, glm::vec3& result) {
		GLfloat x = v1[0] + v2[0] + v3[0] + v4[0] + v5[0] + v6[0];
		GLfloat y = v1[1] + v2[1] + v3[1] + v4[1] + v5[1] + v6[1];
		GLfloat z = v1[2] + v2[2] + v3[2] + v4[2] + v5[2] + v6[2];
		GLfloat l = sqrt(x * x + y * y + z * z);
		result[0] = x / l;
		result[1] = y / l;
		result[2] = z / l;
	}

	void putinPointNormal(float HighMap[])//�����㷨����Ū��ȥ
	{
		glm::vec3 p1, p2, p3, p4, p5, p6, p7;
		glm::vec3 nor1, nor2, nor3, nor4, nor5, nor6, nor_res;
		int i, j;
		for (i = 0; i < MAP_SIZE; i++)
		{
			for (j = 0; j < MAP_SIZE; j++)
			{

				p1.x = BACK + j * STEP_SIZE;
				p1.z = LEFT + i * STEP_SIZE;
				p1.y = getHeight(HighMap, j, i);

				//�����һ�в���
				if (j != (MAP_SIZE - 1))
				{
					p2.x = BACK + (j + 1) * STEP_SIZE;
					p2.z = LEFT + i * STEP_SIZE;
					p2.y = getHeight(HighMap, j + 1, i);
				}

				//����ͷ�����һ�в���
				if (i != 0 && j != (MAP_SIZE - 1))
				{
					p3.x = BACK + (j + 1) * STEP_SIZE;
					p3.z = LEFT + (i - 1) * STEP_SIZE;
					p3.y = getHeight(HighMap, j + 1, i - 1);
				}


				//����ͷ�в���
				if (i != 0)
				{
					p4.x = BACK + j * STEP_SIZE;
					p4.z = LEFT + (i - 1) * STEP_SIZE;
					p4.y = getHeight(HighMap, j, i - 1);
				}

				//�ǵ�һ�в���
				if (j != 0)
				{
					p5.x = BACK + (j - 1) * STEP_SIZE;
					p5.z = LEFT + i * STEP_SIZE;
					p5.y = getHeight(HighMap, j - 1, i);
				}


				//�ǵ�һ�С����һ�� ����
				if (i != (MAP_SIZE - 1) && j != 0)
				{
					p6.x = BACK + (j - 1) * STEP_SIZE;
					p6.z = LEFT + (i + 1) * STEP_SIZE;
					p6.y = getHeight(HighMap, j - 1, i + 1);
				}


				//�����һ�в���
				if (i != (MAP_SIZE - 1))
				{
					p7.x = BACK + j * STEP_SIZE;
					p7.z = LEFT + (i + 1) * STEP_SIZE;
					p7.y = getHeight(HighMap, j, i + 1);
				}

				nor1 = nor2 = nor3 = nor4 = nor5 = nor6 = glm::vec3(0, 0, 0);
				//������㷨����
				if (i == 0)
				{
					if (j == 0)
					{
						getNormal(p1, p2, p7, nor6);
						normals.push_back(nor6);
					}
					else if (j == (MAP_SIZE - 1))
					{
						getNormal(p1, p5, p6, nor4);
						getNormal(p1, p6, p7, nor5);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
					else
					{
						getNormal(p1, p2, p7, nor6);
						getNormal(p1, p5, p6, nor4);
						getNormal(p1, p6, p7, nor5);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
				}
				else if (i == (MAP_SIZE - 1))
				{
					if (j == 0)
					{
						getNormal(p1, p2, p3, nor1);
						getNormal(p1, p3, p4, nor2);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
					else if (j == (MAP_SIZE - 1))
					{
						getNormal(p1, p4, p5, nor3);
						normals.push_back(nor3);
					}
					else
					{
						getNormal(p1, p2, p3, nor1);
						getNormal(p1, p3, p4, nor2);
						getNormal(p1, p4, p5, nor3);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
				}
				else
				{
					if (j == 0)
					{
						getNormal(p1, p2, p3, nor1);
						getNormal(p1, p3, p4, nor2);
						getNormal(p1, p2, p7, nor6);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
					else if (j == (MAP_SIZE - 1))
					{
						getNormal(p1, p4, p5, nor3);
						getNormal(p1, p5, p6, nor4);
						getNormal(p1, p6, p7, nor5);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
					else
					{
						getNormal(p1, p2, p3, nor1);
						getNormal(p1, p3, p4, nor2);
						getNormal(p1, p4, p5, nor3);
						getNormal(p1, p5, p6, nor4);
						getNormal(p1, p6, p7, nor5);
						getNormal(p1, p2, p7, nor6);
						normalize(nor1, nor2, nor3, nor4, nor5, nor6, nor_res);
						normals.push_back(nor_res);
					}
				}


			}
		}



	}


	float* mergeVN()
	{
		int i, j, tem;
		int maxsize = vertics.size();
		float* res = new float[5 * maxsize];
		for (i = 0; i < maxsize; i++)
		{
			tem = 5 * i;
			res[tem] = vertics[i].x;
			res[tem + 1] = vertics[i].y;
			res[tem + 2] = vertics[i].z;
			res[tem + 3] = texCoords[i].x;
			res[tem + 4] = texCoords[i].y;
		}
		return res;
	}

	//�����ͼ����
	float getAnyPlaceHeight(float HighMap[], glm::vec3 in_po)
	{
		if (in_po.x <BACK || in_po.x>BACK + MAP_SIZE * STEP_SIZE || in_po.z<LEFT || in_po.z>LEFT + MAP_SIZE * STEP_SIZE)return -1;
		float in_x = in_po.x;
		float in_z = in_po.z;

		int cloest_x = floor((in_x - BACK) / STEP_SIZE);
		int cloest_z = floor((in_z - LEFT) / STEP_SIZE);

		float clo_x_coor = cloest_x * STEP_SIZE + BACK;
		float clo_z_coor = cloest_z * STEP_SIZE + LEFT;

		glm::vec2 zs(clo_x_coor, clo_z_coor);
		glm::vec2 ys(clo_x_coor + STEP_SIZE, clo_z_coor);
		glm::vec2 zx(clo_x_coor, clo_z_coor + STEP_SIZE);
		glm::vec2 yx(clo_x_coor + STEP_SIZE, clo_z_coor + STEP_SIZE);

		float del_x = in_po.x - zs.x;
		float del_z = zx.y - in_po.z;
		float alpha, beta, gama, fa, fb, fc;
		fa = getHeight(HighMap, cloest_x, cloest_z + 1);
		fc = getHeight(HighMap, cloest_x + 1, cloest_z);
		if (del_x < del_z)//���������������ֵ�õ������߶�
		{
			alpha = (-(in_po.x - zs.x) * (ys.y - zs.y) + (in_po.z - zs.y) * (ys.x - zs.x)) / (-(zx.x - zs.x) * (ys.y - zs.y) + (zx.y - zs.y) * (ys.x - zs.x));
			beta = (-(in_po.x - ys.x) * (zx.y - ys.y) + (in_po.z - ys.y) * (zx.x - ys.x)) / (-(zs.x - ys.x) * (zx.y - ys.y) + (zs.y - ys.y) * (zx.x - ys.x));
			gama = 1 - alpha - beta;
			fb = getHeight(HighMap, cloest_x, cloest_z);
			return (alpha * fa + beta * fb + gama * fc);
		}
		else
		{
			alpha = (-(in_po.x - yx.x) * (ys.y - yx.y) + (in_po.z - yx.y) * (ys.x - yx.x)) / (-(zx.x - yx.x) * (ys.y - yx.y) + (zx.y - yx.y) * (ys.x - yx.x));
			beta = (-(in_po.x - ys.x) * (zx.y - ys.y) + (in_po.z - ys.y) * (zx.x - ys.x)) / (-(yx.x - ys.x) * (zx.y - ys.y) + (yx.y - ys.y) * (zx.x - ys.x));
			gama = 1 - alpha - beta;
			fb = getHeight(HighMap, cloest_x + 1, cloest_z + 1);
			return (alpha * fa + beta * fb + gama * fc);
		}
		return -1;

	}


	bool Intersects(float HighMap[], Ray ray, glm::vec3& collisionPoint)
	{
		float blockScale = 0.5;
		int count_step = 0;
		float inc_y = ray.Direction.y;
		glm::vec3 rayStep = ray.Direction * blockScale;
		glm::vec3 rayStartPosition = ray.Position;

		glm::vec3 lastRayPosition = ray.Position;
		ray.Position += rayStep;
		float height = getAnyPlaceHeight(HighMap, ray.Position);


		while ((ray.Position.y > height&& height >= 0) || ((height == -1) && ((inc_y > 0 && ray.Position.y <= HIGH_MAX) || (ray.Position.y >= 0 && inc_y < 0))))
		{
			lastRayPosition = ray.Position;
			ray.Position += rayStep;
			height = getAnyPlaceHeight(HighMap, ray.Position);
		}

		if (height >= 0)
		{
			glm::vec3 startPosition = lastRayPosition;
			glm::vec3 endPosition = ray.Position;

			// Binary search. Find the exact collision point 
			for (int i = 0; i < 32; i++)
			{
				// Binary search pass 
				glm::vec3 middlePoint = (startPosition + endPosition) * 0.5f;
				if (middlePoint.y < height)
					endPosition = middlePoint;
				else
					startPosition = middlePoint;
			}
			collisionPoint = (startPosition + endPosition) * 0.5f;
			return true;
		}
		return false;
	}


	int MAP_SIZE;
	float STEP_SIZE, HIGH_MAX;
	float BACK, LEFT;
	std::vector<int> vertics_index;
	std::vector < glm::vec3 > vertics;//������Ϣ
	std::vector < glm::vec3 > normals;//������
	std::vector < glm::vec2 > texCoords;//��ͼ
};


