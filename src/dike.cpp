/*@ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 **
 **    Copyright (c) 2011-2015, JGU Mainz, Anton Popov, Boris Kaus
 **    All rights reserved.
 **
 **    This software was developed at:
 **
 **         Institute of Geosciences
 **         Johannes-Gutenberg University, Mainz
 **         Johann-Joachim-Becherweg 21
 **         55128 Mainz, Germany
 **
 **    project:    LaMEM                                                                                                                                                
 **    filename:   dike.cpp
 **
 **    LaMEM is free software: you can redistribute it and/or modify
 **    it under the terms of the GNU General Public License as published
 **    by the Free Software Foundation, version 3 of the License.
 **
 **    LaMEM is distributed in the hope that it will be useful,
 **    but WITHOUT ANY WARRANTY; without even the implied warranty of
 **    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 **    See the GNU General Public License for more details.
 **
 **    You should have received a copy of the GNU General Public License
 **    along with LaMEM. If not, see <http://www.gnu.org/licenses/>.
 **
 **
 **    Contact:
 **        Boris Kaus       [kaus@uni-mainz.de]
 **        Anton Popov      [popov@uni-mainz.de]
 **
 **
 **    This routine:
 **         Anton Popov      [popov@uni-mainz.de]
 **         Boris Kaus       [kaus@uni-mainz.de]
 **         Jana Schierjott
 **         Garrett Ito
 **
 ** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ @*/
