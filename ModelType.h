#pragma once
// モデルの種類
enum class ModelType {
	StageMain,
	StageMoveFloor,
	// 他のモデルタイプを追加する場合はここに列挙する
};

// Switchモデルの種類
enum class ModelSwitchType {
	TrapOn,
	TrapOff,
	// 他のモデルタイプを追加する場合はここに列挙する
};
enum class ModelSwitch {
	Bittkuri,
	Yukaswitch,
	Max
};

// Switchの数
enum class SwitchCountType {
	SwitchMain,
	Switch1,
	Switch2,
	Switch3,
	Max
	// 他のモデルタイプを追加する場合はここに列挙する
};