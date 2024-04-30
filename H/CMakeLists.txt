### project
project(H2O)
cmake_minimum_required(VERSION 2.8.12)

### compil
set(CMAKE_CXX_COMPILER      "clang++")
set(CMAKE_CXX_FLAGS         "-std=c++11 -W -Wall -DLINUX=1 -pthread")
set(CMAKE_CXX_FLAGS_DEBUG   "-DDEBUG -g3 -O0 -DGFX_MULTI_API -DENABLE_AUTOMATIC_FILE_RELOAD -DENABLE_KEYBOARD_AND_MOUSE -DENABLE_PAUSE -DENABLE_LOG -DENABLE_RUNTIME_COMPILATION -DENABLE_TWEAKVAL -DMUTE_SOUND")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Oz -DGFX_OPENGL_ONLY")

# set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-ldl")

# thirdparty are declared as system includes, to disable warnings from
# those headers.
include_directories(SYSTEM thirdparty thirdparty/unittest-cpp-1.6.0)
include_directories(src)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

#
# Static library: gfx
#
add_library(gfx STATIC
  src/gfx/Helpers.cpp
  src/gfx/IGraphicLayer.cpp
  src/gfx/ResourceID.cpp
  src/gfx/ShadingParameters.cpp
  src/gfx/OpenGL/Extensions.cpp
  src/gfx/OpenGL/OpenGLLayer.cpp
  src/gfx/OpenGL/OpenGLTypeConversion.cpp
  )

#
# Static library: engine
#
add_library(engine STATIC
  src/engine/algebra/Functions.cpp
  src/engine/algebra/Matrix.cpp
  src/engine/algebra/Quaternion.cpp
  src/engine/algebra/Spline.cc
  src/engine/algebra/Vector3.cpp
  src/engine/core/Camera.cpp
  src/engine/core/Debug.cpp
  src/engine/core/FileIO.cpp
  src/engine/core/Log.cpp
  src/engine/core/msys_temp.cpp
  src/engine/core/Node.cpp
  src/engine/core/RigidTransform.cpp
  src/engine/core/Settings.cpp
  src/engine/core/StringUtils.cpp
  src/engine/mesh/Cube.cpp
  src/engine/mesh/HeightMap.cpp
  src/engine/mesh/Marching.cpp
  src/engine/mesh/Mesh.cpp
  src/engine/mesh/MeshPool.cpp
  src/engine/mesh/Revolution.cpp
  src/engine/mesh/RevolutionFunctions.cpp
  src/engine/mesh/Utils.cpp
  src/engine/noise/Gradient.cpp
  src/engine/noise/Hash.cpp
  src/engine/noise/LatticeNoise.cpp
  src/engine/noise/PerlinNoise.cpp
  src/engine/noise/Rand.cpp
  src/engine/noise/SimplexNoise.cpp
  src/engine/noise/SimplexValueNoise.cpp
  src/engine/noise/Utils.cpp
  src/engine/noise/ValueNoise.cpp
  src/engine/noise/WorleyNoise.cpp
  src/engine/runtime/FileWatcher.cpp
  src/engine/runtime/RuntimeCompiler.cpp
  src/engine/sound/MusicPlayerBASS.cpp
  src/engine/texture/Texture.cpp
  src/engine/texture/Utils.cpp
  src/engine/timeline/Clock.cpp
  src/engine/timeline/Timeline.cpp
  src/engine/timeline/Variable.cpp
  thirdparty/imgui/imgui.cpp
  thirdparty/imgui/imgui_demo.cpp
  thirdparty/imgui/imgui_draw.cpp
  thirdparty/tweakval/tweakval.cpp
  )

#
# Static library: platform
#
add_library(platform STATIC
  src/platform/XLibPlatform.cpp
  )

#
# Static library: runtimecompiler
#
add_library(runtimecompiler STATIC
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/BuildTool.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/Compiler_PlatformPosix.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/FileChangeNotifier.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/SimpleFileWatcher/FileWatcher.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/SimpleFileWatcher/FileWatcherLinux.cpp
  )

#
# Static library: runtimeobjectsystem
#
add_library(runtimeobjectsystem STATIC
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectFactorySystem/ObjectFactorySystem.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModuleSource.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/RuntimeObjectSystem.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/RuntimeObjectSystem_PlatformPosix.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/SimpleSerializer/SimpleSerializer.cpp
  )

