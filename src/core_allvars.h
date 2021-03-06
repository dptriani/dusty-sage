#ifndef ALLVARS_H
#define ALLVARS_H

/* define off_t as a 64-bit long integer */
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "macros.h"
#include "core_simulation.h"

// This structure contains the properties that are output
struct GALAXY_OUTPUT  
{
  int   SnapNum;

#if 0    
  short Type;
  short isFlyby;
#else
  int Type;
#endif    

  long long   GalaxyIndex;
  long long   CentralGalaxyIndex;
  int   SAGEHaloIndex;
  int   SAGETreeIndex;
  long long   SimulationHaloIndex;
  
  int   mergeType;  /* 0=none; 1=minor merger; 2=major merger; 3=disk instability; 4=disrupt to ICS */
  int   mergeIntoID;
  int   mergeIntoSnapNum;
  float dT;

  /* (sub)halo properties */
  float Pos[3];
  float Vel[3];
  float Spin[3];
  int   Len;   
  float Mvir;
  float CentralMvir;
  float Rvir;
  float Vvir;
  float Vmax;
  float VelDisp;

  /* baryonic reservoirs */
  float ColdGas;
  float f_H2;
  float f_HI;
  float cf;
  float Zp;
  float Pressure;
  float StellarMass;
  float BulgeMass;
  float BulgeInstability;
  float HotGas;
  float EjectedMass;
  float BlackHoleMass;
  float ICS;

  /* metals */
  float MetalsColdGas;
  float MetalsStellarMass;
  float MetalsBulgeMass;
  float MetalsHotGas;
  float MetalsEjectedMass;
  float MetalsICS;

  /* dust */
  float ColdDust;
  float HotDust;
  float EjectedDust;

  /* to calculate magnitudes */
  float SfrDisk;
  float SfrBulge;
  float SfrDiskZ;
  float SfrBulgeZ;
  float SfrDiskDTG;
  float SfrBulgeDTG;

  /* dust dot */
  float dustdotform;
  float dustdotgrowth;
  float dustdotdestruct;

  /* star formation rate in each snapshot */
//  float Sfr[SNAPLEN];
  
  /* misc */
  float DiskScaleRadius;
  float Cooling;
  float Heating;
  float QuasarModeBHaccretionMass;
  float TimeOfLastMajorMerger;
  float TimeOfLastMinorMerger;
  float OutflowRate;

  /* infall properties */
  float infallMvir;
  float infallVvir;
  float infallVmax;
};


/* This structure contains the properties used within the code */
struct GALAXY
{
  int   SnapNum;
  int   Type;

  int   GalaxyNr;
  int   CentralGal;
  int   HaloNr;
  long long MostBoundID;

  int   mergeType;  /* 0=none; 1=minor merger; 2=major merger; 3=disk instability; 4=disrupt to ICS */
  int   mergeIntoID;
  int   mergeIntoSnapNum;
  float dT;

  /* (sub)halo properties */
  float Pos[3];
  float Vel[3];
  int   Len;   
  float Mvir;
  float deltaMvir;
  float CentralMvir;
  float Rvir;
  float Vvir;
  float Vmax;

  /* baryonic reservoirs */
  float ColdGas;
  float f_H2;
  float f_HI;
  float cf;
  float Zp;
  float Pressure;
  float StellarMass;
  float BulgeMass;
  float BulgeInstability;
  float HotGas;
  float EjectedMass;
  float BlackHoleMass;
  float ICS;

  /* metals */
  float MetalsColdGas;
  float MetalsStellarMass;
  float MetalsBulgeMass;
  float MetalsHotGas;
  float MetalsEjectedMass;
  float MetalsICS;
  
  /* dust */
  float ColdDust;
  float HotDust;
  float EjectedDust;

  /* to calculate magnitudes */
  float SfrDisk[STEPS];
  float SfrBulge[STEPS];
  float SfrDiskColdGas[STEPS];
  float SfrDiskColdGasMetals[STEPS];
  float SfrDiskColdGasDust[STEPS];
  float SfrBulgeColdGas[STEPS];
  float SfrBulgeColdGasMetals[STEPS];
  float SfrBulgeColdGasDust[STEPS];

  /* dust dot */
  float dustdotform[STEPS];
  float dustdotgrowth[STEPS];
  float dustdotdestruct[STEPS];
 
 /* to calculate metal */
  float Sfr[SNAPLEN];

  /* misc */
  float DiskScaleRadius;
  float MergTime;
  double Cooling;
  double Heating;
  float r_heat;
  float QuasarModeBHaccretionMass;
  float TimeOfLastMajorMerger;
  float TimeOfLastMinorMerger;
  float OutflowRate;
  float TotalSatelliteBaryons;

  /* infall properties */
  float infallMvir;
  float infallVvir;
  float infallVmax;
};



/* auxiliary halo data */
struct halo_aux_data   
{
    int DoneFlag;
    int HaloFlag;
    int NGalaxies;
    int FirstGalaxy;
    int orig_index;
    int output_snap_n;
};