/*

    This file defines properties for the dike which is defined as an additional 
    source term on the RHS of the continutiy equation

*/
//---------------------------------------------------------------------------
//.................. DIKE PARAMETERS READING ROUTINES....................
//---------------------------------------------------------------------------
#include "LaMEM.h"
#include "phase.h"
#include "parsing.h"
//#include "JacRes.h"
#include "dike.h"
#include "constEq.h"
#include "bc.h"
#include "tssolve.h"
#include "scaling.h"
//---------------------------------------------------------------------------
#undef __FUNCT__
#define __FUNCT__ "DBDikeCreate"
PetscErrorCode DBDikeCreate(DBPropDike *dbdike, DBMat *dbm, FB *fb, PetscBool PrintOutput)   
{

        // read all dike parameter blocks from file
  
        PetscInt jj;

        PetscErrorCode ierr;
        PetscFunctionBegin;

        //===============                                                                                                                                               
        // DIKE PARAMETER                                                                                                               
        //===============                                                                                                                                               

        // setup block access mode                                                                                                                                      
        ierr = FBFindBlocks(fb, _OPTIONAL_, "<DikeStart>", "<DikeEnd>"); CHKERRQ(ierr);

        if(fb->nblocks)
        {
                // print overview of dike blocks from file                                                                                                           
                if (PrintOutput)
		  {
		    PetscPrintf(PETSC_COMM_WORLD,"Dike blocks : \n");
		  }
                // initialize ID for consistency checks                                                                                                                 
                for(jj = 0; jj < _max_num_dike_ ; jj++) dbdike->matDike[jj].ID = -1;


		// error checking
                if(fb->nblocks > _max_num_dike_)
                {
                        SETERRQ1(PETSC_COMM_WORLD, PETSC_ERR_USER, "Too many dikes specified! Max allowed: %lld", (LLD)_max_num_dike_);
                }

                // store actual number of softening laws 
                dbdike->numDike = fb->nblocks;

                if (PrintOutput){
                        PetscPrintf(PETSC_COMM_WORLD,"--------------------------------------------------------------------------\n");
                }
                // read each individual softening law                                                                                                                   
                for(jj = 0; jj < fb->nblocks; jj++)
                {
		  ierr = DBReadDike(dbdike, dbm, fb, PrintOutput); CHKERRQ(ierr);

                        fb->blockID++;
                }
        }

	ierr = FBFreeBlocks(fb); CHKERRQ(ierr);

	PetscFunctionReturn(0);
}
//---------------------------------------------------------------------------
#undef __FUNCT__
#define __FUNCT__ "DBReadDike"
PetscErrorCode DBReadDike(DBPropDike *dbdike, DBMat *dbm, FB *fb, PetscBool PrintOutput)
{
        // read dike parameter from file 
        Dike     *dike;
        PetscInt  ID;
	Scaling  *scal;
	
        PetscErrorCode ierr;
        PetscFunctionBegin;

	// access context
	scal    =  dbm->scal;
	
        // Dike ID                                                                                                                                                         
        ierr    = getIntParam(fb, _REQUIRED_, "ID", &ID, 1, dbdike->numDike-1); CHKERRQ(ierr);
        fb->ID  = ID;

        // get pointer to specified dike parameters
        dike = dbdike->matDike + ID;

        // check ID
        if(dike->ID != -1)
        {
                 SETERRQ(PETSC_COMM_WORLD, PETSC_ERR_USER, "Duplicate of Dike option!");
        }

        // set ID 
        dike->ID = ID;

	// read and store dike  parameters. 
        ierr = getScalarParam(fb, _REQUIRED_, "Mf",      &dike->Mf,      1, 1.0);              CHKERRQ(ierr);
        ierr = getScalarParam(fb, _REQUIRED_, "Mb",      &dike->Mb,      1, 1.0);              CHKERRQ(ierr);
	ierr = getIntParam(   fb, _REQUIRED_, "PhaseID", &dike->PhaseID, 1, dbm->numPhases-1); CHKERRQ(ierr);  
	ierr = getIntParam(   fb, _REQUIRED_, "PhaseTransID", &dike->PhaseTransID, 1, dbm->numPhtr-1); CHKERRQ(ierr);
	ierr = getScalarParam(fb, _OPTIONAL_, "t0_dike", &dike->t0_dike, 1, 1.0);       CHKERRQ(ierr);
	ierr = getScalarParam(fb, _OPTIONAL_, "t1_dike", &dike->t1_dike, 1, 1.0);       CHKERRQ(ierr);
	ierr = getScalarParam(fb, _OPTIONAL_, "v_dike",  &dike->v_dike,  1, 1.0);   CHKERRQ(ierr);

	// scale parameters
      	dike->t0_dike /= scal->time;
       	dike->t1_dike /= scal->time;
	dike->v_dike  /= scal->velocity; 

        if (PrintOutput)
	  {
	    PetscPrintf(PETSC_COMM_WORLD,"   Dike parameters ID[%lld] : Mf = %g, Mb = %g\n", (LLD)(dike->ID), dike->Mf, dike->Mb);
      	    PetscPrintf(PETSC_COMM_WORLD,"   Optional dike parameters: v_dike = %g \n", dike->v_dike, scal->lbl_velocity);
	    PetscPrintf(PETSC_COMM_WORLD,"                             t0_dike = %g \n", dike->t0_dike, scal->lbl_time);
	    PetscPrintf(PETSC_COMM_WORLD,"                             t1_dike = %g \n", dike->t1_dike, scal->lbl_time);
	    PetscPrintf(PETSC_COMM_WORLD,"--------------------------------------------------------------------------\n");
        }

        PetscFunctionReturn(0);
}
//------------------------------------------------------------------------------------------------------------------
#undef __FUNCT__
#define __FUNCT__ "GetDikeContr"
PetscErrorCode GetDikeContr(ConstEqCtx *ctx,
                                  PetscScalar *phRat,          // phase ratios in the control volume
                                  PetscScalar &dikeRHS)
{

        BCCtx       *bc;
        Dike        *matDike;
        Ph_trans_t  *PhaseTrans;
        PetscInt     i, j, numDike;
        PetscScalar  v_spread, M, left, right;

        numDike    = ctx->numDike;
        matDike    = ctx->matDike;

        bc         = ctx->bc;
        PhaseTrans = ctx->PhaseTrans;

          // loop through all dikes
          for(j = 0; j < numDike; j++)
            {
	      // access the phase ID of the dike parameters
              i = matDike->PhaseID;

             // check if the phase ratio of a dike phase is greater than 0 in the current cell
            if(phRat[i]>0)
              {
               if(matDike->Mb == matDike->Mf)
                 {
                  // constant M
                  M = matDike->Mf;
                  v_spread = PetscAbs(bc->velin);
                  left = PhaseTrans->bounds[0];
                  right = PhaseTrans->bounds[1];
                  matDike->dikeRHS = M * 2 * v_spread / PetscAbs(left-right);  // necessary to write dike->dikeRHS?
                 }

	  /*else                                                                                                                                                          
            {
	    // Mb an Mf are different
                // FDSTAG *fs;

                // access context
                // fs = bc->fs;
		// bdx = SIZE_NODE(i, sx, fs->dsx); // distance between two neighbouring cell centers in x-direction 
                //  cdx = SIZE_CELL(i, sx, fs->dsx); // distance between two neigbouring nodes in x-direction       
                                             
                if(front == back)
                {
                    // linear interpolation between different M values, Mf is M in front, Mb is M in back
                    M = dike.Mf + (dike.Mb - dike.Mf) * (y/(PetscAbs(front+back))); 
                    dikeRHS = M * 2 * v_spread / PetscAbs(left+right);  // [1/s] SCALE THIS TERM, now it is in km
		}
                else
                {
                    // linear interpolation if the ridge/dike phase is oblique
                    y = COORD_CELL(j,sy,fs->dsy);
                    M = Mf + (Mb - Mf) * (y/(PetscAbs(front+back)));
                    dikeRHS = M * 2 * v_spread / PetscAbs(left+right);  // [1/s] SCALE THIS TERM, now it is in km 
                }
            }*/
            else
            {
              matDike->dikeRHS = 0.0;   // necessary dike->dikeRHS ?? not really right? it is always passed as a variable
            }

             dikeRHS += phRat[i]*matDike->dikeRHS;   // is it correct to just use dikeRHS? still necessary to save as dike->dikeRHS before because used in cellconsteq?

	      }
        }
    PetscFunctionReturn(0);

}

