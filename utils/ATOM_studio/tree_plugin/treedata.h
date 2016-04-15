#ifndef __ATOM3D_STUDIO_TREEDATA_H
#define __ATOM3D_STUDIO_TREEDATA_H

#if _MSC_VER > 1000
# pragma once
#endif

#if defined(SUPPORT_BILLBOARD_TREE)

#define		TD_CONICAL				0
#define		TD_SPHERICAL			1
#define		TD_HEMISPHERICAL		2
#define		TD_CYLINDRICAL			3
#define		TD_TAPERED_CYLINDRICAL	4
#define		TD_FLAME				5
#define		TD_INVERSE_CONICAL		6
#define		TD_TEND_FLAME			7
#define		TD_ENVELOPE				8

struct Trunk
{
	// trunk scale
	float scale;

	// variation of trunk scale
	float scaleV;

	// stem splits at base of trunk
	// base splits are the stem splits at the top of the first trunk segment.
	// So with baseSplits=2 you get a trunk splitting into three parts. Other then with 0 segSplits the clones are evenly distributed over the 360,
	// so, if you want to use splitting, you should use baseSplits for the first splitting  to get a circular stem distribution
	// min: 0 default: 0
	float baseSplits;

	float dist;
};

struct Level
{
	int levelNumber;

	// The angle between the stem and it's parent
	// min: -179.99999 max: 179.99999 default: 0.0
	float downAngle;

	// downAngle variation
	// Using a negative value, the downAngleV is variated over the length of the stem, so that the lower branches have a bigger downAngle than the higher branches.
	// min: -179.99999 max: 179.99999 default: 0.0
	float downAngleV;

	// spiraling angle
	// This is the angle, the branches are rotating around the parent.
	// if rotate is negative the branches are located on alternating sides of the parent.
	// min: -360.0 max: 360.0 default: 0.0
	float rotate;

	// spiraling angle varation
	// min: -360.0 max: 360.0 default: 0.0
	float rotateV;

	// The maximal number of branches on a parent stem.
	// The number of branches are reduced proportional to the relative length of their parent
	// min: 0 default: 1
	int branches;

	// An additional parameter of arbaro.
	// It influences the distribution of branches over a segment of the parent stem.
	// With 1.0 you get evenly distribution of branches like in the original model.
	// With 0.0 all branches grow from the segments base like for conifers.
	// min: 0.0 max: 1.0 default: 0.0

	float branchDist;  //Used by Arbaro
	// fractional trunk scaling
	// with scale=10 and length=0.8 the length of the trunk will be 8m.
	// min: 1.0E-7 max: INF default: 1.0
	float length;

	// variation of fractional trunk scaling
	// min: 0.0 max: INF default: 0.0
	float lengthV;

	// cross-section scaling
	// 0: non-tapering cylinder
	// 1: taper to a point (cone)
	// 2: taper to a spherical end
	// 3: periodic tapering (concatenated spheres)
	// can use fractional values to get intermediate result
	// min: 0.0 max: 2.9999999 default: 1.0
	float taper;

	// stem split per segment
	// This parameter determines how much splits per segment occures.
	// Normally  you would use a value between 0.0 and 1.0. A value of 0.5 means a split at every second segment.
	// If you use splitting for the trunk you should use baseSplits for the first split, otherwise the tree will tend to one side.
	// min: 0.0 max: INF default: 0.0
	float segSplits;

	// splitting angle
	// It is the vertical splitting angle. A horizontal diverging angle will be added too, but this one you cannot influence with parameters.
	// The declination of the splitting branches won't exceed the splitting angle.
	// min: 0.0 max: 180.0 default: 0.0
	float splitAngle;

	// splitting angle variation
	// min: 0.0 max: 180.0 default: 0.0
	float splitAngleV;

	// curvature resolution
	// normally you will use higher values for the first levels, and low values for the higher levels
	// min: 1 default: 3
	int curveRes;

	// curving angle
	// It is the angle the branches are declined over their whole length.
	// If curveBack is used, the curving angle is distributed only over the first half of the stem.
	// min: -INF max: INF default: 0.0
	float curve;