#
# Static library: unittestpp
#
add_library(unittestpp STATIC
  thirdparty/unittest-cpp-1.6.0/UnitTest++/AssertException.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/Checks.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/CompositeTestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/CurrentTest.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/DeferredTestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/DeferredTestResult.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/MemoryOutStream.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/Posix/SignalTranslator.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/Posix/TimeHelpers.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/ReportAssert.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/RequiredCheckException.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/RequiredCheckTestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/Test.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestDetails.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestList.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestReporterStdout.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestResults.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestRunner.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/ThrowingTestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TimeConstraint.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/XmlTestReporter.cpp
  )

#
# Executable: unittests
#
add_executable(unittests
  src/unittests/Algorithm.cpp
  src/unittests/Quaternion.cpp
  src/unittests/Vector2.cpp
  src/unittests/Vector3.cpp
  src/unittests/Vector4.cpp
  src/unittests/main.cpp
  )
target_link_libraries(unittests engine unittestpp)

#
# Executable: example01
#
#add_executable(example01
#  src/example01/main.cpp
#  )
#target_link_libraries(example01 gfx engine platform GL GLU X11 dl)

#
# Executable: example02
#
add_executable(example02
  src/example02/Example02.cpp
  src/example02/main.cpp
  src/meshes/MyCube.cpp
  src/tool/CameraLoader.cpp
  src/tool/CommonMaterial.cpp
  src/tool/DebugControls.cpp
  src/tool/MeshLoader.cpp
  src/tool/ShaderHelper.cpp
  src/tool/ShaderLoader.cpp
  src/tool/ShaderMinifier.cpp
  src/tool/mesh/MeshHelper.cpp
  src/tool/mesh/VertexDataPNT.cpp
  )
target_link_libraries(example02 gfx engine platform runtimeobjectsystem runtimecompiler GL GLU X11 dl)

#
# Executable: h
#
add_executable(h
  src/H/BoatScene.cpp
  src/H/CityScene.cpp
  src/H/DescentScene.cpp
  src/H/EmergenceScene.cpp
  src/H/H.cpp
  src/H/InsideTempleScene.cpp
  src/H/LoadingBar.cpp
  src/H/main.cpp
  src/H/RuinsScene.cpp
  src/H/TheSubmersible.cpp
  src/H/ViaductScene.cpp
  src/meshes/Artemision.cpp
  src/meshes/CityFloor.cpp
  src/meshes/Fence.cpp
  src/meshes/Fountain.cpp
  src/meshes/Gate.cpp
  src/meshes/GreekColumn.cpp
  src/meshes/Houses.cpp
  src/meshes/Lightning.cpp
  src/meshes/Obelisk.cpp
  src/meshes/Parthenon.cpp
  src/meshes/Plant.cpp
  src/meshes/PoseidonSeated.cpp
  src/meshes/ResearchVessel.cpp
  src/meshes/Road.cpp
  src/meshes/Rocks.cpp
  src/meshes/RuinsFloor.cpp
  src/meshes/SeaSurface.cpp
  src/meshes/Seagull.cpp
  src/meshes/Stairs.cpp
  src/meshes/Statue.cpp
  src/meshes/Submersible.cpp
  src/meshes/Tholos.cpp
  src/meshes/Trident.cpp
  src/meshes/UnderwaterFloor.cpp
  src/meshes/Viaduct.cpp
  src/postprocessing/BilateralGaussianBlur.cpp
  src/postprocessing/CommonPostProcessing.cpp
  src/postprocessing/FXAA.cpp
  src/postprocessing/FinalCombine.cpp
  src/postprocessing/GaussianBlur.cpp
  src/postprocessing/StreakBlur.cpp
  src/textures/Cobbles.cpp
  src/textures/GreekColumn.cpp
  src/textures/Ground.cpp
  src/textures/Marble.cpp
  src/textures/OldStone.cpp
  src/textures/RandomTexture.cpp
  src/textures/ResearchVessel.cpp
  src/textures/SeaFoam.cpp
  src/tool/CameraLoader.cpp
  src/tool/CommonMaterial.cpp
  src/tool/DebugControls.cpp
  src/tool/DebugGraphicLayer.cpp
  src/tool/FixedLightToolBox.cpp
  src/tool/ImGuiHelper.cpp
  src/tool/MeshLoader.cpp
  src/tool/ParticleField.cpp
  src/tool/ProjectLoader.cpp
  src/tool/Quad.cpp
  src/tool/RenderList.cpp
  src/tool/ShaderHelper.cpp
  src/tool/ShaderLoader.cpp
  src/tool/ShaderMinifier.cpp
  src/tool/SimpleSkyGradient.cpp
  src/tool/TextureHelper.cpp
  src/tool/TextureLoader.cpp
  src/tool/VolumetricLighting.cpp
  src/tool/mesh/MeshHelper.cpp
  src/tool/mesh/VertexDataP2dT.cpp
  src/tool/mesh/VertexDataPNT.cpp
  src/tool/mesh/VertexDataPNTT.cpp
  thirdparty/64klang2_Player/MusicPlayer64k2.cpp
  )
