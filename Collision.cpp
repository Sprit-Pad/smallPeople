#include "Collision.h"


inline DirectX::XMFLOAT3 GetTriangleNormVector(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
	//①三角形trの法線nを求める
	//※３点が時計回りに見える側を表とする
	DirectX::XMFLOAT3 n = {};

	//点０と点１の(p1-p0のベクトル)
	DirectX::XMVECTOR ed = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p0));
	//点2と点１の(p2-p1のベクトル)
	DirectX::XMVECTOR ed1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p2), DirectX::XMLoadFloat3(&p1));
	//上記の２辺の外積の正規化された値がn
	DirectX::XMStoreFloat3(&n, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(ed, ed1)));

	return n;
}

bool Collision::IntersectSphereVsIntersectSphere(const DirectX::XMFLOAT3& spherePosition, float shpereRadius, const DirectX::XMFLOAT3& spherePosition1, float shpereRadius1, DirectX::XMFLOAT3& outCylinderPosition)
{


	DirectX::XMFLOAT2 posA(spherePosition.x, spherePosition.z);
	DirectX::XMFLOAT2 posB(spherePosition1.x, spherePosition1.z);

	// Check vertical separation
	if (spherePosition.y + shpereRadius < spherePosition1.y || spherePosition.y - shpereRadius > spherePosition1.y + shpereRadius1)
		return false;

	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&spherePosition);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&spherePosition1);

	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);

	// Calculate the squared length of the vector
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	float collisionRange = shpereRadius + shpereRadius1;

	// Check if the spheres overlap
	if (lengthSq > collisionRange * collisionRange)
		return false;

	// Check if the spheres are already intersecting vertically
	if (spherePosition.y >= spherePosition1.y && spherePosition.y <= spherePosition1.y + shpereRadius1)
		return true;

	// Calculate the closest point on the second sphere's surface
	float vx = spherePosition1.x - spherePosition.x;
	float vz = spherePosition1.z - spherePosition.z;
	float distXZ = sqrtf(vx * vx + vz * vz);

	// Check if the spheres overlap horizontally
	if (distXZ > collisionRange)
		return false;

	// Normalize and scale the direction vector
	vx /= distXZ;
	vz /= distXZ;

	// Set the intersection point to the point on the second sphere's surface where it intersects with the first sphere
	outCylinderPosition.x = spherePosition.x + (vx * collisionRange);
	outCylinderPosition.y = spherePosition1.y;
	outCylinderPosition.z = spherePosition.z + (vz * collisionRange);

	return true;
}


bool Collision::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA,
	float radiusA, float heightA, const DirectX::XMFLOAT3& positionB, float radiusB,
	float heightB, DirectX::XMFLOAT3& outPositionB)
{

	if (positionA.y>positionB.y+heightB) {



		return false;
	}
	if (positionA.y + heightA< positionB.y ) {


		return false;
	}
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;

	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);

	if (distXZ > range) {





		return false;

	}


	vx /= distXZ;
	vz /= distXZ;

	//上記ベクトルを代入する
	
	outPositionB.x = positionA.x+ (vx * range);
	outPositionB.y = positionB.y;
	outPositionB.z = positionA.z+(vz*range);

	return true;


}

bool Collision::IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition, float sphereRadius, const DirectX::XMFLOAT3& cylinderPosition,
	float cylinderRadius, float cylinderHeight, DirectX::XMFLOAT3& outCylinderPosition)
{

	
	DirectX::XMFLOAT2 posA(spherePosition.x, spherePosition.z);
	DirectX::XMFLOAT2 posB(cylinderPosition.x, cylinderPosition.z);

	if (spherePosition.y + sphereRadius < cylinderPosition.y)return false;

	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderRadius)return false;
	

	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&spherePosition);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&cylinderPosition);

	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);

	//Vecの長さを計算(XMVector3LengthSqを利用してもいい)
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;

	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	float range = sphereRadius + cylinderRadius;

	if (lengthSq > range) {



		return false;

	}

	if (spherePosition.y > cylinderPosition.y + cylinderHeight) {



		return false;
	}
	if (spherePosition.y  < cylinderPosition.y) {


		return false;
	}
	float vx = cylinderPosition.x - spherePosition.x;
	float vz = cylinderPosition.z - spherePosition.z;

	//float range = sphereRadius + cylinderRadius; 
	float distXZ = sqrtf(vx * vx + vz * vz);

	if (distXZ > range) {





		return false;

	}


	vx /= distXZ;
	vz /= distXZ;


	outCylinderPosition.x = spherePosition.x + (vx * range);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (vz * range);


	return true;
}

