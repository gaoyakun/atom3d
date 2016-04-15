//  #**************************************************************************
//  #
//  #    $Id: arbarostem.cpp,v 1.2 2005/08/07 16:34:33 kirchdorfer Exp $  
//  #             - Stem class - here is most of the logic of 
//  #               the tree generating algorithm
//  #
//  #    Copyright (C) 2003  Wolfram Diestel
//  #    Copyright (C) 2005  Pascal Kirchdorfer (porting to c++)
//  #
//  #    This program is free software; you can redistribute it and/or modify
//  #    it under the terms of the GNU General Public License as published by
//  #    the Free Software Foundation; either version 2 of the License, or
//  #    (at your option) any later version.
//  #
//  #    This program is distributed in the hope that it will be useful,
//  #    but WITHOUT ANY WARRANTY; without even the implied warranty of
//  #    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  #    GNU General Public License for more details.
//  #
//  #    You should have received a copy of the GNU General Public License
//  #    along with this program; if not, write to the Free Software
//  #    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//  #
//  #    Send comments and bug fixes to diestel@steloj.de
//  #
//  #**************************************************************************/

#include "arbarostem.h"
#include "arbarotree.h"
#include "arbarosegment.h"
#include "arbaroleaf.h"

#include <math.h>

namespace Arbaro
{

  const float Stem::MIN_STEM_LEN=0.0005f; 
  const float Stem::MIN_STEM_RADIUS=MIN_STEM_LEN/10; 

  Stem::Stem(Tree* tree, Stem* growsOutOf, int stlev, otMatrix trf, float offs)
  : tree(tree), transf(trf), offset(offs), par(tree->params), 
    lpar(par.level[min(stlev,3)]), stemlevel(stlev)
  {
    splitErrorValue = 0;
    substemErrorValue = 0;
    leavesErrorValue = 0;

    parent = 0;
    clonedFrom = 0;

    if (growsOutOf != 0)
    {
      if (growsOutOf->stemlevel<stemlevel)
      {
        parent = growsOutOf;
      }
      else 
      {
        clonedFrom = growsOutOf;
        parent = growsOutOf->parent;
      }
    }

    // inialize other variables
    leavesPerSegment = 0;
    splitCorrection = 0;
    pruneTest = false; // flag used for pruning
  }

  Stem::~Stem()
  {
    clones.delAll();
    substems.delAll();
    segments.delAll();
    leaves.delAll();
  }

  Stem* Stem::clone(otMatrix& trf, int start_segm)
  {
    // creates a clone stem with same atributes as this stem
    Stem* clone = new Stem(tree,this,stemlevel,trf,offset);
    clone->segmentLength = segmentLength;
    clone->segmentCount = segmentCount;
    clone->length = length;
    clone->baseRadius = baseRadius;
    clone->splitCorrection = splitCorrection; 
    clone->pruneTest = pruneTest;
    
    if (! pruneTest) 
    {
      clone->lengthChildMax = lengthChildMax;
      clone->substemsPerSegment = substemsPerSegment;
      clone->substemRotangle=substemRotangle+180;
      clone->leavesPerSegment=leavesPerSegment;
    }
    return clone;
  }

  void Stem::make() {
  	
	  // makes the stem with all its segments, substems, clones and leaves
	  segmentCount = lpar.curveRes;
	  length = stemLength();
	  segmentLength = length/lpar.curveRes;
	  baseRadius = stemBaseRadius();
	  if (stemlevel==0) {
		  //float baseWidth = max(baseRadius,stemRadius(0));
		  //minMaxTest(otVector3(baseWidth,baseWidth,0));
	  }
  	
	  //DBG("Stem.make(): len: "+length+" sgm_cnt: "+ segmentCount+" base_rad: "+baseRadius);
  	
	  // FIXME: should pruning occur for the trunk too?
	  if (stemlevel>0 && par.pruneRatio > 0) {
		  pruning();
	  }
  	
	  // FIXME: if length<=MIN_STEM_LEN the stem object persists here but without any segments
	  // alternatively make could return an error value, the invoking function
	  // then had to delete this stem
	  if (length > MIN_STEM_LEN && baseRadius > MIN_STEM_RADIUS)
	  {
		  prepareSubstemParams();
		  makeSegments(0,segmentCount);
	  } else {
		  //DBG("length "+length+" (after pruning?) to small - stem not created");
	  }
  	
  //		tree.minMaxTest(maxPoint);
  //		tree.minMaxTest(minPoint);
  }

