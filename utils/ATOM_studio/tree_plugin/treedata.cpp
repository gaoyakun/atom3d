#include "StdAfx.h"
#include "treedata.h"

#if defined(SUPPORT_BILLBOARD_TREE)

void initTreeData (TreeData &treeData)
{
	treeData.trunk.scale = 1.f;
	treeData.trunk.dist = 1.f;
	treeData.trunk.scaleV = 0.f;
	treeData.trunk.baseSplits = 0.f;

	treeData.level[0].branchDist = 1.f;
	treeData.level[0].branches = 1;
	treeData.level[0].curve = 0.f;
	treeData.level[0].curveBack = 0.f;
	treeData.level[0].curveRes = 6.f;
	treeData.level[0].curveV = 0.f;
	treeData.level[0].downAngle = 0.f;
	treeData.level[0].downAngleV = 0.f;
	treeData.level[0].length = 10.f;
	treeData.level[0].lengthV = 0.f;
	treeData.level[0].levelNumber = 0;
	treeData.level[0].rotate = 0.f;
	treeData.level[0].rotateV = 0.f;
	treeData.level[0].segSplits = 0.f;
	treeData.level[0].splitAngle = 0.f;
	treeData.level[0].splitAngleV = 0.f;
	treeData.level[0].taper = 1.f;

	treeData.shape = TD_HEMISPHERICAL;
	treeData.baseSize = 0.25f;
	treeData.scale = 1.f;
	treeData.scaleV = 0.f;
	treeData.levels = 1;
	treeData.ratio = 0.02f;
	treeData.ratioPower = 1.5f;
	treeData.lobes = 0;
	treeData.lobeDepth = 0.f;
	treeData.flare = 0.f;
	treeData.attractionUp = 0.f;
	treeData.pruneRatio = 0.f;
	treeData.prunePowerLow = 0.f;
	treeData.prunePowerHigh = 0.f;
	treeData.pruneWidth = 0.f;
	treeData.pruneWidthPeak = 0.f;
}

