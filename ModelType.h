#pragma once
// ���f���̎��
enum class ModelType {
	StageMain,
	StageMoveFloor,
	// ���̃��f���^�C�v��ǉ�����ꍇ�͂����ɗ񋓂���
};

// Switch���f���̎��
enum class ModelSwitchType {
	TrapOn,
	TrapOff,
	// ���̃��f���^�C�v��ǉ�����ꍇ�͂����ɗ񋓂���
};
enum class ModelSwitch {
	Bittkuri,
	Yukaswitch,
	Max
};

// Switch�̐�
enum class SwitchCountType {
	SwitchMain,
	Switch1,
	Switch2,
	Switch3,
	Max
	// ���̃��f���^�C�v��ǉ�����ꍇ�͂����ɗ񋓂���
};