  void Stem::pruning() {
	  //return; //disable pruning
	  // save random state, split and len values
	  float splitcorr = splitCorrection;
	  float origlen = length;
	  //float seglen = segmentLength;
  	
	  // start pruning
	  pruneTest = true;
  	
	  // test length
	  int segm = makeSegments(0,segmentCount);
  	
	  while (segm >= 0 && length > 0.001*par.scale_tree) {
  		
		  // restore random state and split values
		  splitCorrection = splitcorr;
  		
		  // delete segments and clones
		  clones.removeAll();
		  segments.removeAll();
  		
		  // get new length
		  float minlen = length/2; // shorten max. half of length
		  float maxlen = length-origlen/15; // shorten min of 1/15 of orig. len
		  length = min(max(segmentLength*segm,minlen),maxlen);
  		
		  // calc new values dependent from length
		  segmentLength = length/lpar.curveRes;
		  baseRadius = stemBaseRadius();
  		
		  //if (length>MIN_STEM_LEN && baseRadius < MIN_STEM_RADIUS)
			  //System.err.println("WARNING: stem radius ("+baseRadius+") too small for stem "+getTreePosition());
  		
		  // test once more
		  if (length > MIN_STEM_LEN) segm = makeSegments(0,segmentCount);
	  }
	  // this length fits the envelope, 
	  // diminish the effect corresp. to PruneRatio
	  length = origlen - (origlen-length)*par.pruneRatio;
  	
	  // restore random state and split values
	  splitCorrection = splitcorr;
	  // delete segments and clones
	  clones.removeAll();
	  segments.removeAll();
	  pruneTest = false;
	  //self.DBG("PRUNE-ok: len: %f, segm: %d/%d\n"%(self.length,segm,self.segment_cnt))
  }

  float Stem::stemLength() {
	  if (stemlevel == 0) { // trunk
		  return (lpar.length + calcVariance(lpar.lengthV)) * par.scale_tree;
	  } else if (stemlevel == 1) {
		  float parlen = parent->length;
		  float baselen = par.baseSize*par.scale_tree;
		  float ratio  = (parlen-offset)/(parlen-baselen);
		  //DBG("Stem.stem_length(): parlen: "+parlen+" offset: "+offset+" baselen: "+baselen+" ratio: "+ratio);
		  return parlen * parent->lengthChildMax * getShapeRatio(ratio, par.shape);
	  } else { // higher levels
		  return parent->lengthChildMax*(parent->length-0.6f*offset);
	  }
  }

  // makes the segments of the stem
  int Stem::makeSegments(int start_seg,int end_seg) 
  {
    otMatrix trf = transf;
    
    for (int s=start_seg; s<=end_seg; s++) 
    {
      // curving
      newDirection(trf,s);
      
      // segment radius
      float rad1 = stemRadius(s*segmentLength);
      
      // create new segment
      Segment* segment = new Segment(trf,rad1);
      segments.add(segment);
      
      // create substems
      if (! pruneTest && lpar.levelNumber<par.levels-1) 
      {
        makeSubstems(s, trf, segment);
      }
      
      if (! pruneTest && lpar.levelNumber==par.levels-1 && par.leaves!=0) 
      {
        makeLeaves(s, trf, segment);
      }

      trf.translate(trf.getZ()*(segmentLength));

      // test if too long
      if (pruneTest && ! isInsideEnvelope(trf.getTranslation())) 
      {
        return s;
      }
      
      // splitting (create clones)
      if (s<end_seg-1) 
      {
        int segm = makeClones(trf,s);
        // trf is changed by make_clones prune test - clone not inside envelope
        if (segm>=0) 
        {
	  return segm;
        }
      }
    }
    
    return -1;
  }

  bool Stem::isInsideEnvelope(otVector3 vector) {
	  float r = sqrt(vector.X*vector.X + vector.Y*vector.Y);
	  float ratio = (par.scale_tree - vector.Z)/(par.scale_tree*(1-par.baseSize));
	  return (r/par.scale_tree) < (par.pruneWidth * getShapeRatio(ratio,8));
  }