//------------------------------------------------------------------------------------------------------------------
#undef __FUNCT__
#define __FUNCT__ "MovingDike"
PetscErrorCode MovingDike(DBPropDike *dbdike,
			  Ph_trans_t *PhaseTrans,
			  TSSol *ts)
{

  //  PetscInt     i, numDike;
  PetscScalar  t0_dike, t1_dike, v_dike;
  PetscScalar  t_current, dt;                 // dt is time step from last to current time I believe

  Scaling *scal;  //only for testing here
  scal = ts->scal;  // only for testing here
  
  //  PetscFunctionBegin;  NECESSARY?

  //  numDike    = dbdike->numDike;
  t0_dike    = dbdike->matDike->t0_dike;
  t1_dike    = dbdike->matDike->t1_dike;
  v_dike     = dbdike->matDike->v_dike;
  
  //  PetscPrintf(PETSC_COMM_WORLD," t1_dike = %g\n", t1_dike * scal->time);
  
  dt         = ts->dt;       // time step (but from last to current or from current to next? 
  // dt_next    = ts->dt_next;  // tentative time step, should I rather use this one then?
  t_current  = ts->time;     // current time stamp, computed at the end of last time step round
  
  //  PetscPrintf(PETSC_COMM_WORLD," dt = %g \n", dt*scal->time);
  //  PetscPrintf(PETSC_COMM_WORLD," t_current = %g \n", t_current*scal->time);
  
  // check if the current time step is equal to the starting time of when the dike is supposed to move 
  if(t0_dike >= t_current && t1_dike <= t_current)
    {
      
      // loop through all dikes --> it is checked inside the phase transition function whether we have a dike or not
      //      for(i = 0; i < numDike; i++)
      //{

      PetscPrintf(PETSC_COMM_WORLD," left old = %g\n", PhaseTrans->bounds[0]);
      
      PhaseTrans->bounds[0] = PhaseTrans->bounds[0] + v_dike * dt;  // dt or dt_next?
      PhaseTrans->bounds[1] = PhaseTrans->bounds[1] + v_dike * dt;  // dt or dt_next? if called before phase transition I think dt is correct
      
      PetscPrintf(PETSC_COMM_WORLD," left_new = %g\n", PhaseTrans->bounds[0]);
      //}
      
    }
  
  PetscFunctionReturn(0);
}


// --------------------------------------------------------------------------------------------------------------- 
