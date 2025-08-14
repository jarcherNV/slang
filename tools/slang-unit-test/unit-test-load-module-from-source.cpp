// unit-test-load-module-from-source.cpp

#include "slang-com-ptr.h"
#include "slang.h"
#include "unit-test/slang-unit-test.h"

#include <stdio.h>
#include <stdlib.h>

using namespace Slang;

// Test the loadModuleFromSource method and slang_loadModuleFromSource function
SLANG_UNIT_TEST(loadModuleFromSource)
{
    // Test source code with various content
    const char* testSource = R"(
        [shader("compute")]
        [numthreads(1,1,1)]
        void computeMain(uint3 workGroup : SV_GroupID)
        {
            // Simple compute shader
        }
    )";

    size_t sourceSize = strlen(testSource);

    ComPtr<slang::IGlobalSession> globalSession;
    SLANG_CHECK(slang_createGlobalSession(SLANG_API_VERSION, globalSession.writeRef()) == SLANG_OK);

    slang::SessionDesc sessionDesc = {};
    sessionDesc.targetCount = 1;
    slang::TargetDesc targetDesc = {};
    targetDesc.format = SLANG_HLSL;
    targetDesc.profile = globalSession->findProfile("sm_5_0");
    sessionDesc.targets = &targetDesc;

    ComPtr<slang::ISession> session;
    SLANG_CHECK(globalSession->createSession(sessionDesc, session.writeRef()) == SLANG_OK);

    // Test 1: Test the method version (existing functionality)
    {
        ComPtr<slang::IModule> module;
        ComPtr<ISlangBlob> diagnostics;

        // Use loadModuleFromSourceString which takes a string directly
        module = session->loadModuleFromSourceString(
            "testModule",
            "test.slang",
            testSource,
            diagnostics.writeRef());

        SLANG_CHECK(module != nullptr);
        if (diagnostics)
        {
            // If there are diagnostics, they should be warnings or errors
            SLANG_CHECK(diagnostics->getBufferSize() > 0);
        }
    }

    // Test 2: Test the new slang_loadModuleFromSource function
    {
        ComPtr<slang::IModule> module;
        ComPtr<ISlangBlob> diagnostics;

        module = slang_loadModuleFromSource(
            session,
            "testModule2",
            "test2.slang",
            (const void*)testSource,
            sourceSize,
            diagnostics.writeRef());

        SLANG_CHECK(module != nullptr);
        if (diagnostics)
        {
            // If there are diagnostics, they should be warnings or errors
            SLANG_CHECK(diagnostics->getBufferSize() > 0);
        }
    }

    // Test 3: Test with invalid parameters
    {
        ComPtr<slang::IModule> module;
        ComPtr<ISlangBlob> diagnostics;

        // Test with null session
        module = slang_loadModuleFromSource(
            nullptr,
            "testModule",
            "test.slang",
            (const void*)testSource,
            sourceSize,
            diagnostics.writeRef());

        SLANG_CHECK(module == nullptr);

        // Test with null moduleName
        module = slang_loadModuleFromSource(
            session,
            nullptr,
            "test.slang",
            (const void*)testSource,
            sourceSize,
            diagnostics.writeRef());

        SLANG_CHECK(module == nullptr);

        // Test with null path
        module = slang_loadModuleFromSource(
            session,
            "testModule",
            nullptr,
            (const void*)testSource,
            sourceSize,
            diagnostics.writeRef());

        SLANG_CHECK(module == nullptr);
    }

    // Test 4: Test with null source and non-zero size (should fail)
    {
        ComPtr<slang::IModule> module;
        ComPtr<ISlangBlob> diagnostics;

        module = slang_loadModuleFromSource(
            session,
            "testModule",
            "test.slang",
            nullptr,
            10,
            diagnostics.writeRef());

        SLANG_CHECK(module == nullptr);
    }

    // Test 5: Test with complex source code
    {
        const char* complexSource = R"(
            [shader("compute")]
            [numthreads(8,8,1)]
            void computeMain(uint3 workGroup : SV_GroupID, uint3 localID : SV_GroupThreadID)
            {
                uint2 pixelPos = workGroup.xy * uint2(8,8) + localID.xy;
                
                // Simple computation
                float result = sin(pixelPos.x * 0.1f) * cos(pixelPos.y * 0.1f);
                
                // Store result (in a real shader, this would go to a buffer)
                // outputBuffer[pixelPos] = result;
            }
        )";

        size_t complexSourceSize = strlen(complexSource);

        ComPtr<slang::IModule> module;
        ComPtr<ISlangBlob> diagnostics;

        module = slang_loadModuleFromSource(
            session,
            "complexModule",
            "complex.slang",
            (const void*)complexSource,
            complexSourceSize,
            diagnostics.writeRef());

        SLANG_CHECK(module != nullptr);
    }
}
