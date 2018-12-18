#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/SubclassOf.h"
#include "Components/ActorComponent.h"
#include "JavascriptContext.h"

#include "JSComponent.generated.h"

class UJSObject;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALJSSAMPLE_API UJSComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UJSComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Javascript")
		FString ScriptSourceFile;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Javascript")
		bool bActiveWithinEditor;

	UPROPERTY(transient)
		UJavascriptContext* JavascriptContext;

	UPROPERTY(transient)
		UJavascriptIsolate* JavascriptIsolate;

	void LoadJSFile();

	UFUNCTION(BlueprintCallable, Category = "Javascript")
		void SetJsObject(UJSObject* jsObject);

private:
	UPROPERTY()
		UJSObject* JsObject;
};
