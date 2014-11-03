//---------------------------------------------------------------------------
//........................... BOUNDARY CONDITIONS ...........................
//---------------------------------------------------------------------------
#ifndef __bc_h__
#define __bc_h__
//---------------------------------------------------------------------------
// boundary condition context
typedef struct
{
	// boundary conditions vectors (velocity, pressure, temperature)
	// NOTE: get rid of these vectors, by extending single- and two-point constraint specification
	Vec bcvx,  bcvy, bcvz, bcp, bcT; // local (ghosted)

	// single-point constraints
	PetscInt     numSPC;   // number of single point constraints (SPC)
	PetscInt    *SPCList;  // global indices of SPC (global layout)
	PetscScalar *SPCVals;  // values of SPC

	PetscInt     numSPCPres;   // number of pressure SPC
	PetscInt    *SPCListPres;  // global indices of pressure SPC (pressure layout)

	// two-point constraints
	PetscInt     numTPC;       // number of two-point constraints (TPC)
	PetscInt    *TPCList;      // local indices of TPC (ghosted layout)
	PetscInt    *TPCPrimeDOF;  // local indices of primary DOF (ghosted layout)
	PetscScalar *TPCVals;      // values of TPC
	PetscScalar *TPCLinComPar; // linear combination parameters

	// Dirichlet pushing constraints
	PetscScalar  xbs[3];       // block start coord
	PetscScalar  xbe[3];       // block end coord
	PetscScalar  vval[2];      // dirichlet values for Vx and Vy
	PetscScalar  theta;        // rotation angle
	PetscBool    pflag;        // flag for activating pushing

} BCCtx;
//---------------------------------------------------------------------------

// create boundary condition context
PetscErrorCode BCCreate(BCCtx *bc, FDSTAG *fs);

// destroy boundary condition context
PetscErrorCode BCDestroy(BCCtx *bc);

// initialize boundary constraint vectors
PetscErrorCode BCInit(BCCtx *bc, FDSTAG *fs, idxtype idxmod);

//---------------------------------------------------------------------------

// initialize pushing boundary conditions context
PetscErrorCode PBCInit(BCCtx *bc, UserContext *user);

// get the spc for pushing - dynamic
PetscErrorCode PBCGetIndices(BCCtx *bc, FDSTAG *fs, PetscScalar ***pbcvx, PetscScalar ***pbcvy, PetscInt *SPCListPush, PetscInt numSPCPush, PetscInt start);

// advect the pushing block
PetscErrorCode PBCAdvectBlock(UserContext *user);

//---------------------------------------------------------------------------

#endif