  void Stem::newDirection(otMatrix& trf, int nsegm) {
	  // next segments direction
  	
	  // The first segment shouldn't get another direction 
	  // down and rotation angle shouldn't be falsified 
	  if (nsegm == 0) return;
  	
	  //TRF("Stem.new_direction() before curving",trf);
  	
	  // get curving angle
	  float delta;
	  if (lpar.curveBack==0) {
		  delta = lpar.curve / lpar.curveRes;
  		
	  } else {
		  if (nsegm < (lpar.curveRes+1)/2) {
			  delta = lpar.curve*2 / lpar.curveRes;
		  } else {
			  delta = lpar.curveBack*2 / lpar.curveRes;
		  }
	  }
	  delta += splitCorrection;
	  //DBG("Stem.new_direction(): delta: "+delta);
	  trf.lrotXDegree(delta);
  	
  	
	  // With Weber/Penn the orientation of the x- and y-axis 
	  // shouldn't be disturbed (maybe, because proper curving relies on this)
	  // so may be such random rotations shouldn't be used, instead nCurveV should
	  // add random rotation to rotx, and rotate nCurveV about the tree's z-axis too?
  	
	  // add random rotation about z-axis
	  if (lpar.curveV > 0) {
		  //    		if (nsegm==0 && stemlevel==0) { // first_trunk_segment
		  //    			// random rotation more moderate
		  //    			delta = (fabsf(lpar.var(lpar.nCurveV)) - 
		  //    					fabsf(lpar.var(lpar.nCurveV)))
		  //						/ lpar.nCurveRes;
		  //    		}	else {
		  // full random rotation
		  delta = calcVariance(lpar.curveV)/lpar.curveRes;
		  //    		}
		  // self.DBG("curvV (delta): %s\n" % str(delta))
		  float rho = PI+calcVariance(PI);
                  trf.lrotateDegree(delta,otVector3(1,0,0).rotZRad(rho));
	  }  
	  //TRF("Stem.new_direction() after curving",trf);
  	
	  // attraction up/down
	  if (par.attractionUp != 0 && stemlevel>=2) {
		  //Normalizing 'should' not be needed, in some cases, however, it is :/
		  float declination = acos(trf.getZ().normalize().Z);
                  if (declination != declination)
                    return;
  		
		  // 			I don't see, why we need orientation here, may be this avoids
		  //          attraction of branches with the x-Axis up and thus avoids
		  //			twisting (see below), but why branches in one direction should
		  //			be attracted, those with another direction not, this is unnaturally:
		  //    		float orient = acos(trf.getY().getZ());
		  //    		float curve_up_orig = par.AttractionUp * declination * cos(orient)/lpar.nCurveRes; 
  		
		  // FIXME: devide by (lpar.nCurveRes-nsegm) if last segment
		  // should actually be vertical 
		  float curve_up = par.attractionUp * 
		  fabsf(declination * sin(declination)) / lpar.curveRes;
  		
		  otVector3 z = trf.getZ();
		  // FIXME: the mesh is twisted for high values of AttractionUp
		  trf.grotateDegree(-curve_up*180/PI,otVector3(-z.Y,z.X,0));
		  // trf = trf.rotx(curve_up*180/PI);
	  }
	  return;
  }

  float Stem::stemBaseRadius() {
	  if (stemlevel == 0) { // trunk
		  // radius at the base of the stem
		  // I think nScale+-nScaleV should applied to the stem radius but not to base radius(?)
		  return length * par.ratio; // * par._0Scale; 
		  //+ var(par._0ScaleV))
	  } else {
		  // max radius is the radius of the parent at offset
		  float max_radius = parent->stemRadius(offset);
  		
		  // FIXME: RatioPower=0 seems not to work here
		  float radius = parent->baseRadius * pow(length/parent->length,par.ratioPower);
		  return min(radius,max_radius);
	  }
  }