target_link_libraries(h gfx engine platform runtimeobjectsystem runtimecompiler GL GLU X11 dl)

#
# Executable: meshplayground
#
add_executable(meshplayground
  src/H/TheSubmersible.cpp
  src/meshes/Artemision.cpp
  src/meshes/GreekColumn.cpp
  src/meshes/Parthenon.cpp
  src/meshes/ResearchVessel.cpp
  src/meshes/ResearchVessel.cpp
  src/meshes/SdfTest.cpp
  src/meshes/Stairs.cpp
  src/meshes/Statue.cpp
  src/meshes/Submersible.cpp
  src/meshes/Viaduct.cpp
  src/meshplayground/OrbitCameraControls.cpp
  src/meshplayground/main.cpp
  src/postprocessing/CommonPostProcessing.cpp
  src/postprocessing/FXAA.cpp
  src/postprocessing/FinalCombine.cpp
  src/postprocessing/GaussianBlur.cpp
  src/postprocessing/StreakBlur.cpp
  src/postprocessing/ToneMapping.cpp
  src/textures/GreekColumn.cpp
  src/textures/GreekTiles.cpp
  src/textures/Marble.cpp
  src/textures/MaterialStudy.cpp
  src/textures/OldStone.cpp
  src/textures/RandomTexture.cpp
  src/textures/ResearchVessel.cpp
  src/tool/CommonMaterial.cpp
  src/tool/MeshLoader.cpp
  src/tool/Quad.cpp
  src/tool/ShaderHelper.cpp
  src/tool/ShaderLoader.cpp
  src/tool/ShaderMinifier.cpp
  src/tool/TextureHelper.cpp
  src/tool/TextureLoader.cpp
  src/tool/mesh/MeshHelper.cpp
  src/tool/mesh/VertexDataP2dT.cpp
  src/tool/mesh/VertexDataPNT.cpp
  src/tool/mesh/VertexDataPNTT.cpp
  )
target_link_libraries(meshplayground gfx engine platform runtimeobjectsystem runtimecompiler GL GLU X11 dl)

#
# Executable: sandbox
#
add_executable(sandbox
  src/meshes/Dolphin.cpp
  src/meshes/GreekColumn.cpp
  src/meshes/Parthenon.cpp
  src/meshes/Shark.cpp
  src/meshes/Stairs.cpp
  src/postprocessing/BilateralGaussianBlur.cpp
  src/postprocessing/CommonPostProcessing.cpp
  src/postprocessing/FXAA.cpp
  src/postprocessing/FinalCombine.cpp
  src/postprocessing/GaussianBlur.cpp
  src/postprocessing/StreakBlur.cpp
  src/sandbox/Attractor.cpp
  src/sandbox/CameraTest.cpp
  src/sandbox/CubeGrid.cpp
  src/sandbox/CubeMapTest.cpp
  src/sandbox/Flipper.cpp
  src/sandbox/Fountain.cpp
  src/sandbox/NodesTest.cpp
  src/sandbox/SeaSurface.cpp
  src/sandbox/TestScene.cpp
  src/sandbox/TextureShelf.cpp
  src/sandbox/main.cpp
  src/textures/GreekTiles.cpp
  src/textures/NoiseTextures.cpp
  src/textures/RandomTexture.cpp
  src/textures/RCCTest.cpp
  src/tool/CameraLoader.cpp
  src/tool/CommonMaterial.cpp
  src/tool/DebugControls.cpp
  src/tool/DebugGraphicLayer.cpp
  src/tool/FixedLightToolBox.cpp
  src/tool/MeshLoader.cpp
  src/tool/ProjectLoader.cpp
  src/tool/Quad.cpp
  src/tool/ShaderHelper.cpp
  src/tool/ShaderLoader.cpp
  src/tool/ShaderMinifier.cpp
  src/tool/TextureHelper.cpp
  src/tool/TextureLoader.cpp
  src/tool/VolumetricLighting.cpp
  src/tool/mesh/MeshHelper.cpp
  src/tool/mesh/VertexDataP2dT.cpp
  src/tool/mesh/VertexDataPN.cpp
  src/tool/mesh/VertexDataPNT.cpp
  src/tool/mesh/VertexDataPNTT.cpp
#  src/tool/Server.cpp
  )
target_link_libraries(sandbox gfx engine platform runtimeobjectsystem runtimecompiler GL GLU X11 dl)
