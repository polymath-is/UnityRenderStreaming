﻿#include "pch.h"
#include "Context.h"
#include "IUnityGraphics.h"
#include "IUnityGraphicsD3D11.h"

namespace WebRTC
{
    IUnityInterfaces* s_UnityInterfaces = nullptr;
    IUnityGraphics* s_Graphics = nullptr;
    UnityGfxRenderer s_RenderType;
    //d3d11 context
    ID3D11DeviceContext* context;
    //d3d11 device
    ID3D11Device* g_D3D11Device = nullptr;
}
using namespace WebRTC;
//get d3d11 device
static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    switch (eventType)
    {
    case kUnityGfxDeviceEventInitialize:
    {
        s_RenderType = s_UnityInterfaces->Get<IUnityGraphics>()->GetRenderer();
        if (s_RenderType == kUnityGfxRendererD3D11)
        {
            g_D3D11Device = s_UnityInterfaces->Get<IUnityGraphicsD3D11>()->GetDevice();
            g_D3D11Device->GetImmediateContext(&context);
        }
        break;
    }
    case kUnityGfxDeviceEventShutdown:
    {
        //UnityPluginUnload not called normally
        s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
        break;
    }
    case kUnityGfxDeviceEventBeforeReset:
    {
        break;
    }
    case kUnityGfxDeviceEventAfterReset:
    {
        break;
    }
    };
}
// Unity plugin load event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = unityInterfaces->Get<IUnityGraphics>();
    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
    if (ContextManager::GetInstance() != nullptr)
    {
        ContextManager::GetInstance()->curContext->EncodeFrame();
    }
}


extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
    return OnRenderEvent;
}