  float Stem::stemRadius(float h) {
	  //DBG("Stem.stem_radius("+h+") base_rad:"+baseRadius);
  	
	  float angle = 0; //FIXME: add an argument "angle" for Lobes, 
	  // but at the moment Lobes are calculated later in mesh creation
  	
	  // gets the stem width at a given position within the stem
	  float Z = min(h/length,1.0f); // min, to avoid rounding errors
	  float taper = lpar.taper;
  	
	  float unit_taper=0;
	  if (taper <= 1) {
		  unit_taper = taper;
	  } else if (taper <=2) {
		  unit_taper = 2 - taper;
	  }
  	
	  float radius = baseRadius * (1 - unit_taper * Z);
  	
	  // spherical end or periodic tapering
	  float depth;
	  if (taper>1) {
		  float Z2 = (1-Z)*length;
		  if (taper<2 || Z2<radius) {
			  depth = 1;
		  } else {
			  depth=taper-2;
		  }
		  float Z3;
		  if (taper<2) {
			  Z3=Z2;
		  } else {
			  Z3=fabsf(Z2-2*radius*(int)(Z2/2/radius+0.5));
		  }
		  if (taper>2 || Z3<radius) {
			  radius=(1-depth)*radius+depth*sqrt(radius*radius-(Z3-radius)*(Z3-radius));
			  //  self.DBG("TAPER>2: Z2: %f, Z3: %f, depth: %f, radius %f\n"%(Z2,Z3,depth,radius))
		  }	  
	  }	    
	  if (stemlevel==0) { 
		  // add flaring (thicker stem base)
		  if (par.flare != 0) {
			  float y = max(0.0f,1-8*Z);
			  float flare = 1 + par.flare * (pow(100.0f,y) - 1) / 100.0f;
			  //DBG("Stem.stem_radius(): Flare: "+flare+" h: "+h+" Z: "+Z);
			  radius = radius*flare;
		  }
		  // add lobes - this is done in mesh creation not here at the moment
		  if (par.lobes>0 && angle!=0) {
			  // FIXME: use the formular from Segment.create_mesh_section() instead
			  radius = radius*(1.0f+par.lobeDepth*sin(par.lobes*angle*PI/180));
		  }
  		
		  // multiply with 0Scale;
		  // 0ScaleV is applied only in mesh creation (Segment.create_section_meshpoints)
		  radius = radius*par.trunk.scale;
	  }
  	
	  //DBG("Stem.stem_radius("+h+") = "+radius);
  	
	  return radius;
  }

  void Stem::prepareSubstemParams() {
	  //int level = min(stemlevel+1,3);
	  Level lpar_1 = par.level[min(stemlevel+1,3)];
  	
	  // maximum length of a substem
	  lengthChildMax = lpar_1.length+calcVariance(lpar_1.lengthV);
  	
	  // maximum number of substems
	  float stems_max = (float)lpar_1.branches;
  	
	  // actual number of substems and substems per segment
	  float substem_cnt;
	  if (stemlevel==0) {
		  substem_cnt = stems_max;
		  substemsPerSegment = substem_cnt / (float)segmentCount / (1-par.baseSize);
		  //DBG("Stem.prepare_substem_params(): stems_max: "+ substem_cnt 
		  //		+ " substems_per_segment: " + substemsPerSegment);
	  } else if (stemlevel==1) {
		  substem_cnt = (stems_max * 
				  (0.2f + 0.8f*length/parent->length/parent->lengthChildMax));
		  substemsPerSegment = substem_cnt / (float)segmentCount;
		  //DBG("Stem.prepare_substem_params(): substem_cnt: "+ substem_cnt 
		  //		+ " substems_per_segment: " + substemsPerSegment);
	  } else {
		  substem_cnt = (stems_max * (1.0f - 0.5f * offset/parent->length));
		  substemsPerSegment = substem_cnt / (float)segmentCount;
	  }
	  substemRotangle = 0;
  	
	  // how much leaves for this stem - not really a substem parameter
	  if (lpar.levelNumber == par.levels-1) {
		  leavesPerSegment = leavesPerBranch() / segmentCount;
	  }
  }

  float Stem::leavesPerBranch() {
	  // calcs the number of leaves for a stem
	  if (par.leaves==0) return 0;
	  if (stemlevel == 0) {
		  // FIXME: maybe set Leaves=0 when Levels=1 in Params.prepare()
		  //System.err.println("WARNING: trunk cannot have leaves, no leaves are created");
		  return 0;
	  }
  	
	  return (float(abs(par.leaves))
			  * getShapeRatio(offset/parent->length,par.leafShapeRatio) 
			  * par.leafQuality);
  }

