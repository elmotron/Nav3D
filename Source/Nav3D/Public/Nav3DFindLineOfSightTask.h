#pragma once

#include "Nav3DComponent.h"
#include "Nav3DStructs.h"
#include "Async/Async.h"
#include "Async/AsyncWork.h"

class FNav3DFindLineOfSightTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FNav3DFindLineOfSightTask>;

public:
	FNav3DFindLineOfSightTask(
		UNav3DComponent* Nav3DComponent,
		const FNav3DLink StartLink,
		const FNav3DLink TargetLink,
		const FVector& StartLocation,
		AActor* TargetActor,
		const float MinimumDistance,
		const FNav3DPathFindingConfig& Config,
		FNav3DPath& Path,
		const FFindLineOfSightTaskCompleteDynamicDelegate& Complete) :
	
		Nav3DComponent(Nav3DComponent),
		StartLink(StartLink),
		TargetLink(TargetLink),
		StartLocation(StartLocation),
		TargetActor(TargetActor),
		MinimumDistance(MinimumDistance),
		Config(Config),
		Path(Path),
		TaskComplete(Complete) {}

protected:
    TWeakObjectPtr<UNav3DComponent> Nav3DComponent;
	FNav3DLink StartLink;
	FNav3DLink TargetLink;
	FVector StartLocation;
	AActor* TargetActor;
	float MinimumDistance;
	FNav3DPathFindingConfig Config;
	FNav3DPath& Path;
	FFindLineOfSightTaskCompleteDynamicDelegate TaskComplete;

	void DoWork() const
	{
		Nav3DComponent->ExecutePathFinding(StartLink, TargetLink, StartLocation, TargetActor->GetActorLocation(),
		                                   Config, Path);
		Nav3DComponent->AddPathStartLocation(Path);
		Nav3DComponent->ApplyPathLineOfSight(Path, TargetActor, MinimumDistance);

		// Run the path pruning, smoothing and debug draw back on the game thread
		AsyncTask(ENamedThreads::GameThread, [=,this]()
		{
			if (!Nav3DComponent.IsValid())
			{
				return;
			}

			Nav3DComponent->ApplyPathPruning(Path, Config);
			Nav3DComponent->ApplyPathSmoothing(Path, Config);
#if WITH_EDITOR
			Nav3DComponent->RequestNavPathDebugDraw(Path);
#endif

			TaskComplete.Execute(Path.Points.Num() > 0);
		});
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FNav3DFindLineOfSightTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};
