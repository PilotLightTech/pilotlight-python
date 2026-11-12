/*
   pl_internal_m.c
     * hidden support for unstable extensions (API version < 1.0)

     Only stable extensions (API version >= 1.0) are bound to python. The
     exceptions are gltf & terrain (used to generate assets for the C version)
     and plScriptI (python scripts). gltf & terrain rely on asset types & ecs
     components owned by unstable extensions (texture, material, mesh, skeleton,
     animation, renderer, etc.), so those are registered here, behind the
     bound asset/ecs/starter calls:

       * plAssetI.finalize()  -> registers the unstable asset types first
       * plEcsI.finalize()    -> registers the unstable ecs components first
       * plStarterI.cleanup() -> cleans up the renderer first (if initialized)

     The renderer must be initialized for its types to be registered (it owns
     the object component used by gltf import), which requires the starter's
     device. So plStarterI must be initialized before plAssetI.finalize() for
     gltf import to work.
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] internal state
// [SECTION] implementations
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] internal state
//-----------------------------------------------------------------------------

static bool gbPyRendererInitialized = false;

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

// called by plAssetI.finalize() before gptAsset->finalize()
static void
pl__internal_register_asset_types(void)
{
    if(!gbPyRendererInitialized)
    {
        plDevice* ptDevice = gptStarter->get_device();
        if(ptDevice)
        {
            const plRendererInit tRendererInit = {
                .ptDevice    = ptDevice,
                .ptSwapchain = gptStarter->get_swapchain()
            };
            gbPyRendererInitialized = gptRenderer->initialize(&tRendererInit);
        }
    }

    gptTexture->register_asset_types();
    gptMaterial->register_asset_types();
    gptMesh->register_asset_types();
    gptSkeleton->register_asset_types();
    gptAnimation->register_asset_types();
    if(gbPyRendererInitialized)
        gptRenderer->register_asset_types();
}

// called by plEcsI.finalize() before gptEcs->finalize()
static void
pl__internal_register_ecs_components(void)
{
    gptIk->register_ecs_components();
    gptSkeleton->register_ecs_components();
    gptAnimation->register_ecs_components();
    gptCameraEcs->register_ecs_components();
    gptPhysics->register_ecs_components();
    if(gbPyRendererInitialized)
        gptRenderer->register_ecs_components();
}

// called by plStarterI.cleanup() before gptStarter->cleanup()
static void
pl__internal_cleanup(void)
{
    if(gbPyRendererInitialized)
    {
        gptRenderer->cleanup();
        gbPyRendererInitialized = false;
    }
}