  void Stem::makeSubstems(int index, otMatrix seg_trf, Segment* segment) {
	  // creates substems for the current segment
	  Level lpar_1 = par.level[min(stemlevel+1,3)];
  	
	  //DBG("Stem.make_substems(): substems_per_segment "+substemsPerSegment);
  	
	  float subst_per_segm;
	  float offs;

	  if (stemlevel>0) {
		  // full length of stem can have substems
		  subst_per_segm = substemsPerSegment;
  		
		  if (index==0) {
			  offs = parent->stemRadius(offset)/segmentLength;
		  } else { offs = 0; }
  		
	  } else if (index*segmentLength > par.baseSize*length) {
		  // segment is fully out of the bare trunk region => normal nb of substems
		  subst_per_segm = substemsPerSegment;
		  offs = 0;
	  } else if ((index+1)*segmentLength <= par.baseSize*length) {
		  // segment is fully part of the bare trunk region => no substems
		  return;
	  } else {
		  // segment has substems in the upper part only
		  offs = (par.baseSize*length - index*segmentLength)/segmentLength;
		  subst_per_segm = substemsPerSegment*(1-offs);
	  }	
  	
	  // how many substems in this segment
	  int substems_eff = (int)(subst_per_segm + substemErrorValue+0.5);
  	
	  // adapt error value
	  substemErrorValue -= float(substems_eff - subst_per_segm);
  	
	  if (substems_eff <= 0) return;
  	
	  //DBG("Stem.make_substems(): substems_eff: "+substems_eff);
  	
	  // what distance between the segements substems
	  float dist = (1.0f-offs)/substems_eff*lpar_1.branchDist;
	  float distv = dist*0.25f; // lpar_1.nBranchDistV/2;
  	
	  //DBG("Stem.make_substems(): offs: "+offs+" dist: "+dist+" distv: "+distv);
  	
	  for (int s=0; s<substems_eff; s++) {
		  // where on the segment add the substem
		  float where = offs+dist/2+s*dist+calcVariance((float)distv);
  		
		  //offset from stembase
		  float offset = (index + where) * segmentLength;
  		
		  //DBG("Stem.make_substems(): offset: "+ offset+" segminx: "+segment.index
		  //		+" where: "+where+ " seglen: "+segmentLength);
  		
		  otMatrix trf = seg_trf;

                  substemDirection(trf,offset);

                  trf.translate(seg_trf.getZ()*(where*segmentLength));
  		
		  // create new substem
		  Stem* substem = new Stem(tree,this,stemlevel+1,trf,offset);
		  substem->make();
		  substems.add(substem);
	  }
  }

  void Stem::substemDirection(otMatrix& trf, float offset) {
	  Level lpar_1 = par.level[min(stemlevel+1,3)];
	  //lev = min(level+1,3);
  	
	  // get rotation angle
	  float rotangle;
	  if (lpar_1.rotate>=0) { // rotating substems
		  substemRotangle = (substemRotangle + lpar_1.rotate+calcVariance(lpar_1.rotateV)+360);
                  while (substemRotangle>=360.0) substemRotangle -= 360;
                  while (substemRotangle<0.0) substemRotangle += 360;
		  rotangle = substemRotangle;
	  } else { // alternating substems
		  if (fabsf(substemRotangle) != 1) substemRotangle = 1;
		  substemRotangle = -substemRotangle;
		  rotangle = substemRotangle * (180+lpar_1.rotate+calcVariance(lpar_1.rotateV));
	  }
  	
	  // get downangle
	  float downangle;
	  if (lpar_1.downAngleV>=0) {
		  downangle = lpar_1.downAngle+calcVariance(lpar_1.downAngleV);
	  } else {
		  float len = (stemlevel==0)? length*(1-par.baseSize) : length;
		  downangle = lpar_1.downAngle +
		  lpar_1.downAngleV*(1 - 2 * getShapeRatio((length-offset)/len,0));
	  }  
	  //DBG("Stem.substem_direction(): down: "+downangle+" rot: "+rotangle);
          Stem::rotxzDegree(trf,downangle,rotangle);
  }