//bool Collision::IntersectRayVsModel(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const skinned_mesh* model, HitResult& result)
//{
//	//以前の処理が正しく動くように仮の実装
//	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
//	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
//	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
//	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
//
//	//ワールド空間のレイの長さ
//	DirectX::XMStoreFloat(&result.distance, WorldRayLength);
//
//	bool hit = false;
//	const std::vector<skinned_mesh::mesh>& meshes = model->meshes;
//	for (const  skinned_mesh::mesh&mesh : model->meshes)
//	{
//		//メッシュノード取得
//		const scene::node& node = model->get_scene().nodes.at(mesh.node_index);
//		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&model->node_transforms[mesh.node_index]);
//		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
//
//		//レイをワールド空間からローカル空間へ変換
//		//DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
//	//	DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
//
//		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
//		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
//		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
//		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
//		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);
//
//		//レイの長さ
//		float neart;
//		DirectX::XMStoreFloat(&neart, Length);
//
//		//三角形(面)との交差判定
//		const std::vector<skinned_mesh::vertex>& vertices = mesh.vertices;
//		const std::vector<UINT>indices = mesh.indices;
//
//
//		int materialIndex = -1;
//		DirectX::XMVECTOR HitPosition;
//		DirectX::XMVECTOR HitNormal;
//		for (const skinned_mesh::mesh::subset& subset : mesh.subsets)
//		{
//
//			for (UINT i = 0; i < subset.index_count; i += 3)
//			{
//
//				UINT index = subset.start_index_location + i;
//
//				//三角形の頂点を抽出
//				const skinned_mesh::vertex& a = vertices.at(indices.at(index));
//				const skinned_mesh::vertex& b = vertices.at(indices.at(index + 1));
//				const skinned_mesh::vertex& c = vertices.at(indices.at(index + 2));
//
//
//
//				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
//				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
//				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);
//
//				//三角形の三辺ベクトルを算出
//
//				
//				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
//				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
//				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);
//
//				//三角形の法線ベクトルを算出
//				DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);
//
//				//内積の結果がプラスならば裏向き
//				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
//
//				float d;
//				DirectX::XMStoreFloat(&d, Dot);
//				if (d >= 0)continue;
//
//				//レイと平面の交点を算出
//				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
//				DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(N, SA), Dot);
//				float x;
//				DirectX::XMStoreFloat(&x, X);
//				if (x<0.0f || x>neart)continue;
//
//				DirectX::XMVECTOR P = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(V, X), S);
//
//				//交点が三角形の内側にあるか判定
//				//1
//				DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
//				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
//				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
//				float dot1;
//				DirectX::XMStoreFloat(&dot1, Dot1);
//				if (dot1 < 0.0f)continue;
//
//				//2
//				DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
//				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
//				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
//				float dot2;
//				DirectX::XMStoreFloat(&dot2, Dot2);
//				if (dot2 < 0.0f)continue;
//
//				//3
//				DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
//				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
//				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
//				float dot3;
//				DirectX::XMStoreFloat(&dot3, Dot3);
//				if (dot3 < 0.0f)continue;
//
//				//最近距離を更新
//				HitPosition = P;
//				HitNormal = N;
//				materialIndex = subset.material_unique_id;
//
//
//			}
//		}
//
//		if (materialIndex >= 0)
//		{
//			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
//			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
//			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
//			float distance;
//			DirectX::XMStoreFloat(&distance, WorldCrossLength);
//			//ヒット情報
//			if (result.distance > distance)
//			{
//				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
//
//				result.distance = distance;
//				result.materialIndex = materialIndex;
//				DirectX::XMStoreFloat3(&result.position, WorldPosition);
//				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
//				hit = true;
//
//
//			}
//
//		
//			
//
//
//
//	    }
//
//
//
//
//	}
//	
//	return hit;
//	
//}


