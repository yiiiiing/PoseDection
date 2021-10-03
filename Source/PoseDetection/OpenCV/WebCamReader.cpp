// Fill out your copyright notice in the Description page of Project Settings.


#include "WebCamReader.h"
#include "Components/StaticMeshComponent.h"
#include <Rendering/Texture2DResource.h>

// Sets default values
AWebCamReader::AWebCamReader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("RenderingCube");
	RootComponent = CubeMesh;
	
	// Initialize OpenCV and webcam properties
	CameraID = 0;
	RefreshRate = 15;
	isStreamOpen = false;
	m_originalFrameSize = FVector2D(0, 0);
	ShouldResize = false;
	ResizeDeminsions = FVector2D(320, 240);
	RefreshTimer = 0.0f;
	m_videoStream = cv::VideoCapture();
	m_videoFrame = cv::Mat();

}

// Called when the game starts or when spawned
void AWebCamReader::BeginPlay()
{
	Super::BeginPlay();
	size = cv::Size(ResizeDeminsions.X, ResizeDeminsions.Y);
	// Open the stream
	isStreamOpen = m_videoStream.open(CameraID);
	if (isStreamOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("Webcam has been opened"));
		// Initialize stream		
		UpdateFrame();
		m_originalFrameSize = FVector2D(m_videoFrame.cols, m_videoFrame.rows);
		
		VideoTexture = UTexture2D::CreateTransient(m_originalFrameSize.X, m_originalFrameSize.Y);
		VideoTexture->UpdateResource();
		VideoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, m_originalFrameSize.X, m_originalFrameSize.Y);

		// Initialize data array
		m_frameData.Init(FColor(0, 0, 0, 255), m_originalFrameSize.X * m_originalFrameSize.Y);

		// Do first frame
		DoProcessing();
		UpdateTexture();
		OnNextVideoFrame();
	}

}

// Called every frame
void AWebCamReader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isStreamOpen)
	{
		UpdateFrame();
		DoProcessing();
		//UpdateTexture();
		//OnNextVideoFrame();
	}
// 
// 	RefreshTimer += DeltaTime;
// 	if (isStreamOpen && RefreshTimer > 1.0f / RefreshRate)
// 	{
// 		RefreshTimer -= 1.0f / RefreshRate;
// 		UpdateFrame();
// 		DoProcessing();
// 		UpdateTexture();
// 		OnNextVideoFrame();
// 	}
}

void AWebCamReader::UpdateFrame()
{
	if (isStreamOpen)
	{
		m_videoStream.read(m_videoFrame);
		if (ShouldResize)
		{
			cv::resize(m_videoFrame, m_videoFrame, size);
		}
	}
	else {
		isStreamOpen = false;
	}
}

void AWebCamReader::DoProcessing()
{
	// TODO: Do any processing here!
}

void AWebCamReader::UpdateTexture()
{
	if (isStreamOpen && m_videoFrame.data)
	{
		// Copy Mat data to Data array
		for (int y = 0; y < m_originalFrameSize.Y; y++)
		{
			for (int x = 0; x < m_originalFrameSize.X; x++)
			{
				int i = x + (y * m_originalFrameSize.X);
				m_frameData[i].B = m_videoFrame.data[i * 3 + 0];
				m_frameData[i].G = m_videoFrame.data[i * 3 + 1];
				m_frameData[i].R = m_videoFrame.data[i * 3 + 2];
			}
		}

		// Update texture 2D
		UpdateTextureRegions(VideoTexture, (int32)0, (uint32)1, VideoUpdateTextureRegion, (uint32)(4 * m_originalFrameSize.X), (uint32)4, (uint8*)m_frameData.GetData(), false);
	}
}

void AWebCamReader::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_RENDER_COMMAND(FUpdateTextureRegionsData)([RegionData, bFreeData](FRHICommandListImmediate& RHICmdList) {
			for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
			{
				int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
				if (RegionData->MipIndex >= CurrentFirstMip)
				{
					RHIUpdateTexture2D(
						RegionData->Texture2DResource->GetTexture2DRHI(),
						RegionData->MipIndex - CurrentFirstMip,
						RegionData->Regions[RegionIndex],
						RegionData->SrcPitch,
						RegionData->SrcData
						+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
						+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
					);
				}
			}
			if (bFreeData)
			{
				FMemory::Free(RegionData->Regions);
				FMemory::Free(RegionData->SrcData);
			}
			delete RegionData;
		});

	}
}