  void Stem::makeLeaves(int index, otMatrix seg_trf, Segment* segment) {
	  // creates leaves for the current segment
  	
	  if (par.leaves > 0) { // ### NORMAL MODE, leaves along the stem
		  // how many leaves in this segment
		  float leaves_eff = (leavesPerSegment + leavesErrorValue+0.5f);
  		
		  // adapt error value
		  leavesErrorValue -= float(leaves_eff - leavesPerSegment);
  		
		  if (leaves_eff <= 0) return;
  		
		  float offs;
		  if (index==0) {
			  offs = parent->stemRadius(offset)/segmentLength;
		  } else {
			  offs = 0;
		  }
  		
		  // what distance between the leaves
		  float dist = (1.0f-offs)/leaves_eff;
  		
		  for (int s=0; s<leaves_eff; s++) {
			  // where on the segment add the leaf
  			
			  // FIXME: may be use the same distribution method (BranchDist) as for substems?
			  float where = offs+dist/2+s*dist+calcVariance((float)dist/2.0f);
  			
			  // offset from stembase
			  float loffs = (index+where)*segmentLength;
			  // get a new direction for the leaf
			  otMatrix trf = seg_trf;
                          substemDirection(trf,loffs);
			  // translate it to its position on the stem
			  trf.translate(seg_trf.getZ()*(where*segmentLength));
  			
			  // create new leaf
			  Leaf* leaf = new Leaf(par,trf);
			  leaf->make();
			  leaves.add(leaf);
  			
		  }
	  }
  	
	  // ##### FAN MOD, leaves placed in a fan at stem end
	  else if (par.leaves<0 && index == segmentCount-1) {
  		
		  Level lpar_1 = par.level[min(stemlevel+1,3)];
		  int cnt = (int)(leavesPerBranch()+0.5);
  		
		  otMatrix trf = seg_trf;
                  trf.translate(seg_trf.getZ()*(segmentLength));
		  float distangle = lpar_1.rotate/cnt;
		  float varangle = lpar_1.rotateV/cnt;
		  float downangle = lpar_1.downAngle;
		  float vardown = lpar_1.downAngleV;
		  float offsetangle = 0;
		  // use different method for odd and even number
		  if (cnt%2 == 1) {
			  // create one leaf in the middle
			  Leaf* leaf = new Leaf(par,trf/*,segmentCount*segmentLength*/);
			  leaf->make();
			  leaves.add(leaf);
			  offsetangle = distangle;
		  } else {
			  offsetangle = distangle/2;
		  }
		  // create leaves left and right of the middle
		  for (int s=0; s<cnt/2; s++) {
			  for (int rot=1; rot >=-1; rot-=2) {
				  otMatrix transf1 = trf;
                                  transf1.lrotYDegree(rot*(offsetangle+s*distangle
						  +calcVariance((float)varangle)));
				  transf1.lrotXDegree(downangle+calcVariance((float)vardown));
				  Leaf* leaf = new Leaf(par,transf1/*,segmentCount*segmentLength*/);
				  leaf->make();
				  leaves.add(leaf);
			  }
		  }
	  }
  }

  int Stem::makeClones(otMatrix& trf,int nseg) 
  {
    // splitting
    int seg_splits_eff;
    if (stemlevel==0 && nseg==0 && par.trunk.baseSplits>0) 
    {
      seg_splits_eff = (int)par.trunk.baseSplits;
    }
    else
    {
      // how many clones?
      float seg_splits = lpar.segSplits;
      seg_splits_eff = (int)(seg_splits+splitErrorValue+0.5);
      
      // adapt error value
      splitErrorValue -= (float)(seg_splits_eff - seg_splits);
    }
    
    if (seg_splits_eff<1) return -1;
    
    float s_angle = 360.0f/(seg_splits_eff+1);
    
    // make clones
    // if seg_splits_eff > 0:
    for (int i=0; i<seg_splits_eff; i++) 
    { 
      // copy params
      Stem* clone = this->clone(trf,nseg+1);
      
      // change the direction for the clone
      clone->transf = trf;
      clone->split(clone->transf,s_angle*(1+i),nseg,seg_splits_eff);
      
      // make segments etc. for the clone
      int segm = clone->makeSegments(nseg+1,clone->segmentCount);
      if (segm>=0) // prune test - clone not inside envelope
      {
        return segm;
      }
      // add clone to the list
      clones.add(clone);
    }
    // get another direction for the original stem too   
    split(trf,0,nseg,seg_splits_eff);
    return -1;
  }