	// curving angle upper stem half
	// Using this value you can give the stem an S-like shape.
	// The first half of the stem the curve value is applied, the second half the curveBack value is applied.
	// It's also possible to give both parameters the same sign to get different curving over the stem length, instead of S-shape
	// min: -INF max: INF default: 0.0
	float curveBack;

	// curving angle variation
	// A negative value means helical curvature.
	// min: -90 max: INF default: 0.0
	float curveV;
};

struct TreeData
{
	Trunk trunk;
	Level level[5];

	// shape of the tree
	// min: 0 max: 8 default: 0
	int shape;

	// fractional branchless area at tree base
	// 0.0: means branches begin on the bottom of the tree
	// 0.5: means half of the trunk is branchless
	// 1.0: branches grow out from the peak of the trunk only
	// min: 0.0 max: 1.0 default: 0.25
	float baseSize;

	// average tree height in meters
	// min: 1.0E-6 max: INF default: 10.0
	float scale;

	// variation of tree height in meters
	// min: 0.0 max: INF default: 0.0
	float scaleV;

	// levels of recursion
	// min: 0 max: 9 default: 3
	int levels;

	// trunk radius/length ratio
	// min: 1.0E-6 max: INF default: 0.05
	float ratio;

	// radius reduction
	// 1.0 means the radius decreases linearly with decreasing stem length
	// 2.0 means it decreases with the second power
	// 0.0 means radius is the same as parent radius(t.e. it doesn't depend of the length)
	// -1.0 means the shorter the stem the thicker it is (radius = parent radius * 1 / length)
	// note: the radius of a stem cannot be greater than the parent radius at the stem offset
	// min: -INF max: INF default: 1.0
	float ratioPower;

	// sinusolidal cross-section variation
	// This isn't supported for cones output, but for mesh only
	// min: 0.0 default: 0.0
	int lobes;

	// amplitude of cross-section variation
	// min: 0.0 max: INF  default: 0.0
	float lobeDepth;

	// exponential expansion at base of tree
	// 0.0 means base radius is used at trunk base
	// 1.0 means trunk base is twice as thick as it's base radius
	// min: -1.0 max: INF default: 0.5
	float flare;

	// upward/downward growth tendency
	// the tendency of stems with level>=2 to grow upwards
	// A value of 1.0 for a horizontal stem means the last segment should point upwards.
	// Greater values means earlier reaching of upward direction. Value of 10 and greater could cause overcorrection resulting in a snaking oscillation.
	// Can use a negative value to simulate the weeping willow.
	// min: -INF max: INF default: 0.0
	float attractionUp;

	// fractional effect of pruning
	// 1.0 means all branches are inside the envelope.
	// 0.0 means no pruning
	// min: 0.0 max: 1.0 default 0.0
	float pruneRatio;

	// describes the envelope curve below the peak
	// A value of 1 means linear decreasing. Higher value means concave, lower values convex curve
	// This value is used for the shape "envelope" too, even if PruneRatio is off
	// min: 0.0 max: INF default: 0.5
	float prunePowerLow;

	// describes the envelope curve above the peak
	// A value of 1 means linear decreasing. Higher value means concave, lower values convex curve
	// This value is used for the shape "envelope" too, even if PruneRatio is off
	// min: 0.0 max: INF default: 0.5
	float prunePowerHigh;

	// width of envelope peak
	// A value of 0.5 means the tree is half as wide as high
	// This value is used for the shape "envelope" too, even if PruneRatio is off
	// min: 0.0 max: 1.0 default: 0.5
	float pruneWidth;

	// position of envelope peak
	// A value of 0.5 means upper part and lower part of the envelope have the same height
	// This value is used for the shape "envelope" too, even if PruneRatio is off
	// min: 0.0 max: 1.0 default: 0.5
	float pruneWidthPeak;
};

void initTreeData (TreeData &treeData);
bool readTreeDataFromXML (ATOM_TiXmlElement *root, TreeData &treeData);

#endif

#endif // __ATOM3D_STUDIO_TREEDATA_H
