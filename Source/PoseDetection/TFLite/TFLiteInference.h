// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "tensorflow/lite/c/c_api.h"
#include <opencv2/opencv.hpp>
#include "TFLiteInference.generated.h"

UCLASS()
class POSEDETECTION_API ATFLiteInference : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATFLiteInference();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool LoadModel(const FString& ModelName);

	bool BuildInterpreter();

	void DeleteModel();

	// Fill input buffers
	void FillInputBuffers(const cv::Mat& inputImage);

	// Read output Buffers
	void ProcessOutputTensors();

	// Invoke the interpreter
	bool InvokeInterpreter();

private:
	TfLiteModel* m_tfLiteModel;
	TfLiteInterpreterOptions* m_options;
	TfLiteInterpreter* m_interpreter;

	TfLiteTensor* m_inputTensor;
	// outputTensor, type is const TFliteTensor*
	const TfLiteTensor* m_outputTensor;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString m_ModelName;

	// Model parameters
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 m_modelSize;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 m_LandmarkNum;
	// number of properties for each output points, like, x, y, z, probability of visibility
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 m_landmarkProNum;

};
