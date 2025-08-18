#pragma once

#include "Nav3DComponent.h"
#include "Nav3DStructs.h"
#include "Async/Async.h"
#include "Async/AsyncWork.h"

class FNav3DFindPathTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FNav3DFindPathTask>;

public:
	FNav3DFindPathTask(
		UNav3DComponent* Nav3DComponent,
		const FNav3DLink StartLink,
		const FNav3DLink TargetLink,
		const FVector& StartLocation,
		const FVector& TargetLocation,
		const FNav3DPathFindingConfig& Config,
		const FNav3DPathSharedPtr& Path,
		const FFindPathTaskCompleteDynamicDelegate& Complete) :

		Nav3DComponent(Nav3DComponent),
		StartLink(StartLink),
		TargetLink(TargetLink),
		StartLocation(StartLocation),
		TargetLocation(TargetLocation),
		Config(Config),
		Path(Path),
		TaskComplete(Complete)
	{
	}

protected:
	TWeakObjectPtr<UNav3DComponent> Nav3DComponent;
	FNav3DLink StartLink;
	FNav3DLink TargetLink;
	FVector StartLocation;
	FVector TargetLocation;
	FNav3DPathFindingConfig Config;
	FNav3DPathSharedPtr Path;
	FFindPathTaskCompleteDynamicDelegate TaskComplete;

	void DoWork() const
	{
		// Capture the local variables
		TWeakObjectPtr<UNav3DComponent> Comp = Nav3DComponent;
		FNav3DPathSharedPtr PathPtr = Path;
		FNav3DPathFindingConfig ConfigCopy = Config;
		FFindPathTaskCompleteDynamicDelegate TaskCompleteCopy = TaskComplete;

		if (!Comp.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid UNav3DComponent, FNav3DFindPathTask abort"));
			return;
		}

		const double Start = FPlatformTime::Seconds();
		Nav3DComponent->ExecutePathFinding(StartLink, TargetLink, StartLocation, TargetLocation, Config, *Path.Get());
		Nav3DComponent->AddPathStartLocation(*Path.Get());
		const double End = FPlatformTime::Seconds();

#if WITH_EDITORONLY_DATA
		if (Nav3DComponent->bDebugLogPathfinding)
		{
			UE_LOG(LogTemp, Display, TEXT("Pathfinding took %f seconds"), End - Start);
		}
#endif

		// Run the path pruning, smoothing and debug draw back on the game thread
		AsyncTask(ENamedThreads::GameThread, [Comp, PathPtr, ConfigCopy, TaskCompleteCopy]()
		{
			FNav3DPath& PathRef = *PathPtr.Get();

			Comp->ApplyPathPruning(PathRef, ConfigCopy);
			Comp->ApplyPathSmoothing(PathRef, ConfigCopy);

#if WITH_EDITOR
			Comp->RequestNavPathDebugDraw(PathRef);
#endif

			TaskCompleteCopy.Execute(PathRef.Points.Num() > 0);
		});
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FNav3DFindPathTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};
