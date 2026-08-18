#include "CH3Pawn.h"
// CameraComponent를 사용하기 위한 헤더
#include "Camera/CameraComponent.h"
// CapsuleComponent를 사용하기 위한 헤더
#include "Components/CapsuleComponent.h"
// SkeletalMeshComponent를 사용하기 위한 헤더
#include "Components/SkeletalMeshComponent.h"
// SpringArmComponent를 사용하기 위한 헤더
#include "GameFramework/SpringArmComponent.h"
// PlayerController를 사용하기 위한 헤더
#include "GameFramework/PlayerController.h"
// Enhanced Input Component를 사용하기 위한 헤더
#include "EnhancedInputComponent.h"
// Enhanced Input Local Player Subsystem을 사용하기 위한 헤더
#include "EnhancedInputSubsystems.h"
// Input Action을 사용하기 위한 헤더
#include "InputAction.h"
// Input Mapping Context를 사용하기 위한 헤더
#include "InputMappingContext.h"

// ================================
// Constructor
// ================================

ACH3Pawn::ACH3Pawn()
{
    // Tick()을 매 프레임 실행할 수 있도록 활성화함.
    PrimaryActorTick.bCanEverTick = true;


    // ================================
    // Capsule Component
    // ================================

    // CapsuleComponent 생성
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    // CapsuleComponent를 Pawn의 RootComponent로 설정함.
    RootComponent = CapsuleComp;
    // Capsule의 반지름과 높이를 설정함.
    CapsuleComp->InitCapsuleSize(42.0f,96.0f);
    // Simulate Physics를 false로 설정함.
    CapsuleComp->SetSimulatePhysics(false);

    // ================================
    // Skeletal Mesh Component
    // ================================

    // 캐릭터 외형을 보여줄 SkeletalMeshComponent 생성
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    // Mesh를 Capsule에 부착함.
    MeshComp->SetupAttachment(CapsuleComp);
    // Mesh 역시 물리 시뮬레이션을 사용하지 않음.
    MeshComp->SetSimulatePhysics(false);


    // ================================
    // Spring Arm Component
    // ================================

    // 3인칭 카메라 위치를 관리할 SpringArm 생성
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    // SpringArm을 Capsule에 부착함.
    SpringArmComp->SetupAttachment(CapsuleComp);
    // Pawn과 Camera 사이의 기본 거리를 300으로 설정함.
    SpringArmComp->TargetArmLength = 300.0f;
    // Controller 회전을 자동으로 사용하지 않도록 설정함.
    SpringArmComp->bUsePawnControlRotation = false;


    // ================================
    // Camera Component
    // ================================

    // 실제 플레이 화면을 보여줄 Camera 생성
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    // Camera를 SpringArm 끝에 부착함.
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    // Camera 회전을 자동으로 사용하지 않음.
    CameraComp->bUsePawnControlRotation = false;

    // ================================
    // Enhanced Input 초기값
    // ================================

    // 기본값
    InputMappingContext = nullptr;
    MoveAction = nullptr;
    LookAction = nullptr;

    // ================================
    // Movement 초기값
    // ================================

    // Pawn 이동 속도
    MoveSpeed = 600.0f;
    // Pawn 좌우 회전 속도
    YawSpeed = 100.0f;
    // Camera 상하 회전 속도
    PitchSpeed = 100.0f;

    MoveInput = FVector2D::ZeroVector;
    LookInput = FVector2D::ZeroVector;
    // Camera의 초기 Pitch 값
    CameraPitch = 0.0f;
}

// ================================
// BeginPlay
// ================================

void ACH3Pawn::BeginPlay()
{
    Super::BeginPlay();
    // PlayerController를 가져옴.
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    // PlayerController를 찾지 못했다면 아래 코드를 실행하지 않고 종료함.
    if (!PlayerController)
    {
        return;
    }
    // PlayerController가 사용하고 있는 LocalPlayer를 가져옴.
    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer)
    {
        return;
    }
    // Enhanced Input Subsystem을 가져옴.
    UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!Subsystem)
    {
        return;
    }
    // Blueprint에서 InputMappingContext가 정상적으로 지정되어 있다면 활성화함.
    if (InputMappingContext)
    {
        Subsystem->AddMappingContext(InputMappingContext, 0);
    }
}

// ================================
// SetupPlayerInputComponent
// ================================

