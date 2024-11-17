#pragma once
enum UpdateType {
    UpdateAll,
    UpdateTime,
    UpdateStatus,
};
wchar_t* MsToTime(int ms);
void UpdateUI(UpdateType type);