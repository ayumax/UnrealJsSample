#include "JSComponent.h"
#include "JavascriptComponent.h"
#include "JavascriptIsolate.h"
#include "JavascriptContext.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "IV8.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Regex.h"
#include "JSObject.h"

UJSComponent::UJSComponent()
	: JavascriptContext(nullptr)
	, JavascriptIsolate(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.03f;
	bTickInEditor = false;
	bAutoActivate = true;
	bWantsInitializeComponent = true;
}


void UJSComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadJSFile();
}

void UJSComponent::OnRegister()
{
	auto ContextOwner = GetOuter();
	if (ContextOwner && !HasAnyFlags(RF_ClassDefaultObject) && !ContextOwner->HasAnyFlags(RF_ClassDefaultObject))
	{
		if (GetWorld() && ((GetWorld()->IsGameWorld() && !GetWorld()->IsPreviewWorld())))
		{
			UJavascriptIsolate* Isolate = nullptr;
			UJavascriptStaticCache* StaticGameData = Cast<UJavascriptStaticCache>(GEngine->GameSingleton);
			if (StaticGameData)
			{
				if (StaticGameData->Isolates.Num() > 0)
					Isolate = StaticGameData->Isolates.Pop();
			}

			if (!Isolate)
			{
				Isolate = NewObject<UJavascriptIsolate>();
				Isolate->Init(false);
				Isolate->AddToRoot();
			}

			auto* Context = Isolate->CreateContext();
			JavascriptContext = Context;
			JavascriptIsolate = Isolate;

			Context->Expose("Root", this);
			Context->Expose("GWorld", GetWorld());
			Context->Expose("GEngine", GEngine);
		}
	}

	Super::OnRegister();
}

void UJSComponent::SetJsObject(UJSObject* jsObject)
{
	JsObject = jsObject;
}

void UJSComponent::LoadJSFile()
{
	if (JavascriptContext == nullptr) return;

	auto scriptSourceFilePath = FPaths::Combine(FPaths::ProjectContentDir(), ScriptSourceFile);

	scriptSourceFilePath = FPaths::ConvertRelativePathToFull(scriptSourceFilePath);

	FString script;
	FFileHelper::LoadFileToString(script, *scriptSourceFilePath);

	const FRegexPattern pattern = FRegexPattern(FString(TEXT("class\\s+(.+)\\s+extends\\s+JSObject")));
	FRegexMatcher matcher(pattern, script);
	if (matcher.FindNext())
	{
		auto className = matcher.GetCaptureGroup(1);

		script = TEXT("(function (global) {\r\n") + script;
		script += TEXT("let MyUObject_C = require('uclass')()(global,") + className + TEXT(")\r\n");
		script += TEXT("let instance = new MyUObject_C()\r\n");
		script += TEXT("Root.SetJsObject(instance)\r\n");
		script += TEXT("})(this)");

		JavascriptContext->RunScript(script);
	}
}

FString UJSComponent::NotifyTrigger()
{
	if (JsObject != nullptr)
	{
		return JsObject->NotifyTrigger();
	}

	return FString();
}
