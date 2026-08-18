#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "CH3Pawn.generated.h"

// 헤더에서 포인터로 사용할 클래스들을 미리 선언함.
class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ACH3Pawn : public APawn
{
    GENERATED_BODY()

public:
    // CH3Pawn이 생성될 때 실행되는 생성자
    ACH3Pawn();
    // 매 프레임 호출되는 함수 DeltaTime을 이용해 이동과 회전을 직접 계산함.
    virtual void Tick(float DeltaTime) override;
    // Enhanced Input의 Input Action과 실제 C++ 함수를 연결하는 함수
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    // 게임이 시작될 때 한 번 호출됨. Input Mapping Context를 활성화함.
    virtual void BeginPlay() override;

    // ================================
    // Component
    // ================================

    // Pawn의 충돌을 담당하는 Capsule
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCapsuleComponent* CapsuleComp;
    // Pawn의 실제 외형을 보여주는 Skeletal Mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* MeshComp;
    // Pawn과 Camera 사이의 거리를 관리하는 SpringArm
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComp;
    // 플레이어가 실제로 보게 되는 Camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComp;

    // ================================
    // Enhanced Input
    // ================================

    // IA_Move와 IA_Look이 들어 있는 Input Mapping Context
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* InputMappingContext;
    // WASD 이동 입력을 담당하는 Input Action
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MoveAction;
    // 마우스 회전 입력을 담당하는 Input Action
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAction;

    // ================================
    // Movement
    // ================================

    // Pawn의 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Movement")
    float MoveSpeed;
    // 마우스 좌우 회전 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float YawSpeed;
    // 마우스 위아래 회전 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float PitchSpeed;

    // ================================
    // Input Value
    // ================================

    // 현재 WASD 입력값을 저장함.
    FVector2D MoveInput;
    // 현재 마우스 입력값을 저장함.
    FVector2D LookInput;
    // 현재 SpringArm의 Pitch 값을 저장함.
    float CameraPitch;

    // ================================
    // Input Function
    // ================================

    // IA_Move에서 전달된 Vector2D 값을 받아 저장함.
    void Move(const FInputActionValue& Value);
    // WASD 키를 떼었을 때 MoveInput을 0으로 초기화함.
    void StopMove(const FInputActionValue& Value);
    // IA_Look에서 전달된 마우스 값을 받아 저장함.
    void Look(const FInputActionValue& Value);
};
