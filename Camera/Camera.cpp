#include "Camera.h"
#include "../Graphics.h"
#include "../imgui/imgui.h"

void Camera::SetLookAt(const DirectX::XMFLOAT4& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	
	DirectX::XMFLOAT3 e{ eye.x,eye.y,eye.z };
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&e);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View); 
	


	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;
	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;
	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;
	this->eye = eye;
	
	this->focus = focus;

}

void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{

	DirectX::XMMATRIX Projection =  DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);

	DirectX::XMStoreFloat4x4(&projection, Projection);

}

void Camera::MixViewProjection(ShaderContext::scene_constants &data, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
	
	//	   // ビュー行列とプロジェクション行列を掛け合わせる(乗算)
	XMStoreFloat4x4(&data.view_projection, view * projection);

}

void Camera::CalculateFrustum(Plane* ret, DirectX::XMFLOAT3* nearP, DirectX::XMFLOAT3* farP, float expandDistance)
{
	Camera& camera = Camera::Instance();
	//cameraPos = camera.GetEye();
	Graphics& graphics = Graphics::Instance();
	// ビュープロジェクション行列を取得する
	DirectX::XMMATRIX matrix = {};
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&camera.GetView());
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(&camera.GetProjection());
	matrix = viewMat * projMat;

	//ビュープロジェクション行列の逆行列
	DirectX::XMMATRIX inv_matrix = DirectX::XMMatrixInverse(nullptr, matrix);

	//ビュープロジェクション内の頂点算出用位置ベクトル
	DirectX::XMVECTOR verts[8] =
	{
		// near plane corners
		{ -1, -1, 0 },	// [0]:左下
		{  1, -1, 0 },	// [1]:右下
		{  1,  1, 0 },	// [2]:右上
		{ -1,  1 ,0 },	// [3]:左上

		// far plane corners.
		{ -1, -1, 1 },	// [4]:左下
		{  1, -1, 1 },	// [5]:右下
		{  1,  1, 1 },	// [6]:右上
		{ -1,  1, 1 } 	// [7]:左上
	};

	//①ビュープロジェクション行列の逆行列を用いて、各頂点を算出する
	for (int i = 0; i < 8; ++i) {
		verts[i] = DirectX::XMVector3TransformCoord(verts[i], inv_matrix);
	}
	for (int i = 0; i < 4; ++i) {
		DirectX::XMStoreFloat3(&nearP[i], verts[i]);
		DirectX::XMStoreFloat3(&farP[i], verts[i + 4]);
	}

	DirectX::XMFLOAT4X4 matrix4X4 = {};
	DirectX::XMStoreFloat4x4(&matrix4X4, matrix);

	//②視錐台（フラスタム）を構成する６平面を算出する
	// 0:左側面, 1:右側面, 2:下側面, 3:上側面, 4:奥側面, 5:手前側面
	// 全ての面の法線は内側を向くように設定すること

	// 左側面
	DirectX::XMVECTOR leftNorm = DirectX::XMVectorSet(matrix4X4._14 + matrix4X4._11, matrix4X4._24 + matrix4X4._21, matrix4X4._34 + matrix4X4._31, 0);
	DirectX::XMVECTOR leftPlane = DirectX::XMPlaneNormalize(leftNorm);
	frustum[0].n.x = DirectX::XMVectorGetX(leftPlane);
	frustum[0].n.y = DirectX::XMVectorGetY(leftPlane);
	frustum[0].n.z = DirectX::XMVectorGetZ(leftPlane);
	frustum[0].d = -DirectX::XMVectorGetW(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&NearP[0]), leftPlane)) + expandDistance ;

	// 右側面
	DirectX::XMVECTOR rightNorm = DirectX::XMVectorSet(matrix4X4._14 - matrix4X4._11, matrix4X4._24 - matrix4X4._21, matrix4X4._34 - matrix4X4._31, 0);
	DirectX::XMVECTOR rightPlane = DirectX::XMPlaneNormalize(rightNorm);
	frustum[1].n.x = DirectX::XMVectorGetX(rightPlane) ;
	frustum[1].n.y = DirectX::XMVectorGetY(rightPlane) ;
	frustum[1].n.z = DirectX::XMVectorGetZ(rightPlane) ;
	frustum[1].d = -DirectX::XMVectorGetW(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&NearP[1]), rightPlane)) + expandDistance ;

	// 下側面
	DirectX::XMVECTOR bottomNorm = DirectX::XMVectorSet(matrix4X4._14 + matrix4X4._12, matrix4X4._24 + matrix4X4._22, matrix4X4._34 + matrix4X4._32, 0);
	DirectX::XMVECTOR bottomPlane = DirectX::XMPlaneNormalize(bottomNorm);
	frustum[2].n.x = DirectX::XMVectorGetX(bottomPlane);
	frustum[2].n.y = DirectX::XMVectorGetY(bottomPlane);
	frustum[2].n.z = DirectX::XMVectorGetZ(bottomPlane);
	frustum[2].d = -DirectX::XMVectorGetW(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&FarP[0]), bottomPlane)) + expandDistance*1.8f;
	//-300
	// 上側面
	DirectX::XMVECTOR topNorm = DirectX::XMVectorSet(matrix4X4._14 - matrix4X4._12, matrix4X4._24 - matrix4X4._22, matrix4X4._34 - matrix4X4._32, 0);
	DirectX::XMVECTOR topPlane = DirectX::XMPlaneNormalize(topNorm);
	frustum[3].n.x = DirectX::XMVectorGetX(topPlane) ;
	frustum[3].n.y = DirectX::XMVectorGetY(topPlane) ;
	frustum[3].n.z = DirectX::XMVectorGetZ(topPlane) ;
	frustum[3].d = -DirectX::XMVectorGetW(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&FarP[2]), topPlane)) + expandDistance;
	//-1500
	// 奥側面
	DirectX::XMVECTOR backNorm = DirectX::XMVectorSet(-matrix4X4._14 - matrix4X4._13, -matrix4X4._24 - matrix4X4._23, -matrix4X4._34 - matrix4X4._33, 0);
	DirectX::XMVECTOR backPlane = DirectX::XMPlaneNormalize(backNorm);
	frustum[4].n.x = DirectX::XMVectorGetX(backPlane) ;
	frustum[4].n.y = DirectX::XMVectorGetY(backPlane) ;
	frustum[4].n.z = DirectX::XMVectorGetZ(backPlane) ;
	frustum[4].d = -DirectX::XMVectorGetW(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&FarP[0]), backPlane)) + expandDistance;

	// 手前側面
	DirectX::XMVECTOR frontNorm = DirectX::XMVectorSet(-matrix4X4._14 + matrix4X4._13, -matrix4X4._24 + matrix4X4._23, -matrix4X4._34 + matrix4X4._33, 0);
	DirectX::XMVECTOR frontPlane = DirectX::XMPlaneNormalize(frontNorm);
	frustum[5].n.x = DirectX::XMVectorGetX(frontPlane) ;
	frustum[5].n.y = DirectX::XMVectorGetY(frontPlane) ;
	frustum[5].n.z = DirectX::XMVectorGetZ(frontPlane) ;
	frustum[5].d = -DirectX::XMVectorGetW(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&NearP[0]), frontPlane)) + expandDistance;



	//③各境界線outLineNormを面の法線の外積から求めて正規化する
	// 左下境界線
	DirectX::XMVECTOR leftBottomNorm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&frustum[0].n), DirectX::XMLoadFloat3(&frustum[2].n)));
	DirectX::XMStoreFloat3(&outLineNorm[0], leftBottomNorm);

	// 右下境界線
	DirectX::XMVECTOR rightBottomNorm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&frustum[1].n), DirectX::XMLoadFloat3(&frustum[3].n)));
	DirectX::XMStoreFloat3(&outLineNorm[1], rightBottomNorm);

	// 右上境界線
	DirectX::XMVECTOR rightTopNorm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&frustum[2].n), DirectX::XMLoadFloat3(&frustum[1].n)));
	DirectX::XMStoreFloat3(&outLineNorm[2], rightTopNorm);

	// 左上境界線
	DirectX::XMVECTOR leftTopNorm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&frustum[3].n), DirectX::XMLoadFloat3(&frustum[0].n)));
	DirectX::XMStoreFloat3(&outLineNorm[3], leftTopNorm);
}



