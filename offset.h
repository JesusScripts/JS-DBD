#pragma once
#include <iostream>

namespace offsets
{
    constexpr auto GWorld = 0xa8c7160;
    constexpr auto GNames = 0xa66ecc0;

    constexpr auto OwningGameInstance = 0x1C8; // Offsets::Classes::UWorld::OwningGameInstance
    constexpr auto LocalPlayers = 0x40; // Offsets::Classes::UWorld::UGameInstance::LocalPlayers
    constexpr auto PlayerController = 0x38; // Offsets::Classes::UWorld::UGameInstance::UPlayer::PlayerController
    constexpr auto ControllerId = 0xe8; // int32
    //uintptr_t PlayerControlRotation = 0x248; // Offsets::Classes::AActor::UObject::AController
    //uintptr_t GroupID = 0x38; // Offsets::Classes::APlayerState::Ainfo::AActor::UObject::GroupId
    //uintptr_t CurrentHealth = 0x1E0; // Offsets::ACharacter::APawn::AActor::UObject::CurrentHealth
    //uintptr_t CharacterMesh = 0x4d8; // Offsets::ACharacter::APawn::AActor::UObject::Mesh
    constexpr auto AcknowledgedPawn = 0x350; // Offsets::Classes::UWorld::UGameInstance::UPlayer::APlayerController::AcknowledgedPawn
    constexpr auto Controller = 0x2d8; // AController*
    constexpr auto Character = 0x2f8; 
    constexpr auto Mesh = 0x328; // USkeletalMeshComponent*
    constexpr auto PlayerCameraManager = 0x360; // Offsets::Classes::UWorld::UGameInstance::UPlayer::APlayerController::PlayerCameraManager
    constexpr auto CameraCachePrivate = 0x22d0; // // PlayerCameraManager::CameraCachePrivate
    constexpr auto PlayerState = 0x2c0;  // Offsets::Classes::AActor::UObject::APawn::PlayerState
    constexpr auto PlayerStateLocalPlayer = 0x2a8;  // Offsets::Classes::AActor::UObject::AController::PlayerState
    constexpr auto RootComponent = 0x1a8; // Offsets::Classes::AActor::UObject::RootComponent 
    constexpr auto RelativeLocation = 0x140; // Offsets::USceneComponent::UObject::RelativeLocation
    constexpr auto RelativeRotation = 0x158; // Offsets::USceneComponent::UObject::RelativeRotation 
    constexpr auto PersistentLevel = 0x38; // Offsets::Classes::UWorld::PersistentLevel
    //uintptr_t LODData = 0x508; // UMeshComponent::UPrimitiveComponent::USceneComponent::UActorComponent::UObject::LODData
    //uintptr_t PlayerNamePrivate = 0x318; // APlayerState PlayerNamePrivate
    constexpr auto ActorArray = 0xa0; // Offsets::Classes::UWorld::ULevel::UNetConnection::OwningActor
    constexpr auto ActorCount = 0xa8; // Offsets::Classes::UWorld::ULevel::UNetConnection::MaxPacket
    constexpr auto ActorID = 0x18; // offset_actor_id
    constexpr auto ComponentToWorld = 0x1E0;
}

