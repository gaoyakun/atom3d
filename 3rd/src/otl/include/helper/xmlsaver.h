#ifndef _XMLSAVER_H_
#define _XMLSAVER_H_

#include "opentree/opentree.h"
#include "tinyxml/tinyxml.h"
#include <string>

namespace opentree
{

  /// \addtogroup Helper
  /// @{

  /// XML Saver for the OpenTree Tree format

  class xmlSaver
  {
    public:
    xmlSaver() {}
    ~xmlSaver() {}
    std::string docStart;
    std::string docLevel;

    bool Save(TreeData& params, const char* filename, const char* name)
    {
      char buffer[32];

      docStart= "<?xml version=\"1.0\"  >";
      docStart+="<opentree>";
      docStart+="<species name=\"";docStart+=name;docStart+="\">";

      docStart+="<Lobes>";
        sprintf(buffer,"%d",params.lobes); docStart+=buffer;
          docStart+="</Lobes>";
      docStart+="<LobeDepth>";
        sprintf(buffer,"%f",params.lobeDepth); docStart+=buffer;
          docStart+="</LobeDepth>";

      docStart+="<BaseSize>";
        sprintf(buffer,"%f",params.baseSize); docStart+=buffer;
          docStart+="</BaseSize>";
      docStart+="<AttractionUp>";
        sprintf(buffer,"%f",params.attractionUp); docStart+=buffer;
          docStart+="</AttractionUp>";

      docStart+="<Scale>";
        sprintf(buffer,"%f",params.scale); docStart+=buffer;
          docStart+="</Scale>";
      docStart+="<ScaleV>";
        sprintf(buffer,"%f",params.scaleV); docStart+=buffer;
          docStart+="</ScaleV>";
      
      docStart+="<Ratio>";
        sprintf(buffer,"%f",params.ratio); docStart+=buffer;
          docStart+="</Ratio>";
      docStart+="<RatioPower>";
        sprintf(buffer,"%f",params.ratioPower); docStart+=buffer;
          docStart+="</RatioPower>";

      docStart+="<Shape>";
        sprintf(buffer,"%d",params.shape); docStart+=buffer;
          docStart+="</Shape>";

      docStart+="<Levels>";
        sprintf(buffer,"%d",params.levels); docStart+=buffer;
          docStart+="</Levels>";

      docStart+="<Flare>";
        sprintf(buffer,"%f",params.flare); docStart+=buffer;
          docStart+="</Flare>";

      docStart+="<Leaves>";
        sprintf(buffer,"%d",params.leaves); docStart+=buffer;
          docStart+="</Leaves>";
      docStart+="<LeafScale>";
        sprintf(buffer,"%f",params.leafScale); docStart+=buffer;
          docStart+="</LeafScale>";
      docStart+="<LeafScaleX>";
        sprintf(buffer,"%f",params.leafScaleX); docStart+=buffer;
          docStart+="</LeafScaleX>";

      docStart+="<LeafQuality>";
        sprintf(buffer,"%f",params.leafQuality); docStart+=buffer;
          docStart+="</LeafQuality>";
      docStart+="<LeafDistrib>";
        sprintf(buffer,"%d",params.leafShapeRatio); docStart+=buffer;
          docStart+="</LeafDistrib>";
      docStart+="<LeafBend>";
        sprintf(buffer,"%f",params.leafBend); docStart+=buffer;
          docStart+="</LeafBend>";
      docStart+="<LeafScale>";
        sprintf(buffer,"%f",params.leafScale); docStart+=buffer;
          docStart+="</LeafScale>";
      docStart+="<PruneRatio>";
        sprintf(buffer,"%f",params.pruneRatio); docStart+=buffer;
          docStart+="</PruneRatio>";
      docStart+="<PrunePowerLow>";
        sprintf(buffer,"%f",params.prunePowerLow); docStart+=buffer;
          docStart+="</PrunePowerLow>";
      docStart+="<PrunePowerHigh>";
        sprintf(buffer,"%f",params.prunePowerHigh); docStart+=buffer;
          docStart+="</PrunePowerHigh>";
      docStart+="<PruneWidth>";
        sprintf(buffer,"%f",params.pruneWidth); docStart+=buffer;
          docStart+="</PruneWidth>";
      docStart+="<PruneWidthPeak>";
        sprintf(buffer,"%f",params.pruneWidthPeak); docStart+=buffer;
          docStart+="</PruneWidthPeak>";
 
      for (int i=0; i<params.levels; i++)
      {
        docLevel="<level level=\"";docLevel+=itoa(i,buffer,32);docLevel+="\">";
        if (i==0)
        {
          docLevel+="<Scale>";
            sprintf(buffer,"%f",params.trunk.scale); docStart+=buffer;
              docLevel+="</Scale>";
          docLevel+="<ScaleV>";
            sprintf(buffer,"%f",params.trunk.scaleV); docLevel+=buffer;
              docLevel+="</ScaleV>";
          docLevel+="<BaseSplits>";
            sprintf(buffer,"%f",params.trunk.baseSplits); docLevel+=buffer;
              docLevel+="</BaseSplits>";
          docLevel+="<BranchDist>";
            sprintf(buffer,"%f",params.trunk.dist); docLevel+=buffer;
              docLevel+="</BranchDist>";
        }
        docLevel+="<DownAngle>";
          sprintf(buffer,"%f",params.level[i].downAngle);docLevel+=buffer;
            docLevel+="</DownAngle>";
        docLevel+="<DownAngleV>";
          sprintf(buffer,"%f",params.level[i].downAngleV);docLevel+=buffer;
            docLevel+="</DownAngleV>";
        docLevel+="<Rotate>";
          sprintf(buffer,"%f",params.level[i].rotate); docLevel+=buffer;
            docLevel+="</Rotate>";
        docLevel+="<RotateV>";
          sprintf(buffer,"%f",params.level[i].rotateV); docLevel+=buffer;
            docLevel+="</RotateV>";
        docLevel+="<Branches>";
          sprintf(buffer,"%d",params.level[i].branches); docLevel+=buffer;
            docLevel+="</Branches>";
        docLevel+="<BranchDist>";
          sprintf(buffer,"%f",params.level[i].branchDist); docLevel+=buffer;
            docLevel+="</BranchDist>";
        docLevel+="<SegSplits>";
          sprintf(buffer,"%f",params.level[i].segSplits); docLevel+=buffer;
            docLevel+="</SegSplits>";
        docLevel+="<Taper>";
          sprintf(buffer,"%f",params.level[i].taper); docLevel+=buffer;
            docLevel+="</Taper>";
        docLevel+="<SplitAngle>";
          sprintf(buffer,"%f",params.level[i].splitAngle); docLevel+=buffer;
            docLevel+="</SplitAngle>";
        docLevel+="<SplitAngleV>";
          sprintf(buffer,"%f",params.level[i].splitAngleV); docLevel+=buffer;
            docLevel+="</SplitAngleV>";
        docLevel+="<Curve>";
          sprintf(buffer,"%f",params.level[i].curve); docLevel+=buffer;
            docLevel+="</Curve>";
        docLevel+="<CurveV>";
          sprintf(buffer,"%f",params.level[i].curveV); docLevel+=buffer;
            docLevel+="</CurveV>";
        docLevel+="<CurveRes>";
          sprintf(buffer,"%d",params.level[i].curveRes); docLevel+=buffer;
            docLevel+="</CurveRes>";        
        docLevel+="<CurveBack>";
          sprintf(buffer,"%f",params.level[i].curveBack); docLevel+=buffer;
            docLevel+="</CurveBack>";
        docLevel+="<Length>";
          sprintf(buffer,"%f",params.level[i].length); docLevel+=buffer;
            docLevel+="</Length>";
        docLevel+="<LengthV>";
          sprintf(buffer,"%f",params.level[i].lengthV); docLevel+=buffer;
            docLevel+="</LengthV>";
        docLevel+="</level>";
        docStart+=docLevel;
      }

      docStart+="</species>";
      docStart+="</opentree>";

      TiXmlDocument doc(filename);
      doc.Parse(docStart.c_str());
      if ( doc.Error() )
		  {
			  printf( "Error in %s: %s\n", doc.Value(), doc.ErrorDesc() );
			  return false;
		  }
           
		  doc.SaveFile(filename);
    return true;
    };
  };

  /// @}
}

#endif //_XMLSAVER_H_