bool Camera::MeshFrustumCulling(const skinned_mesh::mesh& mesh)
{
	// ここにメッシュに基づくフラスタムカリングのロジックを追加
	// メッシュが視錐台の外にある場合は true を返す
	bool flag = true;
	

	// Modelの各ノードのworldTransformを適用した境界ボックスを作成
	DirectX::XMFLOAT3 transformedMin{}, transformedMax{};
	DirectX::XMFLOAT3 transformedMin1{}, transformedMax1{};
	
    // ノードのworldTransformを取得
	DirectX::XMFLOAT4X4 worldTransform;
	worldTransform = mesh.default_global_transform;
	DirectX::XMVECTOR minVector = DirectX::XMVectorSet(mesh.bounding_box[0].x, mesh.bounding_box[0].y, mesh.bounding_box[0].z, 1.0f);
	DirectX::XMVECTOR maxVector = DirectX::XMVectorSet(mesh.bounding_box[1].x, mesh.bounding_box[1].y, mesh.bounding_box[1].z, 1.0f);
	DirectX::XMVECTOR transformedMinVector = DirectX::XMVector3TransformCoord(minVector, DirectX::XMLoadFloat4x4(&worldTransform));
	DirectX::XMVECTOR transformedMaxVector = DirectX::XMVector3TransformCoord(maxVector, DirectX::XMLoadFloat4x4(&worldTransform));
	DirectX::XMStoreFloat3(&transformedMin, transformedMinVector);
	DirectX::XMStoreFloat3(&transformedMax, transformedMaxVector);
	DirectX::XMFLOAT3 minmin(FLT_MAX, FLT_MAX, FLT_MAX), maxmax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	{
		minmin.x = min(transformedMin.x, minmin.x);
		minmin.y = min(transformedMin.y, minmin.y);
		minmin.z = min(transformedMin.z, minmin.z);
		minmin.x = min(transformedMax.x, minmin.x);
		minmin.y = min(transformedMax.y, minmin.y);
		minmin.z = min(transformedMax.z, minmin.z);

		maxmax.x = max(transformedMin.x, maxmax.x);
		maxmax.y = max(transformedMin.y, maxmax.y);
		maxmax.z = max(transformedMin.z, maxmax.z);
		maxmax.x = max(transformedMax.x, maxmax.x);
		maxmax.y = max(transformedMax.y, maxmax.y);
		maxmax.z = max(transformedMax.z, maxmax.z);

	}


	for (int i = 0; i < 6; i++)
	{

		// ④各平面の法線の成分を用いてAABBの８頂点の中から最近点と最遠点を求める
		DirectX::XMFLOAT3 NegaPos =/* mesh.bounding_box[0];*/minmin;	// 最近点
			DirectX::XMFLOAT3 PosiPos = /*mesh.bounding_box[1];*/ maxmax;
		// 境界ボックスのサイズを計算する
		DirectX::XMFLOAT3 size = {
			maxmax.x - minmin.x,
			maxmax.y - minmin.y,
			maxmax.z - minmin.z
		};
		// 境界ボックスの半径を求める
		DirectX::XMFLOAT3 radius = {
			size.x / 2,
			size.y / 2,
			size.z / 2
		};

		NegaPos.x -= (radius.x * frustum[i].n.x >= 0) ? radius.x : -radius.x;
		NegaPos.y -= (radius.y * frustum[i].n.y >= 0) ? radius.y : -radius.y;
		NegaPos.z -= (radius.z * frustum[i].n.z >= 0) ? radius.z : -radius.z;

		PosiPos.x += (radius.x * frustum[i].n.x >= 0) ? radius.x : -radius.x;
		PosiPos.y += (radius.y * frustum[i].n.y >= 0) ? radius.y : -radius.y;
		PosiPos.z += (radius.z * frustum[i].n.z >= 0) ? radius.z : -radius.z;

		DirectX::XMVECTOR FN = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&frustum[i].n));
		DirectX::XMVECTOR FP = DirectX::XMVectorScale(FN, -frustum[i].d);

		float PosiDist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&PosiPos), FP), FN));
		float NegaDist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&NegaPos), FP), FN));
		if (PosiDist < 0 && NegaDist < 0)
		{
			// 外部にある場合、確定させて次の平面のチェックに移る
			//collisionState = 2;

			flag = false;

		return flag;
			break;
		}

		if (PosiDist > 0 && NegaDist > 0)
		{
			//collisionState = 1;
			flag = true;

		}
		else
		{
			//collisionState = 0;
		}

	}
	return flag;

}

