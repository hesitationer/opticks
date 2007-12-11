/*
 * The information in this file is
 * Copyright(c) 2007 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "AppVersion.h"
#include "DtedImporter.h"
#include "Classification.h"
#include "AppConfig.h"
#include "Endian.h"
#include "ImportDescriptor.h"
#include "ModelServices.h"
#include "ObjectResource.h"
#include "RasterDataDescriptor.h"
#include "RasterFileDescriptor.h"
#include "RasterUtilities.h"
#include "TypesFile.h"

#include <string>
using namespace std;

DtedImporter::DtedImporter()
{
   setName("DTED Importer");
   setCreator("Ball Aerospace & Technologies Corp.");
   setCopyright(APP_COPYRIGHT);
   setVersion(APP_VERSION_NUMBER);
#if defined(WIN_API)
   setExtensions("DTED Header Files (*.dt?)");
#else
   setExtensions("DTED Header Files (*.dt? *.DT?)");
#endif
   setDescription("Import Digital Terrain Elevation Data (DTED) Files");
   setDescriptorId("{433CC37D-2F29-47e7-8675-CA1129B77C89}");
   allowMultipleInstances(true);
   setProductionStatus(APP_IS_PRODUCTION_RELEASE);
}

DtedImporter::~DtedImporter()
{
}

vector<ImportDescriptor*> DtedImporter::getImportDescriptors(const string& filename)
{
   vector<ImportDescriptor*> descriptors;

   if (filename.empty() == false)
   {
      bool bSuccess = false;

      FILE* pFile = fopen(filename.c_str(), "rb");
      if (pFile != NULL)
      {
         bSuccess = mUhl_h.readHeader(pFile);
         if (bSuccess == true)
         {
            bSuccess = mDsi_h.readHeader(pFile);
         }

         if (bSuccess == true)
         {
            bSuccess = mAcc_h.readHeader(pFile);
         }

         fclose(pFile);
      }

      if (bSuccess == true)
      {
         RasterDataDescriptor* pDescriptor =
            RasterUtilities::generateRasterDataDescriptor(filename, NULL,
               mUhl_h.getLongCount(), mUhl_h.getLatCount(), 1, BIP, INT2UBYTES, IN_MEMORY);
         if (pDescriptor != NULL)
         {
            // Classification
            FactoryResource<Classification> pClassification;
            if (pClassification.get() != NULL)
            {
               string secCode;
               secCode.append(1, mDsi_h.getSecurityCode());
               pClassification->setLevel(secCode);

               pDescriptor->setClassification(pClassification.get());
            }

            // Bad values
            vector<int> badValues;
            badValues.push_back(0);
            pDescriptor->setBadValues(badValues);
            
            RasterFileDescriptor* pFileDescriptor = dynamic_cast<RasterFileDescriptor*>(
               RasterUtilities::generateAndSetFileDescriptor(pDescriptor, filename, "", BIG_ENDIAN));

            if (pFileDescriptor != NULL)
            {
               // Header bytes
               pFileDescriptor->setHeaderBytes(mAcc_h.getTotalHeaderSize());

               // Preline bytes
               pFileDescriptor->setPrelineBytes(8);

               // Postline bytes
               pFileDescriptor->setPostlineBytes(4);

               // GCPs
               list<GcpPoint> gcps;

               GcpPoint lowerLeft;
               lowerLeft.mPixel.mX = 0.0;
               lowerLeft.mPixel.mY = 0.0;
               lowerLeft.mCoordinate.mX = mDsi_h.getSWLatCorner();
               lowerLeft.mCoordinate.mY = mDsi_h.getSWLongCorner();
               gcps.push_back(lowerLeft);

               GcpPoint lowerRight;
               lowerRight.mPixel.mX = mUhl_h.getLongCount() - 1.0;
               lowerRight.mPixel.mY = 0.0;
               lowerRight.mCoordinate.mX = mDsi_h.getSELatCorner();
               lowerRight.mCoordinate.mY = mDsi_h.getSELongCorner();
               gcps.push_back(lowerRight);

               GcpPoint upperLeft;
               upperLeft.mPixel.mX = 0.0;
               upperLeft.mPixel.mY = mUhl_h.getLatCount() - 1.0;
               upperLeft.mCoordinate.mX = mDsi_h.getNWLatCorner();
               upperLeft.mCoordinate.mY = mDsi_h.getNWLongCorner();
               gcps.push_back(upperLeft);

               GcpPoint upperRight;
               upperRight.mPixel.mX = mUhl_h.getLongCount() - 1.0;
               upperRight.mPixel.mY = mUhl_h.getLatCount() - 1.0;
               upperRight.mCoordinate.mX = mDsi_h.getNELatCorner();
               upperRight.mCoordinate.mY = mDsi_h.getNELongCorner();
               gcps.push_back(upperRight);

               GcpPoint center;
               center.mPixel.mX = mUhl_h.getLongCount() / 2.0 - 0.5;
               center.mPixel.mY = mUhl_h.getLatCount() / 2.0 - 0.5;
               center.mCoordinate.mX = (mDsi_h.getNWLatCorner() + mDsi_h.getSWLatCorner()) / 2.0;
               center.mCoordinate.mY = (mDsi_h.getNWLongCorner() + mDsi_h.getNELongCorner()) / 2.0;
               gcps.push_back(center);

               pFileDescriptor->setGcps(gcps);
            }

            Service<ModelServices> pModel;

            ImportDescriptor* pImportDescriptor = pModel->createImportDescriptor(pDescriptor);
            if (pImportDescriptor != NULL)
            {
               descriptors.push_back(pImportDescriptor);
            }
            else
            {
               pModel->destroyDataDescriptor(pDescriptor);
            }
         }
      }
   }

   return descriptors;
}

unsigned char DtedImporter::getFileAffinity(const std::string& filename)
{
   if (getImportDescriptors(filename).empty())
   {
      return Importer::CAN_NOT_LOAD;
   }
   else
   {
      return Importer::CAN_LOAD;
   }
}

bool DtedImporter::validate(const DataDescriptor* pDescriptor, string& errorMessage) const
{
   bool bValid = RasterElementImporterShell::validate(pDescriptor, errorMessage);
   if (bValid == false)
   {
      return false;
   }

   const RasterDataDescriptor* pRasterDescriptor = dynamic_cast<const RasterDataDescriptor*>(pDescriptor);
   if (pRasterDescriptor == NULL)
   {
      errorMessage = "The data descriptor is invalid!";
      return false;
   }

   // Multiple bands
   unsigned int numBands = pRasterDescriptor->getBandCount();
   if (numBands > 1)
   {
      errorMessage = "The data must only have one band!";
      return false;
   }

   return true;
}