#if 0
#ifdef HDF5
extern char          *core_output_file;
extern size_t         HDF5_dst_size;
extern size_t        *HDF5_dst_offsets;
extern size_t        *HDF5_dst_sizes;
extern const char   **HDF5_field_names;
extern hid_t         *HDF5_field_types;
extern int            HDF5_n_props;
#endif
#endif

#define DOUBLE 1
#define STRING 2
#define INT 3

enum Valid_TreeTypes
{
  lhalo_binary = 0,
  genesis_lhalo_hdf5 = 1,
  genesis_standard_hdf5 = 2,
  consistent_trees_ascii = 3,
  ahf_trees_ascii = 4,
  num_tree_types
};

/* do not use '0' as an enum since that '0' usually
   indicates 'success' on POSIX systems */
typedef enum {
    /* start off with a large number */
    FILE_NOT_FOUND=1 << 12,
    SNAPSHOT_OUT_OF_RANGE,
    OUT_OF_MEMBLOCKS,
    MALLOC_FAILURE,
    INVALID_PTR_REALLOC_REQ,
    INTEGER_32BIT_TOO_SMALL,
    NULL_POINTER_FOUND,
    FILE_READ_ERROR,
    FILE_WRITE_ERROR,
} sage_error_types;

struct forest_info {
    int32_t nforests;
    int32_t nsnapshots;
    int32_t *totnhalos_per_forest;
    off_t *bytes_offset_for_forest;
    int32_t **nhalos_per_forest_per_snapshot;
    union {
        FILE *fp;
        int fd;
#ifdef HDF5
        hid_t hdf5_fp;
#endif
    };
    char filename[4*MAX_STRING_LEN + 1];
};


struct params
{
    int    FirstFile;    /* first and last file for processing */
    int    LastFile;

    char   OutputDir[MAX_STRING_LEN];
    char   FileNameGalaxies[MAX_STRING_LEN];
    char   TreeName[MAX_STRING_LEN];
    char   TreeExtension[MAX_STRING_LEN]; // If the trees are in HDF5, they will have a .hdf5 extension. Otherwise they have no extension.
    char   SimulationDir[MAX_STRING_LEN];
    char   FileWithSnapList[MAX_STRING_LEN];

    double Omega;
    double OmegaLambda;
    double PartMass;
    double Hubble_h;
    double EnergySNcode;
    double EnergySN;
    double EtaSNcode;
    double EtaSN;

    /* recipe flags */
    int    ReionizationOn;
    int    SupernovaRecipeOn;
    int    DiskInstabilityOn;
    int    AGNrecipeOn;
    int    SFprescription;
    int    H2prescription;
    int    MetalYieldsOn;
    int    AGBYields; 
    int    SNIIYields;
    int    SNIaYields;
   
    double RecycleFraction;
    double Yield;
    double BinaryFraction;
    double FracZleaveDisk;
    double ReIncorporationFactor;
    double ThreshMajorMerger;
    double BaryonFrac;
    double SfrEfficiency;
    double H2ClumpFactor;
    double H2ClumpExp;
    double H2Exp;
    double FeedbackReheatingEpsilon;
    double FeedbackEjectionEfficiency;
    double RadioModeEfficiency;
    double QuasarModeEfficiency;
    double BlackHoleGrowthRate;
    double Reionization_z0;
    double Reionization_zr;
    double ThresholdSatDisruption;

    /* stellar yields */
    double Qagb[MAXYIELDS][METALGRID];
    double Qsn[MAXYIELDS][METALGRID];
    double qCagb[MAXYIELDS][METALGRID];
    double qNagb[MAXYIELDS][METALGRID];
    double qOagb[MAXYIELDS][METALGRID];
    double qCsn[MAXYIELDS][METALGRID];
    double qOsn[MAXYIELDS][METALGRID];
    double qMgsn[MAXYIELDS][METALGRID];
    double qSisn[MAXYIELDS][METALGRID];
    double qSsn[MAXYIELDS][METALGRID];
    double qCasn[MAXYIELDS][METALGRID];
    double qFesn[MAXYIELDS][METALGRID];
    double qCrsnia;
    double qFesnia;
    double qNisnia;
    double magb[MAXYIELDS];
    double msn[MAXYIELDS];
    int countagb;
    int countsn;

    double UnitLength_in_cm;
    double UnitVelocity_in_cm_per_s;
    double UnitMass_in_g;
    double UnitTime_in_s;
    double RhoCrit;
    double UnitPressure_in_cgs;
    double UnitDensity_in_cgs;
    double UnitCoolingRate_in_cgs;
    double UnitEnergy_in_cgs;
    double UnitTime_in_Megayears;
    double G;
    double Hubble;
    double a0;
    double ar;

    int LastSnapShotNr;
    int MAXSNAPS;
    int NOUT;
    int Snaplistlen;
    enum Valid_TreeTypes TreeType;

    int ListOutputSnaps[ABSOLUTEMAXSNAPS];
    double ZZ[ABSOLUTEMAXSNAPS];
    double AA[ABSOLUTEMAXSNAPS];
    double *Age;
    double lbtime[ABSOLUTEMAXSNAPS];
};
extern struct params run_params;


#endif  /* #ifndef ALLVARS_H */
