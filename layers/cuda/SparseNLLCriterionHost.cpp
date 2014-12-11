// Copyright 2014 Facebook

#include "cuda/DeviceTensor.cuh"
#include "torch/fb/fbcunn/layers/cuda/DeviceTensorUtils.h"
#include "THC.h"
#include "THCTensor.h"
#include "torch/fb/fbcunn/layers/cuda/SparseNLLCriterion.cuh"

#include <cuda_runtime.h>
#include <glog/logging.h>
#include <luaT.h>
#include <lua.hpp>

using namespace std;
using namespace facebook::cuda;

namespace facebook { namespace deeplearning { namespace torch {

namespace {

inline THCudaTensor* getFieldCudaTensor(lua_State* L, int arg,
                                        const char* name) {
  return static_cast<THCudaTensor*>(luaT_getfieldcheckudata(
                                      L, arg, name, "torch.CudaTensor"));
}
inline THCudaTensor* getCudaTensor(lua_State* L, int arg) {
  return static_cast<THCudaTensor*>(luaT_checkudata(L, arg,
                                                    "torch.CudaTensor"));
}

int updateOutput(lua_State *L) {
  auto output = (THCudaTensor*)luaT_getfieldcheckudata(L, 1, "output",
                                                       "torch.CudaTensor");
  auto input     = (THCudaTensor*)luaT_checkudata(L, 2, "torch.CudaTensor");
  auto targetP   = (THCudaTensor*)luaT_checkudata(L, 3, "torch.CudaTensor");
  auto targetIdx = (THCudaTensor*)luaT_checkudata(L, 4, "torch.CudaTensor");
  auto batchSize = targetP->size[0];
  auto K = targetP->size[1];
  luaL_argcheck(L, (output->nDimension == 1) && (output->size[0] == 1),
                1, "output has wrong dimension");
  luaL_argcheck(L, (input->nDimension == 2) && (input->size[0] == batchSize)
                   && (THCudaTensor_isContiguous(input)),
                2, "input has wrong dimension");
  luaL_argcheck(L, (targetP->nDimension == 2)
                   && (THCudaTensor_isContiguous(targetP)),
                3, "targetP has wrong dimension");
  luaL_argcheck(L, (targetIdx->nDimension == 2)
                   && (targetIdx->size[0] == batchSize)
                   && (targetIdx->size[1] == K)
                   && (THCudaTensor_isContiguous(targetIdx)),
                4, "targetIdx has wrong dimension");

  auto targetIdxDev = torchToDeviceTensor<float, 2>(targetIdx);
  auto targetPDev = torchToDeviceTensor<float, 2>(targetP);
  auto inputDev = torchToDeviceTensor<float, 2>(input);
  auto outputDev = torchToDeviceTensor<float, 1>(output);

  detail::runSparseNLLCriterion_updateOutput(targetIdxDev, targetPDev,
                                             inputDev, outputDev);

  return 0;
}

int updateGradInput(lua_State *L) {
  auto gradInput =
    (THCudaTensor*)luaT_getfieldcheckudata(L, 1, "gradInput",
                                           "torch.CudaTensor");
  auto targetP   = (THCudaTensor*)luaT_checkudata(L, 2, "torch.CudaTensor");
  auto targetIdx = (THCudaTensor*)luaT_checkudata(L, 3, "torch.CudaTensor");
  auto batchSize = targetP->size[0];
  auto K = targetP->size[1];
  luaL_argcheck(L, (gradInput->nDimension == 2)
                && (gradInput->size[0] == batchSize)
                && (THCudaTensor_isContiguous(gradInput)),
                1, "gradInput has wrong dimension");
  luaL_argcheck(L, (targetP->nDimension == 2)
                && (THCudaTensor_isContiguous(targetP)),
                2, "targetP has wrong dimension");
  luaL_argcheck(L, (targetIdx->nDimension == 2)
                && (targetIdx->size[0] == batchSize)
                && (targetIdx->size[1] == K)
                && (THCudaTensor_isContiguous(targetIdx)),
                3, "targetIdx has wrong dimension");

  auto targetIdxDev = torchToDeviceTensor<float, 2>(targetIdx);
  auto targetPDev = torchToDeviceTensor<float, 2>(targetP);
  auto gradInputDev = torchToDeviceTensor<float, 2>(gradInput);

  detail::runSparseNLLCriterion_updateGradInput(targetIdxDev, targetPDev,
                                                gradInputDev);

  return 0;
}

const luaL_Reg functions [] = {
  {"SparseNLLCriterion_updateOutput", updateOutput},
  {"SparseNLLCriterion_updateGradInput", updateGradInput},
  {nullptr, nullptr}
};

} // namespace

void initSparseNLLCriterionCuda(lua_State *L) {
  luaT_pushmetatable(L, "torch.CudaTensor");
  luaT_registeratname(L, functions, "nn");
  lua_pop(L, 1);
}

}}} // namespaces
