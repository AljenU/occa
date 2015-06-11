#include "occaParser.hpp"

using namespace occa;
using namespace occa::parserNS;

// Bugs:
//   - missing occaParallelFor
//   - expType::printLeaf for occaXId (extra space)
//   - 0.5 -> 0
//   - if() { (add space)
//   - occaUnroll
//   - ; before occaFor?
//   - occaUnroll(4)for(int fld = 0;fld < 4;++ fld){s_Q[fld][kx][n] = Q[id] + c0 * rhsQ0[id] + c1 * rhsQ1[id] + c2 * rhsQ2[id];id += 32;}if(saveValuesWB){id = n + k * 4 * 32;occaUnroll(4)for(int fld = 0;fld < 4;++ fld){Q[id] = s_Q[fld][kx][n];id += 32;}}
//   - s_D[4][4] -> s_D[4][4 + 1] [X + 1] -> [X] (chopped off)

int main(int argc, char **argv){
  occa::parser parser;
  parser.warnForMissingBarriers     = false;
  parser.warnForBarrierConditionals = false;

  std::string parsedContent;

  // parsedContent = parser.parseFile("tests/easy.c");
  // parsedContent = parser.parseFile("tests/test.cpp");
  // parsedContent = parser.parseFile("tests/test2.cpp");
  parsedContent = parser.parseFile("tests/scratch.okl");
  // parsedContent = parser.parseFile("tests/pwdg.okl");
  // parsedContent = parser.parseFile("tests/openclTest.cpp");
  // parsedContent = parser.parseFile("tests/cudaTest.cpp");
  // parsedContent = parser.parseFile("tests/fd2d_cuda.okl");
  // parsedContent = parser.parseFile("tests/midg.okl");
  // parsedContent = parser.parseFile("tests/cleanTest.c");
  // parsedContent = parser.parseFile("tests/clangTest.c");
  // parsedContent = parser.parseFile("tests/addVectors.okl");
  // parsedContent = parser.parseFile("tests/PCGpart1.cl");
  // parsedContent = parser.parseFile("tests/lookup_kernel.okl");
  // parsedContent = parser.parseFile("tests/reduction.cl");
  // parsedContent = parser.parseFile("tests/loopy.cl");
  // parsedContent = parser.parseFile("tests/addVectors_loopy.cl");

  //---[ Fortran ]----------------------
  // parsedContent = parser.parseFile("tests/scratch.ofl",
  //                                  occa::parsingInfo::parsingFortran);

  // parsedContent = parser.parseFile("tests/addVectors.ofl",
  //                                  occa::parserInfo::parsingFortran);

  // parsedContent = parser.parseFile("tests/fakeNuma.ofl",
  //                                  occa::parsingInfo::parsingFortran);
  //====================================

  //---[ Magic ]------------------------
  {
    // parser.magicEnabled = true;

    //---[ Generic ]--------------------
    // parsedContent = parser.parseFile("tests/ridgV.oak");
    // parsedContent = parser.parseFile("magicTests/addVectors.oak");
    // parsedContent = parser.parseFile("magicTests/fdTest.oak");

    //---[ Rodinia ]--------------------
    // parsedContent = parser.parseFile("magicTests/rodinia/backprop.oak");
    // parsedContent = parser.parseFile("magicTests/rodinia/bfs.oak"); // Fails: Has dynamic bounds
    // parsedContent = parser.parseFile("magicTests/rodinia/gaussian.oak");

    //---[ Arturo ]---------------------
    // parsedContent = parser.parseFile("magicTests/arturo/hermiteAdvec.okl");

    //---[ Frank ]----------------------
    // parsedContent = parser.parseFile("magicTests/frank/acousticsSurfaceKernelHex.okl");
    // parsedContent = parser.parseFile("magicTests/frank/acousticsSurfaceKernelPri.okl");
    // parsedContent = parser.parseFile("magicTests/frank/acousticsSurfaceKernelPyr.okl");
    // parsedContent = parser.parseFile("magicTests/frank/acousticsSurfaceKernelTet.okl");

    // parsedContent = parser.parseFile("magicTests/frank/acousticsUpdateKernelHex.okl");
    // parsedContent = parser.parseFile("magicTests/frank/acousticsUpdateKernelPri.okl");
    // parsedContent = parser.parseFile("magicTests/frank/acousticsUpdateKernelPyr.okl");
    // parsedContent = parser.parseFile("magicTests/frank/acousticsUpdateKernelTet.okl");

    // parsedContent = parser.parseFile("magicTests/frank/acousticsVolumeKernelHex.okl");
    // parsedContent = parser.parseFile("magicTests/frank/acousticsVolumeKernelPri.okl");
    // parsedContent = parser.parseFile("magicTests/frank/acousticsVolumeKernelPyr.okl");
    // parsedContent = parser.parseFile("magicTests/frank/acousticsVolumeKernelTet.okl");

    // parsedContent = parser.parseFile("magicTests/frank/genericPartialGetKernel.okl");
    // parsedContent = parser.parseFile("magicTests/frank/genericPartialPutKernel.okl");

    //---[ Reid ]-----------------------
    // parsedContent = parser.parseFile("magicTests/reid/scvadd.okl");
    // parsedContent = parser.parseFile("magicTests/reid/scvdiv.okl");
    // parsedContent = parser.parseFile("magicTests/reid/scvmult.okl");
    // parsedContent = parser.parseFile("magicTests/reid/test_complex_ips.okl");
    // parsedContent = parser.parseFile("magicTests/reid/test_ips.okl");
    // parsedContent = parser.parseFile("magicTests/reid/test_complex_pwdg.okl"); // Fails: Doesn't handle structs yet
    // parsedContent = parser.parseFile("magicTests/reid/test_pwdg.okl");         // Fails: Doesn't handle structs yet

    //---[ Jesse ]----------------------
    // parsedContent = parser.parseFile("magicTests/jesse/compute_error.okl");
    // parsedContent = parser.parseFile("magicTests/jesse/compute_u_surface.okl");
    // parsedContent = parser.parseFile("magicTests/jesse/invertMass.okl");
    // parsedContent = parser.parseFile("magicTests/jesse/rk_step_kernel.okl");
    // parsedContent = parser.parseFile("magicTests/jesse/surface_kernel.okl");
    // parsedContent = parser.parseFile("magicTests/jesse/volume_kernel.okl");

    //---[ Axel ]-----------------------
    // parsedContent = parser.parseFile("magicTests/axel/acousticsPmlSourceViaInterfKernel.okl"); // Fails: Simplification expansion get's too big and loops?
    // parsedContent = parser.parseFile("magicTests/axel/acousticsForcingSurfaceKernel.okl");     // Fails: face[f] in array access
    // parsedContent = parser.parseFile("magicTests/axel/acousticsPmlSurfaceKernel.okl");         // Fails: Switch
    // parsedContent = parser.parseFile("magicTests/axel/acousticsSourceViaInterfKernel.okl");    // Fails: Simplification expansion get's too big and loops?
    // parsedContent = parser.parseFile("magicTests/axel/acousticsPmlForcingSurfaceKernel.okl");  // Fails: face[f] in array access
    // parsedContent = parser.parseFile("magicTests/axel/acousticsSurfaceKernel.okl");            // Fails: Switch

    // parsedContent = parser.parseFile("magicTests/axel/acousticsCorrelationKernel.okl");
    // parsedContent = parser.parseFile("magicTests/axel/acousticsPmlVolumeKernel.okl");
    // parsedContent = parser.parseFile("magicTests/axel/acousticsUpdateKernel.okl");
    // parsedContent = parser.parseFile("magicTests/axel/genericPartialGetKernel.okl");
    // parsedContent = parser.parseFile("magicTests/axel/acousticsVolumeKernel.okl");
    // parsedContent = parser.parseFile("magicTests/axel/acousticsPmlUpdateKernel.okl");
    // parsedContent = parser.parseFile("magicTests/axel/genericPartialPutKernel.okl");
    // parsedContent = parser.parseFile("magicTests/axel/genericMaskKernel.okl");
    // parsedContent = parser.parseFile("magicTests/axel/genericSourceKernel.okl");
  }
  //====================================

  std::cout << parsedContent << '\n';
}