void ACH3Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // UEnhancedInputComponent로 변환함.
    UEnhancedInputComponent* EnhancedInput =Cast<UEnhancedInputComponent>( PlayerInputComponent);
    // Enhanced Input Component가 아니라면 입력 바인딩을 하지 않고 종료함.
    if (!EnhancedInput)
    {
        return;
    }

    // ================================
    // IA_Move
    // ================================

    if (MoveAction)
    {
        // WASD 입력이 들어오는 동안 Move() 함수를 실행함.
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACH3Pawn::Move);
        // WASD 입력이 끝났을 때 StopMove() 함수를 실행함.
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACH3Pawn::StopMove);
        // 입력이 중간에 취소된 경우에도 MoveInput을 초기화함.
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ACH3Pawn::StopMove);
    }

    // ================================
    // IA_Look
    // ================================

    if (LookAction)
    {
        // 마우스 입력이 들어오면 Look() 함수를 실행함.
        EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACH3Pawn::Look);
    }
}

// ================================
// Move
// ================================

void ACH3Pawn::Move(const FInputActionValue& Value)
{
    // IA_Move에서 전달된 Vector2D 값을 가져옴.
    // X = 전진 / 후진
    // Y = 좌 / 우
    MoveInput = Value.Get<FVector2D>();
}

// ================================
// StopMove
// ================================

void ACH3Pawn::StopMove(const FInputActionValue& Value)
{
    // 키에서 손을 떼면 저장되어 있던 이동 입력값을 0으로 만듦. 이 처리를 하지 않으면 마지막 입력값이 남아 Pawn이 계속 움직일 수 있음.
    MoveInput = FVector2D::ZeroVector;
}

// ================================
// Look
// ================================

void ACH3Pawn::Look(const FInputActionValue& Value)
{
    // IA_Look에서 전달된 Vector2D 값을 가져옴.
    // X = Mouse X
    // Y = Mouse Y
    LookInput = Value.Get<FVector2D>();
}

// ================================
// Tick
// ================================

void ACH3Pawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ================================
    // WASD 이동
    // ================================

    // MoveInput이 (0, 0)이 아닌 경우에만 실제 이동을 계산함.
    if (!MoveInput.IsNearlyZero())
    {
        // Actor Local 좌표에서 X축 = Forward Y축 = Right Z축 = Up 이므로 IA_Move 값을 그대로 Local 이동 방향으로 사용할 수 있음.
        FVector LocalOffset(MoveInput.X, MoveInput.Y, 0.0f);
        // W + D 같은 대각선 입력을 했을 때 이동 속도가 더 빨라지는 것을 방지함.
        LocalOffset = LocalOffset.GetClampedToMaxSize(1.0f);
        // 실제 한 프레임의 이동 거리를 계산함.
        LocalOffset *= MoveSpeed * DeltaTime;
        // Pawn의 Local 좌표를 기준으로 위치를 직접 변경함. true는 Sweep을 사용한다는 의미로 이동하면서 충돌을 검사함.
        AddActorLocalOffset(LocalOffset, true);
    }

    // ================================
    // Mouse X → Yaw
    // ================================

    // 마우스 X 입력이 있을 때만 실행함.
    if (!FMath::IsNearlyZero(LookInput.X))
    {
        // 프레임에 회전할 Yaw 값을 직접 계산함.
        float YawAmount = LookInput.X * YawSpeed * DeltaTime;
        // Pitch = 0
        // Yaw = 계산한 좌우 회전값
        // Roll = 0
        FRotator YawRotation(0.0f, YawAmount, 0.0f);
        // AddControllerYawInput()을 사용하지 않고 Pawn 자체의 Rotation을 직접 변경함.
        AddActorLocalRotation(YawRotation);
    }

    // ================================
    // Mouse Y → Pitch
    // ================================

    // 마우스 Y 입력이 있을 때만 실행함.
    if (!FMath::IsNearlyZero(LookInput.Y))
    {
        // 이번 프레임에 변경할 Pitch 값을 계산함.
        float PitchAmount = LookInput.Y * PitchSpeed * DeltaTime;
        // 기존 CameraPitch 값에 새 Pitch 값을 계속 누적함.
        CameraPitch += PitchAmount;

        // 카메라가 계속 회전해서 뒤집히는 것을 방지함.
        CameraPitch = FMath::Clamp( CameraPitch, -80.0f, 80.0f);
        // AddControllerPitchInput()을 사용하지 않고 SpringArm의 Rotation을 직접 변경함.
        // Pawn 몸 전체는 기울이지 않고 카메라 시점만 위아래로 움직이게 됨.
        SpringArmComp->SetRelativeRotation(
            FRotator(CameraPitch, 0.0f, 0.0f));
    }
    // 마우스 입력은 해당 프레임의 변화량이므로 한 번 사용한 뒤 다시 (0, 0)으로 초기화함.
    LookInput = FVector2D::ZeroVector;
}
