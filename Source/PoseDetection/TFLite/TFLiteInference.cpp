// Fill out your copyright notice in the Description page of Project Settings.


#include "TFLiteInference.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATFLiteInference::ATFLiteInference()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_ModelName = "lite-model_movenet_singlepose_lightning_tflite_float16_4.tflite";
	m_modelSize = 192;
	m_LandmarkNum = 17;
	m_landmarkProNum = 3;
}

// Called when the game starts or when spawned
void ATFLiteInference::BeginPlay()
{
	Super::BeginPlay();
	// load model
	FString modelPath = UKismetSystemLibrary::GetProjectDirectory() + "/SavedModel/" + m_ModelName;
	LoadModel(modelPath);

	// build interpreter
	BuildInterpreter();

	// Fill in the inputBuffer
	cv::Mat inputImage;
	FillInputBuffers(inputImage);

	// Invoke the interpreter
	InvokeInterpreter();


}

// Called every frame
void ATFLiteInference::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATFLiteInference::LoadModel(const FString& ModelName)
{
	// load model
	m_tfLiteModel = TfLiteModelCreateFromFile(TCHAR_TO_ANSI(*ModelName));

	// check if model exists
	if (!m_tfLiteModel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Model dose not exist!"));
		return false;
	}
	return true;
	
}

bool ATFLiteInference::BuildInterpreter()
{
	m_options = TfLiteInterpreterOptionsCreate();
	TfLiteInterpreterOptionsSetNumThreads(m_options, 1);
	m_interpreter = TfLiteInterpreterCreate(m_tfLiteModel, m_options);

	if (!m_interpreter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to build interpreter"));
		return false;
	}

	// Allocate tensor buffers
	if (TfLiteInterpreterAllocateTensors(m_interpreter) != kTfLiteOk) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to allocate tensor buffers"));
		return false;
	}

	// Get input tensor and output tensor
	m_inputTensor = TfLiteInterpreterGetInputTensor(m_interpreter, 0);

	// TODO models may generate several output, so the output is based on the models
	m_outputTensor = TfLiteInterpreterGetOutputTensor(m_interpreter, 0);
	return true;
}

void ATFLiteInference::DeleteModel()
{
	// Delete interpreter, options and model
	TfLiteInterpreterDelete(m_interpreter);
	TfLiteInterpreterOptionsDelete(m_options);
	TfLiteModelDelete(m_tfLiteModel);

	m_inputTensor = nullptr;
	m_outputTensor = nullptr;
}

void ATFLiteInference::FillInputBuffers(const cv::Mat& inputImage)
{
	cv::Mat processingImage;

	// BGR in web cam, so convert it to RGB
	cvtColor(inputImage, processingImage, cv::COLOR_BGRA2RGB);
	// Resize the images as required by the model.
	resize(processingImage, processingImage, cv::Size(m_modelSize, m_modelSize), 0, 0);

	TfLiteTensorCopyFromBuffer(m_inputTensor, processingImage.data, sizeof(float) * m_modelSize * m_modelSize * 3);
}

void ATFLiteInference::ProcessOutputTensors()
{
	const float* outputData = m_outputTensor->data.f;
}

bool ATFLiteInference::InvokeInterpreter()
{
	if (TfLiteInterpreterInvoke(m_interpreter) != kTfLiteOk) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to invoke interpreter"));
		return false;
	}

	return true;
}

