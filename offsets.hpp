namespace O {

	const uint32_t GWorld = 0xa8c7160; //48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 B4 24
	const uint32_t GName = 0xa66ecc0;  //ERROR_NAME_SIZE_EXCEEDED Const Wchar_t

	namespace UWorld {

		const uint32_t OwningGameInstance = 0x1c8;
		const uint32_t GameState = 0x168;
		const uint32_t Level = 0x180;
		const uint32_t LevelSize = Level + 8;
		const uint32_t persistent_level = 0x38;

	}

	namespace ULevel {

		const uint32_t ActorArray = 0xa0;
		const uint32_t ActorCount = ActorArray + 8;

	}

	namespace UGameInstance {

		const uint32_t LocalPlayers = 0x40;

	}

	namespace UPlayer {

		const uint32_t PlayerController = 0x38;

	}

	namespace APlayerController {

		const uint32_t AcknowledgedPawn = 0x350;
		const uint32_t PlayerCameraManager = 0x360;
	}

	namespace APlayerCameraManager {

		const uint32_t POV = 0x2bc;
		const uint32_t DefaultFov = 0x2bc + 4;
		const uint32_t Ratio = 0x2cc + 4;
		const uint32_t CameraCachePrivate = 0x22d0; // Added CameraCachePrivate offset

	}

	namespace FMinimalViewInfo {

		const uint32_t Location = 0x0;
		const uint32_t Rotation = 0x18;
		const uint32_t Fov = 0x30;
		const uint32_t DesiredFOV = 0x34;
		const uint32_t AspectRation = 0x48;

	}

	namespace AActor {

		const uint32_t RootComponent = 0x1a8;

	}

	namespace USceneComponent {

		const uint32_t RelativeScale3D = 0x170;
		const uint32_t RelativeLocation = 0x140;
		const uint32_t ComponentVelocity = 0x188;
		const uint32_t ComponentToWorld = 0x1E0;

	}

	namespace ACharacter {

		const uint32_t Mesh = 0x328;
		const uint32_t CharacterMovement = 0x330;
		const uint32_t CapsuleComponent = 0x338;

	}

	namespace USkeletalMeshComponent {

		const uint32_t LoDData = 0x630;

	}

	namespace APawn {

		const uint32_t PlayerState = 0x2b8;

	}

	namespace APlayerState {

		const uint32_t PlayerNamePrivate = 0x3a0;

	}

	namespace UCharacterMovement {
		constexpr auto CharacterOwner = 0x178; // ACharacter*
		constexpr auto GravityScale = 0x198; // float
		constexpr auto MaxStepHeight = 0x19c; // float
		constexpr auto JumpZVelocity = 0x1a0; // float
		constexpr auto JumpOffJumpZFactor = 0x1a4; // float
		constexpr auto WalkableFloorAngle = 0x1c4; // float
		constexpr auto WalkableFloorZ = 0x1c8; // float
		constexpr auto MovementMode = 0x1cc; // EMovementMode
		constexpr auto CustomMovementMode = 0x1cd; // uint8
		constexpr auto NetworkSmoothingMode = 0x1ce; // ENetworkSmoothingMode
		constexpr auto GroundFriction = 0x1d0; // float
		constexpr auto MaxWalkSpeed = 0x218; // float
		constexpr auto MaxWalkSpeedCrouched = 0x21c; // float
		constexpr auto MaxSwimSpeed = 0x220; // float
		constexpr auto MaxFlySpeed = 0x224; // float
		constexpr auto MaxCustomMovementSpeed = 0x228; // float
		constexpr auto MaxAcceleration = 0x22c; // float
		constexpr auto MinAnalogWalkSpeed = 0x230; // float
		constexpr auto BrakingFrictionFactor = 0x234; // float
		constexpr auto BrakingFriction = 0x238; // float
		constexpr auto BrakingSubStepTime = 0x23c; // float
		constexpr auto BrakingDecelerationWalking = 0x240; // float
		constexpr auto BrakingDecelerationFalling = 0x244; // float
		constexpr auto BrakingDecelerationSwimming = 0x248; // float
		constexpr auto BrakingDecelerationFlying = 0x24c; // float
		constexpr auto AirControl = 0x250; // float
		constexpr auto AirControlBoostMultiplier = 0x254; // float
		constexpr auto AirControlBoostVelocityThreshold = 0x258; // float
	}

	namespace APallet {

		const uint32_t PalletState = 0x3e0; //bruteforce by value 0-4

	}

	namespace AGenerator {

		const uint32_t Percent = 960; //brutforce by value 0.85 in training

	}

	namespace UCapsuleComponent {

		const uint32_t CapsuleHalfHeight = 0x5b0;
		const uint32_t CapsuleRadius = 0x5b4;

	}


	namespace ADBDPlayer {
		constexpr auto _interactionHandler = 0xb88;
	}

	namespace UPlayerInteractionHandler {
		constexpr auto _skillCheck = 0x310;
	}

	namespace UDBDOutlineComponent {
		constexpr auto InterpolationSpeed = 0x350; // float
		constexpr auto ShouldBeAboveOutlines = 0x354; // bool
		constexpr auto ForceOutlineFarAway = 0x355; // bool
		constexpr auto LimitToCustomDepthObjects = 0x356; // bool
		constexpr auto FadeOutAsClosingIn = 0x357; // bool
		constexpr auto IsAlwaysVisible = 0x358; // bool
		constexpr auto MinimumOutlineDistanceWhenIsAlwaysVisible = 0x35c; // float
		constexpr auto MinimumOutlineDistance = 0x360; // float
		constexpr auto OutlineName = 0x368; // FString

		// Color related offsets (used in Aura function)
		constexpr auto ColorRed = 0x3B4;   // float - R component
		constexpr auto ColorGreen = 0x3B8; // float - G component
		constexpr auto ColorBlue = 0x3BC;  // float - B component
		constexpr auto ColorAlpha = 0x3C0; // float - A component
		constexpr auto Visible = 0x3C8;    // bool - Visibility flag

		// Internal components
		constexpr auto RenderStrategySelector = 0x3d0; // FDBDOutlineRenderStrategySelector
		constexpr auto BatchMeshCommands = 0x410;     // UBatchMeshCommands*
		constexpr auto RenderingStrategy = 0x428;     // UBaseOutlineRenderStrategy*
	}

	const uint32_t GeneratorDBDOutline = 0x830;
	const uint32_t WindowDBDOutline = 0x4C8;
	const uint32_t MeatHookDBDOutline = 0x790;
	const uint32_t ChestDBDOutline = 0x530;
	//PALLET
	const uint32_t PalletOutline = 0x5d0;
	const uint32_t KillerOutline = 0x1c18;
	const uint32_t TotemOutline = 0x520;
	const uint32_t SurvivorOutline = 0x1c98;


} // Added missing closing brace for namespace O
