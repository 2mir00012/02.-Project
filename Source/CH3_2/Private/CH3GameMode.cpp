#include "CH3GameMode.h"
// CH3Pawn 클래스를 사용하기 위한 헤더
#include "CH3Pawn.h"

// ================================
// Constructor
// ================================

ACH3GameMode::ACH3GameMode()
{
    // 게임 시작 시 자동으로 생성될
    // 기본 Pawn 클래스를 CH3Pawn으로 지정함.
    DefaultPawnClass = ACH3Pawn::StaticClass();
}