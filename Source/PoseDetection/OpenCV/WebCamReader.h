// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"    
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "WebCamReader.generated.h"

UCLASS()
class POSEDETECTION_API AWebCamReader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWebCamReader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	// The device ID opened by the Video Stream
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	int32 CameraID;

	// If the fame images should be resized every frame
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	bool ShouldResize;

	// The targeted resize width and height (width, height)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	FVector2D ResizeDeminsions;

	// The rate at which the color data array and video texture is updated (in frames per second)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	float RefreshRate;

	// The refresh timer
	UPROPERTY(BlueprintReadWrite, Category = Webcam)
	float RefreshTimer;

	// Blueprint Event called every time the video frame is updated
	UFUNCTION(BlueprintImplementableEvent, Category = Webcam)
	void OnNextVideoFrame();

	// OpenCV fields
	cv::Mat m_videoFrame;
	cv::VideoCapture m_videoStream;
	cv::Size size;

	// OpenCV prototypes
	void UpdateFrame();
	void DoProcessing();
	void UpdateTexture();

	// If the stream has successfully opened yet
	UPROPERTY(BlueprintReadOnly, Category = Webcam)
	bool isStreamOpen;

	// The videos width and height (width, height)
	UPROPERTY(BlueprintReadWrite, Category = Webcam)
	FVector2D m_originalFrameSize;

	// The current video frame's corresponding texture
	UPROPERTY(BlueprintReadOnly, Category = Webcam)
	UTexture2D* VideoTexture;

	// The current data array
	UPROPERTY(BlueprintReadOnly, Category = Webcam)
	TArray<FColor> m_frameData;

	// Camera Material Instance
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
	UStaticMeshComponent* CubeMesh;
protected:

	// Use this function to update the texture rects you want to change:
	// NOTE: There is a method called UpdateTextureRegions in UTexture2D but it is compiled WITH_EDITOR and is not marked as ENGINE_API so it cannot be linked
	// from plug-ins.
	// FROM: https://wiki.unrealengine.com/Dynamic_Textures
	void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

	// Pointer to update texture region 2D struct (Camera render target)
	FUpdateTextureRegion2D* VideoUpdateTextureRegion;

};
