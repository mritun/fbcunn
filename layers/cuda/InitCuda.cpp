/**
 * Copyright 2014 Facebook
 * @author Tudor Bosman (tudorb@fb.com)
 */

#include <lua.hpp>

namespace facebook { namespace deeplearning { namespace torch {

void initCrossMapNormalizationCuda(lua_State* L);
void initLookupTableGPUCuda(lua_State* L);
void initHSMCuda(lua_State* L);
void initTemporalConvolutionFB(lua_State *L);
void initTemporalKMaxPoolingCuda(lua_State* L);
void initOneBitQuantizationCuda(lua_State* L);
void initSparseNLLCriterionCuda(lua_State* L);
void initFeatureLPPoolingCuda(lua_State* L);

}}}  // namespace

using namespace facebook::deeplearning::torch;

extern "C" int LUAOPEN(lua_State* L) {
  initCrossMapNormalizationCuda(L);
  initLookupTableGPUCuda(L);
  initTemporalConvolutionFB(L);
  initTemporalKMaxPoolingCuda(L);
  initHSMCuda(L);
  initOneBitQuantizationCuda(L);
  initSparseNLLCriterionCuda(L);
  initFeatureLPPoolingCuda(L);

  return 0;
}