bool Collision::CollideRayVsTriangle(DirectX::XMFLOAT3* p, const Ray& r, const Triangle& t)
{
	

	//④光線rが三角形tの表側から交差し、かつ、交点pが三角形tの内部にある場合のみ、trueを返す
	DirectX::XMFLOAT3 n = GetTriangleNormVector(t.p0, t.p1, t.p2);

	//光線(レイ)の方向ベクトルr.dと面法線ベクトルnの内積fDotを計算します。
	float fDot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&r.d), DirectX::XMLoadFloat3(&n)));
	// 内積がマイナスかどうか判定（表面ならを続ける）
	if (fDot < 0.0f)
	{

		//レイと平面 𝑑𝑜𝑡((𝑡.𝑝[0]−𝑟_0 ),𝑛)でLを計算
		float L = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&t.p0), DirectX::XMLoadFloat3(&n)));
		// 光線が真横から当たっていないか判定（fabs(fDot) > 1e-6f）
		if (fabs(fDot) > 1e-6f)
		{
			// 光線と三角形の交点までの光線上の距離を計算（スケール値の計算）
			FLOAT l;
			DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p0), DirectX::XMLoadFloat3(&r.p));
			//l=Dot(tmp,n)/fDot
			l = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, DirectX::XMLoadFloat3(&n))) / fDot;
			//平面上にある点pを計算(始点＋方向ベクトル＊スケール)
			 // 距離が０より大きいかどうか判定
			if (l > 0)
			{
				// 交点の座標位置を計算
				//*p = {r.p.x+r.d.x*l,r.p.y+r.d.y*l,r.p.z+r.d.z*l};
				DirectX::XMStoreFloat3(p, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&r.p), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&r.d), l)));
				// 交点から三角形のそれぞれの点までのベクトルを計算
				//DirectX::XMVECTOR tmp0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p0),DirectX::XMLoadFloat3(p)); // t.p0 - p

				DirectX::XMVECTOR tmp0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p0), DirectX::XMLoadFloat3(p)); // t.p0 - p
				DirectX::XMVECTOR tmp1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p1), DirectX::XMLoadFloat3(p)); // t.p1 - p
				DirectX::XMVECTOR tmp2 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p2), DirectX::XMLoadFloat3(p)); // t.p2 - p
		 //点pと三角形の点との外積計算
				// 上記で計算したベクトルのそれぞれの外積を計算
				DirectX::XMVECTOR n0 = DirectX::XMVector3Cross(tmp0, tmp1); // t.p0 - p と t.p1 - p の外積
				DirectX::XMVECTOR n1 = DirectX::XMVector3Cross(tmp1, tmp2); // t.p1 - p と t.p2 - p の外積
				DirectX::XMVECTOR n2 = DirectX::XMVector3Cross(tmp2, tmp0); // t.p2 - p と t.p0 - p の外積

		//それぞれの外積と三角形の法線との内積が0より上なら
				// 計算した外積の値と三角形の内積がすべて同じ方向を向いていたら（内積の値がプラスなら）
				if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(n0, DirectX::XMLoadFloat3(&n))) > 0
					&& DirectX::XMVectorGetX(DirectX::XMVector3Dot(n1, DirectX::XMLoadFloat3(&n))) > 0
					&& DirectX::XMVectorGetX(DirectX::XMVector3Dot(n2, DirectX::XMLoadFloat3(&n))) > 0)
				{
					//三角形が当たっている
					// 光線が三角形に交差している
					return true;
				}

			}

		}
		*p = {};
		return false;
	}
}


bool Collision::IntersectRayVsTriangle(
	const DirectX::XMFLOAT3& rayStart,
	const DirectX::XMFLOAT3& rayDirection,		// 要正規化
	float rayDist,
	const DirectX::XMFLOAT3 triangleVerts[3],
	HitResult& result)
{
	DirectX::XMVECTOR rayStartVec = DirectX::XMLoadFloat3(&rayStart);
	DirectX::XMVECTOR rayDirectionVec = DirectX::XMLoadFloat3(&rayDirection);
	DirectX::XMVECTOR trianglePos[3];
	for (int i = 0; i < 3; i++)
	{
		trianglePos[i] = DirectX::XMLoadFloat3(&triangleVerts[i]);
	}

	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStartVec, DirectX::XMVectorAdd(rayStartVec, DirectX::XMVectorScale(rayDirectionVec, rayDist)));
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

	if (d > 0.0f)	// 表側から交差しているときのみ判定を行う
	{
		if (fabs(d) > 1e-6f)	// 平行確認
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStartVec, trianglePos[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// レイの向きと長さ確認
			{
				DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
				float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
				if (v >= 0.0f && v <= d)
				{
					float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
					if (w > 0.0f && v + w <= d)
					{
						result.distance = rayDist * t / d;
						DirectX::XMStoreFloat3(&result.position, DirectX::XMVectorAdd(rayStartVec, DirectX::XMVectorScale(rayDirectionVec, result.distance)));
						DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(norm));
						for (int i = 0; i < 3; i++)
						{
							result.triangleVerts[i] = triangleVerts[i];
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}