  void Stem::split(otMatrix& trf, float s_angle, int nseg, int nsplits)
  {
    // applies a split angle to the stem - the Weber/Penn method
    int remaining_seg = segmentCount-nseg-1;
    
    // the splitangle
    float declination = acos(trf.getZ().normalize().Z)*180/PI;
    float split_angle = max(0.0f,(
      lpar.splitAngle + calcVariance(lpar.splitAngleV) - declination)
      );
    
    if (lpar.levelNumber == 0)
    {
      Stem::rotxzDegree(trf,split_angle,s_angle);
    }
    else
    {
      trf.lrotXDegree(split_angle);
    }
    
    // adapt split correction
    splitCorrection -=  split_angle/remaining_seg;
    //t_corr = Transformation().rotx(-split_angle/remaining_seg)
    
    float split_diverge;
    if (s_angle>0) // original stem has s_angle==0
    { 
	    if (par.trunk.baseSplits>0 && stemlevel==0 && nseg==0) 
            {
		    split_diverge = s_angle + calcVariance(lpar.splitAngleV);
	    }
            else
            {
		    split_diverge = 20 + 0.75f * (30 + fabsf(declination-90)) 
		    * pow((calcVariance(1)+1)/2.0f,2);
		    if (calcVariance(1) >= 0) split_diverge = - split_diverge;
	    }
  	  
            trf.grotateDegree(split_diverge,otVector3(0,0,1));
  	  
    } 
    
    // lower substem prospensity
    if (! pruneTest) 
    {
	    substemsPerSegment /= (float)(nsplits+1);
	    // FIXME: same reduction for leaves_per_segment?
    }
    return;
  }

  long Stem::substemTotal() 
  {
	  long sum = substems.getCount();
	  for (unsigned int i=0; i<substems.getCount(); i++) 
          {
		  // FIXME: what about clones?
		  sum += substems.get(i)->substemTotal();
	  }
	  return sum;
  }

  long Stem::leafCount() 
  {
	  long sum = 0;
  	
	  // last level add all leaves
	  if (stemlevel==par.levels-1) 
          {
		  sum = leaves.getCount();
  		
		  if (clones.getCount() != 0) 
                  {
			  for (unsigned int i=0; i<clones.getCount(); i++) 
                          {
				  sum += clones.get(i)->leafCount();
			  }
		  }
  		
	  }
	  else // recursive call to substems
          {
		  for (unsigned int i=0; i<substems.getCount(); i++) 
                  {
			  sum += substems.get(i)->leafCount();
		  }
		  if (clones.getCount() != 0) 
                  {
			  for (unsigned int i=0; i<clones.getCount(); i++) 
                          {
				  sum += clones.get(i)->leafCount();
			  }	  	
		  }
	  }
  	
	  return sum;
  }

  float Stem::getShapeRatio(float ratio, int shape)
  {
    switch (shape) 
    { 
      case CONICAL: return 0.2f+0.8f*ratio;
      case SPHERICAL: return 0.2f+0.8f*sinf(PI*ratio);
      case HEMISPHERICAL: return 0.2f+0.8f*sinf(0.5f*PI*ratio);
      case CYLINDRICAL: return 1.0f;
      case TAPERED_CYLINDRICAL: return 0.5f+0.5f*ratio;
      case FLAME: return ratio<=0.7f? ratio/0.7f : (1-ratio)/0.3f;
      case INVERSE_CONICAL: return 1-0.8f*ratio;
      case TEND_FLAME: return ratio<=0.7f? 0.5f+0.5f*ratio/0.7f : 0.5f+0.5f*(1-ratio)/0.3f;
      case ENVELOPE:
      {
        if (ratio<0 || ratio>1) return 0;
        else if (ratio<(1-par.pruneWidthPeak)) return powf((float)ratio/(1-par.pruneWidthPeak),par.prunePowerHigh);
        else return powf((1-(float)ratio)/(1-par.pruneWidthPeak),par.prunePowerLow);
      }
    }
    otAssert("Stem::getShapeRatio => Unknown Shape");
    return 0;
  }

}