bool Camera::FrustumCulling(const std::unique_ptr<skinned_mesh>& aabb)
{
	// いったん内部にあるとしておく
	//collisionState = 0;
	bool flag = true;

	for (const skinned_mesh::mesh& mesh : aabb->meshes) {
		DirectX::XMFLOAT3 transformedMin{}, transformedMax{};
		DirectX::XMFLOAT3 transformedMin1{}, transformedMax1{};
		const std::vector<skinned_mesh::skeleton::bone>& node = mesh.bind_pose.bones;
		//	// ノードのworldTransformを取得
		DirectX::XMFLOAT4X4 worldTransform;
		if (mesh.node_index >= 0 && mesh.node_index < node.size())
		{
			DirectX::XMStoreFloat4x4(&worldTransform, DirectX::XMLoadFloat4x4(&node[mesh.node_index].offset_transform));
		}
		else
		{
			// 範囲外の場合、デフォルトの行列を使用
			DirectX::XMStoreFloat4x4(&worldTransform, DirectX::XMMatrixIdentity());
		}
		

		// ノードのworldTransformを適用した境界ボックスの計算
		DirectX::XMVECTOR minVector = DirectX::XMVectorSet(mesh.bounding_box[0].x, mesh.bounding_box[0].y, mesh.bounding_box[0].z, 1.0f);
		DirectX::XMVECTOR maxVector = DirectX::XMVectorSet(mesh.bounding_box[1].x, mesh.bounding_box[1].y, mesh.bounding_box[1].z, 1.0f);
		DirectX::XMStoreFloat3(&transformedMin1, minVector);
		DirectX::XMStoreFloat3(&transformedMax1, maxVector);
		DirectX::XMVECTOR transformedMinVector = DirectX::XMVector3TransformCoord(minVector, DirectX::XMLoadFloat4x4(&worldTransform));
		DirectX::XMVECTOR transformedMaxVector = DirectX::XMVector3TransformCoord(maxVector, DirectX::XMLoadFloat4x4(&worldTransform));
		DirectX::XMStoreFloat3(&transformedMin, transformedMinVector);
		DirectX::XMStoreFloat3(&transformedMax, transformedMaxVector);
		DirectX::XMFLOAT3 minmin(FLT_MAX, FLT_MAX, FLT_MAX), maxmax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		{
			minmin.x = min(transformedMin.x, minmin.x);
			minmin.y = min(transformedMin.y, minmin.y);
			minmin.z = min(transformedMin.z, minmin.z);
			minmin.x = min(transformedMax.x, minmin.x);
			minmin.y = min(transformedMax.y, minmin.y);
			minmin.z = min(transformedMax.z, minmin.z);

			maxmax.x = max(transformedMin.x, maxmax.x);
			maxmax.y = max(transformedMin.y, maxmax.y);
			maxmax.z = max(transformedMin.z, maxmax.z);
			maxmax.x = max(transformedMax.x, maxmax.x);
			maxmax.y = max(transformedMax.y, maxmax.y);
			maxmax.z = max(transformedMax.z, maxmax.z);

		}

		for (int i = 0; i < 6; i++)
		{

			// ④各平面の法線の成分を用いてAABBの８頂点の中から最近点と最遠点を求める
			DirectX::XMFLOAT3 NegaPos = minmin;	// 最近点
			DirectX::XMFLOAT3 PosiPos = maxmax;
			// 境界ボックスのサイズを計算する
			DirectX::XMFLOAT3 size = {
				maxmax.x - minmin.x,
				maxmax.y - minmin.y,
				maxmax.z - minmin.z
			};
			// 境界ボックスの半径を求める
			DirectX::XMFLOAT3 radius = {
				size.x / 2,
				size.y / 2,
				size.z / 2
			};

			NegaPos.x -= (radius.x * frustum[i].n.x >= 0) ? radius.x : -radius.x;
			NegaPos.y -= (radius.y * frustum[i].n.y >= 0) ? radius.y : -radius.y;
			NegaPos.z -= (radius.z * frustum[i].n.z >= 0) ? radius.z : -radius.z;

			PosiPos.x += (radius.x * frustum[i].n.x >= 0) ? radius.x : -radius.x;
			PosiPos.y += (radius.y * frustum[i].n.y >= 0) ? radius.y : -radius.y;
			PosiPos.z += (radius.z * frustum[i].n.z >= 0) ? radius.z : -radius.z;

			DirectX::XMVECTOR FN = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&frustum[i].n));
			DirectX::XMVECTOR FP = DirectX::XMVectorScale(FN, -frustum[i].d);

			float PosiDist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&PosiPos), FP), FN));
			float NegaDist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&NegaPos), FP), FN));
			if (PosiDist < 0 && NegaDist < 0)
			{
				// 外部にある場合、確定させて次の平面のチェックに移る
				//collisionState = 2;

				flag = false;

				return flag;
				break;
			}

			if (PosiDist > 0 || NegaDist > 0)
			{
				//collisionState = 1;
				flag = true;

			}
			else
			{
				//collisionState = 0;
			}

		}
	}
	return flag;
}

void Camera::DebugImGui()
{

	ImGui::SetNextWindowPos(ImVec2(20, 10), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiSetCond_Once);


	ImGui::Begin("Camera");
	ImGui::SliderFloat("1camera_position.x", &eye.x, -100.0f, +100.0f);
	ImGui::SliderFloat("1camera_position.x", &eye.y, -100.0f, +100.0f);
	ImGui::SliderFloat("1camera_position.x", &eye.z, -100.0f, +100.0f);
	ImGui::Separator();

	ImGui::End();



}
