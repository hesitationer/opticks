/*
 * The information in this file is
 * Copyright(c) 2007 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "DemoAlgorithm.h"
#include "AppAssert.h"
#include "RasterElement.h"
#include "DesktopServices.h"
#include "RasterLayer.h"
#include "SpatialDataWindow.h"
#include "SpatialDataView.h"
#include "MessageLogResource.h"
#include "LayerList.h"
#include "ModelServices.h"
#include "WorkspaceWindow.h"

/**
 *  A baseclass obligation. Performs any necessary preprocessing steps
 *  prior to the display of the GUI. For the Demo plug-in, this does nothing.
 *
 *  @return true on success, false otherwise.
 */
bool DemoAlgorithm::preprocess()
{
   return true;
}

/**
 *  A baseclass obligation. Runs the algorithm, based on the inputs in mInputs,
 *  which has been populated from the GUI or the input arglist.
 *
 *  @return a pointer to the algorithm's GUI, or NULL if it has none
 *
 *  @see Demo::parseArgList
 *  @see Demo::extractFromGui
 */
bool DemoAlgorithm::processAll()
{
   REQUIRE(getRasterElement() != NULL);

   StepResource pStep("Starting demo", "app", "A389EA1B-EDE1-4074-8F7B-AAF6C594CD79");
   pStep->addProperty("Cube", getRasterElement()->getName());
   mpStep = pStep.get();

   std::vector<ColorType> colormap;
   populateColormapFromNodes(colormap);

   applyColormap(*(getRasterElement()), colormap);

   reportProgress(NORMAL, 100, "Demo Complete");
   pStep->finalize(Message::Success);
   mpStep = NULL;

   return true;
}

/**
 *  A baseclass obligation. Performs any required postprocessing
 *  after the dismissal of the GUI and after processAll. For the Demo
 *  plug-in, this does nothing.
 *
 *  @return true on success, false otherwise
 */
bool DemoAlgorithm::postprocess()
{
   return true;
}

/**
 *  Takes a colormap and sets it as the active colormap on the specified data 
 *  cube. Also, ensures that the cube is being displayed in grayscale mode, 
 *  which is required for the display of a colormap.
 *
 *  @param cube
 *       A reference to the cube being displayed with the colormap. 
 *  @param colormap
 *       The colormap to use when displaying the cube. Typically created by
 *       populateColormapFromNodes.
 *  @return a pointer to the algorithm's GUI, or NULL if it has none
 */
void DemoAlgorithm::applyColormap(RasterElement &cube, std::vector<ColorType>& colormap) const
{
   Service<DesktopServices> desktop;
   
   SpatialDataWindow *pSpatialWindow = 
      (SpatialDataWindow*)desktop->getWindow(cube.getName().c_str(), SPATIAL_DATA_WINDOW);
   if (pSpatialWindow == NULL) return;
   SpatialDataView* pView = pSpatialWindow->getSpatialDataView();
   if (pView == NULL) return;
   LayerList* pLayers = pView->getLayerList();
   if (pLayers == NULL) return;
   std::vector<Layer*> lRasterLayers;
   pLayers->getLayers(RASTER, lRasterLayers);
   if (lRasterLayers.empty()) return;
   
   ((RasterLayer*)(lRasterLayers.at(0)))->setDisplayMode(GRAYSCALE_MODE);
   ((RasterLayer*)(lRasterLayers.at(0)))->setColorMap("Dynamic Colormap", colormap);
}

/**
 *  The heart of the algorithm. Creates the colormap from the nodes. The nodes
 *  are supplied via the mInputs.mNodes data member. 
 *  REQUIRES that the number of cells to be created is >0
 *  REQUIRES that the number of nodes is >0
 *  ENSURES that the number of cells in the output vector matches the number
 *     requested.
 *
 *  @param colormap
 *       An empty colormap to be populated with the results.
 */
void DemoAlgorithm::populateColormapFromNodes(std::vector<ColorType>& colormap) const
{
   REQUIRE(mInputs.mNumberOfCells > 0);
   REQUIRE(mInputs.mNodes.size() >= 2);

   colormap.clear();

   unsigned int j = 0;
   for (int i=0; i<mInputs.mNumberOfCells; ++i)
   {
      double cellPosition = double(i)/double(mInputs.mNumberOfCells-1);

      // find next node after cell
      while (j < mInputs.mNodes.size() && mInputs.mNodes[j].first < cellPosition)
      {
         ++j;
      }

      // compute color and insert it into the colormap
      if (j == 0)
      {
         colormap.push_back(mInputs.mNodes.front().second);
      }
      else if (j == mInputs.mNodes.size())
      {
         colormap.push_back(mInputs.mNodes.back().second);
      }
      else
      {
         // compute the fractions of the two nodes for mixing
         double frac1 = (cellPosition-mInputs.mNodes[j-1].first)/(mInputs.mNodes[j].first-mInputs.mNodes[j-1].first);
         double frac2 = 1.0 - frac1;

         // color = frac1 * mNodes[j-1] + frac2 * mNodes[j]
         double red = frac1 * mInputs.mNodes[j-1].second.mRed + frac2 * mInputs.mNodes[j].second.mRed;
         double green = frac1 * mInputs.mNodes[j-1].second.mGreen+ frac2 * mInputs.mNodes[j].second.mGreen;
         double blue = frac1 * mInputs.mNodes[j-1].second.mBlue + frac2 * mInputs.mNodes[j].second.mBlue;

         // insert the computed color into the colormap
         ColorType color;
         color.mRed = int(red);
         color.mGreen = int(green);
         color.mBlue = int(blue);
         colormap.push_back(color);
      }
   }

   ENSURE(colormap.size() == mInputs.mNumberOfCells);
}

/**
 *  Constructor for the algorithm object.
 *
 *  @param rasterElement
 *       The data cube being displayed
 *  @param pProgress
 *       The progress object to report back via
 *  @param interactive
 *       true if the algorithm is being run interactively, false otherwise
 */
DemoAlgorithm::DemoAlgorithm(RasterElement &rasterElement, Progress *pProgress, bool interactive) :
   AlgorithmPattern(&rasterElement, pProgress, interactive, NULL), mpStep(NULL)
{
}

/**
 *  A baseclass obligation. Passes in the inputs from the GUI or the input
 *  arglist. Copies and validates the respective fields.
 *
 *  @param pAlgorithmData
 *       Initialization data to run the algorithm on
 *  @return true if the data were valid, false otherwise
 */
bool DemoAlgorithm::initialize(void *pAlgorithmData)
{
   bool success = true;
   REQUIRE(pAlgorithmData != NULL);

   mInputs = *(DemoInputs*)pAlgorithmData;

   PlugInSamplerQt::Node prevNode;
   for (std::vector<PlugInSamplerQt::Node>::iterator it=mInputs.mNodes.begin();
      it!=mInputs.mNodes.end(); ++it)
   {
      if (it != mInputs.mNodes.begin() && (*it).first <= prevNode.first)
      {
         reportProgress(ERRORS, 0, "Error DemoAlg012: Input values are out of order!");
         success = false;
         break;
      }
      prevNode = *it;
   }

   return success;
}

/**
 *  A baseclass obligation. Indicates whether the algorithm can abort or not.
 *
 *  @return true if the algorithm can abort, false otherwise
 */
bool DemoAlgorithm::canAbort() const
{
   return false;
}

/**
 *  A baseclass obligation. Performs an abort, if supported.
 *
 *  @return true if the algorithm was successfully aborted, false otherwise
 */
bool DemoAlgorithm::doAbort()
{
   return false;
}