bool readTreeDataFromXML (ATOM_TiXmlElement *root, TreeData &treeData)
{
	if (!root)
	{
		return false;
	}

	initTreeData (treeData);

	ATOM_TiXmlElement *eRoot = root;

	ATOM_TiXmlElement *eShape = eRoot->FirstChildElement("Shape");
	if (eShape)
	{
		eShape->Attribute ("Value", &treeData.shape);
	}

	ATOM_TiXmlElement *eBaseSize = eRoot->FirstChildElement("BaseSize");
	if (eBaseSize)
	{
		double val = treeData.baseSize;
		eBaseSize->Attribute("Value", &val);
		treeData.baseSize = val;
	}

	ATOM_TiXmlElement *eScale = eRoot->FirstChildElement("Scale");
	if (eScale)
	{
		double val = treeData.scale;
		eScale->Attribute ("Value", &val);
		treeData.scale = val;
	}

	ATOM_TiXmlElement *eScaleV = eRoot->FirstChildElement("ScaleV");
	if (eScaleV)
	{
		double val = treeData.scaleV;
		eScaleV->Attribute("Value", &val);
		treeData.scaleV = val;
	}

	ATOM_TiXmlElement *eLevels = eRoot->FirstChildElement("LevelCount");
	if (eLevels)
	{
		eLevels->Attribute ("Value", &treeData.levels);
	}

	ATOM_TiXmlElement *eRatio = eRoot->FirstChildElement("Ratio");
	if (eRatio)
	{
		double val = treeData.ratio;
		eRatio->Attribute ("Value", &val);
		treeData.ratio = val;
	}

	ATOM_TiXmlElement *eRatioPower = eRoot->FirstChildElement("RatioPower");
	if (eRatioPower)
	{
		double val = treeData.ratioPower;
		eRatioPower->Attribute ("Value", &val);
		treeData.ratioPower = val;
	}

	ATOM_TiXmlElement *eLobes = eRoot->FirstChildElement("Lobes");
	if (eLobes)
	{
		eLobes->Attribute ("Value", &treeData.lobes);
	}

	ATOM_TiXmlElement *eLobeDepth = eRoot->FirstChildElement("LobeDepth");
	if (eLobeDepth)
	{
		double val = treeData.lobeDepth;
		eLobeDepth->Attribute ("Value", &val);
		treeData.lobeDepth = val;
	}

	ATOM_TiXmlElement *eFlare = eRoot->FirstChildElement("Flare");
	if (eFlare)
	{
		double val = treeData.flare;
		eFlare->Attribute ("Value", &val);
		treeData.flare = val;
	}

	ATOM_TiXmlElement *eAttractionUp = eRoot->FirstChildElement("AttractionUp");
	if (eAttractionUp)
	{
		double val = treeData.attractionUp;
		eAttractionUp->Attribute ("Value", &val);
		treeData.attractionUp = val;
	}

	ATOM_TiXmlElement *ePruneRatio = eRoot->FirstChildElement("PruneRatio");
	if (ePruneRatio)
	{
		double val = treeData.pruneRatio;
		ePruneRatio->Attribute ("Value", &val);
		treeData.pruneRatio = val;
	}

	ATOM_TiXmlElement *ePrunePowerLow = eRoot->FirstChildElement("PrunePowerLow");
	if (ePrunePowerLow)
	{
		double val = treeData.prunePowerLow;
		ePrunePowerLow->Attribute ("Value", &val);
		treeData.prunePowerLow = val;
	}

	ATOM_TiXmlElement *ePrunePowerHigh = eRoot->FirstChildElement("PrunePowerHigh");
	if (ePrunePowerHigh)
	{
		double val = treeData.prunePowerHigh;
		ePrunePowerHigh->Attribute ("Value", &val);
		treeData.prunePowerHigh = val;
	}

	ATOM_TiXmlElement *ePruneWidth = eRoot->FirstChildElement("PrunePowerWidth");
	if (ePruneWidth)
	{
		double val = treeData.pruneWidth;
		ePruneWidth->Attribute ("Value", &val);
		treeData.pruneWidth = val;
	}

	ATOM_TiXmlElement *ePruneWidthPeak = eRoot->FirstChildElement("PruneWidthPeak");
	if (ePruneWidthPeak)
	{
		double val = treeData.pruneWidthPeak;
		ePruneWidthPeak->Attribute ("Value", &val);
		treeData.pruneWidthPeak = val;
	}

	ATOM_TiXmlElement *eTrunk = eRoot->FirstChildElement("Trunk");
	if (eTrunk)
	{
		ATOM_TiXmlElement *eTrunkScale = eTrunk->FirstChildElement("Scale");
		if (eTrunkScale)
		{
			double val = treeData.trunk.scale;
			eTrunkScale->Attribute ("Value", &val);
			treeData.trunk.scale = val;
		}

		ATOM_TiXmlElement *eTrunkScaleV = eTrunk->FirstChildElement("ScaleV");
		if (eTrunkScaleV)
		{
			double val = treeData.trunk.scaleV;
			eTrunkScaleV->Attribute ("Value", &val);
			treeData.trunk.scaleV = val;
		}

		ATOM_TiXmlElement *eTrunkBaseSplits = eTrunk->FirstChildElement("BaseSplits");
		if (eTrunkBaseSplits)
		{
			double val = treeData.trunk.baseSplits;
			eTrunkBaseSplits->Attribute ("Value", &val);
			treeData.trunk.baseSplits = val;
		}
		
		ATOM_TiXmlElement *eTrunkDist = eTrunk->FirstChildElement("Dist");
		if (eTrunkDist)
		{
			double val = treeData.trunk.dist;
			eTrunkDist->Attribute ("Value", &val);
			treeData.trunk.dist = val;
		}
	}

	ATOM_TiXmlElement *eLevelsDesc = eRoot->FirstChildElement("Levels");
	if (eLevelsDesc)
	{
		ATOM_TiXmlElement *eLevel = 0;

		for (int i = 0; i < treeData.levels; ++i)
		{
			treeData.level[i].levelNumber = i;

			if (i == 0)
			{
				eLevel = eLevelsDesc->FirstChildElement("Level");
			}
			else
			{
				eLevel = eLevel->NextSiblingElement("Level");
			}

			if (!eLevel)
			{
				treeData.levels = i;
				break;
			}

			ATOM_TiXmlElement *eLevelDownAngle = eLevel->FirstChildElement("DownAngle");
			if (eLevelDownAngle)
			{
				double val = treeData.level[i].downAngle;
				eLevelDownAngle->Attribute ("Value", &val);
				treeData.level[i].downAngle = val;
			}
			
			ATOM_TiXmlElement *eLevelDownAngleV = eLevel->FirstChildElement("DownAngleV");
			if (eLevelDownAngleV)
			{
				double val = treeData.level[i].downAngleV;
				eLevelDownAngleV->Attribute ("Value", &val);
				treeData.level[i].downAngleV = val;
			}
			
			ATOM_TiXmlElement *eLevelRotate = eLevel->FirstChildElement("Rotate");
			if (eLevelRotate)
			{
				double val = treeData.level[i].rotate;
				eLevelRotate->Attribute ("Value", &val);
				treeData.level[i].rotate = val;
			}
			
			ATOM_TiXmlElement *eLevelRotateV = eLevel->FirstChildElement("RotateV");
			if (eLevelRotateV)
			{
				double val = treeData.level[i].rotateV;
				eLevelRotateV->Attribute ("Value", &val);
				treeData.level[i].rotateV = val;
			}
			
			ATOM_TiXmlElement *eLevelBranches = eLevel->FirstChildElement("Branches");
			if (eLevelBranches)
			{
				eLevelBranches->Attribute ("Value", &treeData.level[i].branches);
			}
			
			ATOM_TiXmlElement *eLevelBranchDist = eLevel->FirstChildElement("BranchDist");
			if (eLevelBranchDist)
			{
				double val = treeData.level[i].branchDist;
				eLevelBranchDist->Attribute ("Value", &val);
				treeData.level[i].branchDist = val;
			}
			
			ATOM_TiXmlElement *eLevelLength = eLevel->FirstChildElement("Length");
			if (eLevelLength)
			{
				double val = treeData.level[i].length;
				eLevelLength->Attribute ("Value", &val);
				treeData.level[i].length = val;
			}
			
			ATOM_TiXmlElement *eLevelLengthV = eLevel->FirstChildElement("LengthV");
			if (eLevelLengthV)
			{
				double val = treeData.level[i].lengthV;
				eLevelLengthV->Attribute ("Value", &val);
				treeData.level[i].lengthV = val;
			}
			
			ATOM_TiXmlElement *eLevelTaper = eLevel->FirstChildElement("Taper");
			if (eLevelTaper)
			{
				double val = treeData.level[i].taper;
				eLevelTaper->Attribute ("Value", &val);
				treeData.level[i].taper = val;
			}
			
			ATOM_TiXmlElement *eLevelSegSplits = eLevel->FirstChildElement("SegSplits");
			if (eLevelSegSplits)
			{
				double val = treeData.level[i].segSplits;
				eLevelSegSplits->Attribute ("Value", &val);
				treeData.level[i].segSplits = val;
			}
			
			ATOM_TiXmlElement *eLevelSplitAngle = eLevel->FirstChildElement("SplitAngle");
			if (eLevelSplitAngle)
			{
				double val = treeData.level[i].splitAngle;
				eLevelSplitAngle->Attribute ("Value", &val);
				treeData.level[i].splitAngle = val;
			}
			
			ATOM_TiXmlElement *eLevelSplitAngleV = eLevel->FirstChildElement("SplitAngleV");
			if (eLevelSplitAngleV)
			{
				double val = treeData.level[i].splitAngleV;
				eLevelSplitAngleV->Attribute ("Value", &val);
				treeData.level[i].splitAngleV = val;
			}
			
			ATOM_TiXmlElement *eLevelCurveRes = eLevel->FirstChildElement("CurveRes");
			if (eLevelCurveRes)
			{
				eLevelCurveRes->Attribute ("Value", &treeData.level[i].curveRes);
			}
			
			ATOM_TiXmlElement *eLevelCurve = eLevel->FirstChildElement("Curve");
			if (eLevelCurve)
			{
				double val = treeData.level[i].curve;
				eLevelCurve->Attribute ("Value", &val);
				treeData.level[i].curve = val;
			}
			
			ATOM_TiXmlElement *eLevelCurveBack = eLevel->FirstChildElement("CurveBack");
			if (eLevelCurveBack)
			{
				double val = treeData.level[i].curveBack;
				eLevelCurveBack->Attribute ("Value", &val);
				treeData.level[i].curveBack = val;
			}
			
			ATOM_TiXmlElement *eLevelCurveV = eLevel->FirstChildElement("CurveV");
			if (eLevelCurveV)
			{
				double val = treeData.level[i].curveV;
				eLevelCurveV->Attribute ("Value", &val);
				treeData.level[i].curveV = val;
			}
		}
	}
	else
	{
		treeData.levels = 0;
	}

	return true;
}

#endif
