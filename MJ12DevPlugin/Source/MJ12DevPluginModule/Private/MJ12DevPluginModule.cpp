#include "MJ12DevPluginModule.h"

#define LOCTEXT_NAMESPACE "FMJ12DevPluginModule"

void FMJ12DevPluginModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    UE_LOG(LogTemp, Warning, TEXT("MJ12DevPluginModule has started!"));
}

void FMJ12DevPluginModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
    UE_LOG(LogTemp, Warning, TEXT("MJ12DevPluginModule has shut down!"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMJ12DevPluginModule, MJ12DevPluginModule)
