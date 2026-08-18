#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CH3GameMode.generated.h"

UCLASS()
class ACH3GameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    // CH3GameMode가 생성될 때 실행되는 생성자
    ACH3GameMode();
};