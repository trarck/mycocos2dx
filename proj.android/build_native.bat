# set params
@echo off
set NDK_ROOT=D:\android-ndk-r8b
set COCOS2DX_ROOT=D:\game\cocos2d-x
set GAME_ROOT=%COCOS2DX_ROOT%\mycocos2dx
set GAME_ANDROID_ROOT=%GAME_ROOT%\proj.android
set RESOURCE_ROOT=%GAME_ROOT%\Resources


echo %NDK_ROOT%
%NDK_ROOT%/ndk-build.cmd -C %GAME_ANDROID_ROOT%  NDK_MODULE_PATH=%COCOS2DX_ROOT%:%COCOS2DX_ROOT%/cocos2dx/platform/third_party/android/prebuilt




