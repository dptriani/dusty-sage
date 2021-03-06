#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include "read_tree_hdf5.h"
#include "../core_mymalloc.h"


/* Local Structs */
struct METADATA_NAMES 
{
  char name_NTrees[MAX_STRING_LEN];
  char name_totNHalos[MAX_STRING_LEN];
  char name_TreeNHalos[MAX_STRING_LEN];
}; 

/* Local Proto-Types */
static int32_t fill_metadata_names(struct METADATA_NAMES *metadata_names, enum Valid_TreeTypes my_TreeType);
static int32_t read_attribute_int(hid_t my_hdf5_file, char *groupname, char *attr_name, int *attribute);
static int32_t read_dataset(char *dataset_name, int32_t datatype, void *buffer, struct forest_info *forests_info);

/* Externally visible Functions */
void load_forest_table_hdf5(struct forest_info *forests_info)
{
    const char *filename = forests_info->filename;
    int32_t totNHalos;
    int32_t nforests;
    int32_t status;

    struct METADATA_NAMES metadata_names;

    forests_info->hdf5_fp = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);

    if (forests_info->hdf5_fp < 0) {
        printf("can't open file `%s'\n", filename);
        ABORT(0);    
    }

    status = fill_metadata_names(&metadata_names, run_params.TreeType);
    if (status != EXIT_SUCCESS) {
        ABORT(0);
    }
 
    status = read_attribute_int(forests_info->hdf5_fp, "/Header", metadata_names.name_NTrees, &nforests);
    if (status != EXIT_SUCCESS) {
        fprintf(stderr, "Error while processing file %s\n", filename);
        fprintf(stderr, "Error code is %d\n", status);
        ABORT(0);
    }
    forests_info->nforests = nforests;

    
    status = read_attribute_int(forests_info->hdf5_fp, "/Header", metadata_names.name_totNHalos, &totNHalos);
    if (status != EXIT_SUCCESS) { 
        fprintf(stderr, "Error while processing file %s\n", filename);
        fprintf(stderr, "Error code is %d\n", status);
        ABORT(0);
    }
    
    forests_info->totnhalos_per_forest = mycalloc(nforests, sizeof(int)); 
    status = read_attribute_int(forests_info->hdf5_fp, "/Header", metadata_names.name_TreeNHalos, forests_info->totnhalos_per_forest);
    if (status != EXIT_SUCCESS) {
        fprintf(stderr, "Error while processing file %s\n", filename);
        fprintf(stderr, "Error code is %d\n", status);
        ABORT(0);
    }
}

#define READ_TREE_PROPERTY(sage_name, hdf5_name, type_int, data_type)   \
    {                                                                   \
        snprintf(dataset_name, MAX_STRING_LEN - 1, "tree_%03d/%s", forestnr, #hdf5_name); \
        status = read_dataset(dataset_name, type_int, buffer, forests_info);         \
        if (status != EXIT_SUCCESS) {                                   \
            ABORT(0);                                                   \
        }                                                               \
        for (int halo_idx = 0; halo_idx < nhalos; ++halo_idx) { \
            local_halos[halo_idx].sage_name = ((data_type*)buffer)[halo_idx];  \
        }                                                               \
    }                                                                   \

#define READ_TREE_PROPERTY_MULTIPLEDIM(sage_name, hdf5_name, type_int, data_type) \
    {                                                                   \
        snprintf(dataset_name, MAX_STRING_LEN - 1, "tree_%03d/%s", forestnr, #hdf5_name); \
        status = read_dataset(dataset_name, type_int, buffer_multipledim, forests_info); \
        if (status != EXIT_SUCCESS) {                                   \
            ABORT(0);                                                   \
        }                                                               \
        for (int halo_idx = 0; halo_idx < nhalos; ++halo_idx) { \
            for (int dim = 0; dim < NDIM; ++dim) {                      \
                local_halos[halo_idx].sage_name[dim] = ((data_type*)buffer_multipledim)[halo_idx * NDIM + dim]; \
            }                                                           \
        }                                                               \
    }                                                                   \


void load_forest_hdf5(int32_t forestnr, const int32_t nhalos, struct halo_data **halos, struct forest_info *forests_info)
{

    char dataset_name[MAX_STRING_LEN];
    int32_t status;

    double *buffer; // Buffer to hold the read HDF5 data.  
    // The largest data-type will be double. 

    double *buffer_multipledim; // However also need a buffer three times as large to hold data such as position/velocity.

    if (forests_info->hdf5_fp <= 0) {
        fprintf(stderr, "The HDF5 file should still be opened when reading the halos in the forest.\n");
        fprintf(stderr, "For forest %d we encountered error\n", forestnr);
        H5Eprint(forests_info->hdf5_fp, stderr);
        ABORT(0);
    }


    *halos = mymalloc(sizeof(struct halo_data) * nhalos); 
    struct halo_data *local_halos = *halos;
    
    buffer = calloc(nhalos, sizeof(*(buffer)));
    if (buffer == NULL) {
        fprintf(stderr, "Could not allocate memory for the HDF5 buffer.\n");
        ABORT(0);
    }

    buffer_multipledim = calloc(nhalos * NDIM, sizeof(*(buffer_multipledim))); 
    if (buffer_multipledim == NULL) {
        fprintf(stderr, "Could not allocate memory for the HDF5 multiple dimension buffer.\n");
        ABORT(0);
    }

    // We now need to read in all the halo fields for this forest.
    // To do so, we read the field into a buffer and then properly slot the field into the Halo struct.

    /* Merger Tree Pointers */ 
    READ_TREE_PROPERTY(Descendant, Descendant, 0, int);
    READ_TREE_PROPERTY(FirstProgenitor, FirstProgenitor, 0, int);
    READ_TREE_PROPERTY(NextProgenitor, NextProgenitor, 0, int);
    READ_TREE_PROPERTY(FirstHaloInFOFgroup, FirstHaloInFOFgroup, 0, int);
    READ_TREE_PROPERTY(NextHaloInFOFgroup, NextHaloInFOFgroup, 0, int);

    /* Halo Properties */ 
    READ_TREE_PROPERTY(Len, Len, 0, int);
    READ_TREE_PROPERTY(M_Mean200, M_mean200, 1, float);
    READ_TREE_PROPERTY(Mvir, Mvir, 1, float);
    READ_TREE_PROPERTY(M_TopHat, M_TopHat, 1, float);
    READ_TREE_PROPERTY_MULTIPLEDIM(Pos, Pos, 1, float);
    READ_TREE_PROPERTY_MULTIPLEDIM(Vel, Vel, 1, float);
    READ_TREE_PROPERTY(VelDisp, VelDisp, 1, float);
    READ_TREE_PROPERTY(Vmax, Vmax, 1, float);
    READ_TREE_PROPERTY_MULTIPLEDIM(Spin, Spin, 1, float);
    READ_TREE_PROPERTY(MostBoundID, MostBoundID, 2, long long);

    /* File Position Info */
    READ_TREE_PROPERTY(SnapNum, SnapNum, 0, int);
    READ_TREE_PROPERTY(FileNr, Filenr, 0, int);
    READ_TREE_PROPERTY(SubhaloIndex, SubHaloIndex, 0, int);
    READ_TREE_PROPERTY(SubHalfMass, SubHalfMass, 0, int);

    free(buffer);
    free(buffer_multipledim);

#ifdef DEBUG_HDF5_READER 
    for (int32_t i = 0; i < 20; ++i) {
        printf("halo %d: Descendant %d FirstProg %d x %.4f y %.4f z %.4f\n", i, local_halos[i].Descendant, local_halos[i].FirstProgenitor, local_halos[i].Pos[0], local_halos[i].Pos[1], local_halos[i].Pos[2]); 
    }
    ABORT(0);
#endif 
 
}

#undef READ_TREE_PROPERTY
#undef READ_TREE_PROPERTY_MULTIPLEDIM

void close_hdf5_file(struct forest_info *forests_info)
{

    H5Fclose(forests_info->hdf5_fp);

}

/* Local Functions */
static int32_t fill_metadata_names(struct METADATA_NAMES *metadata_names, enum Valid_TreeTypes my_TreeType)
{
    switch (my_TreeType) {
    case genesis_lhalo_hdf5: 
        
        snprintf(metadata_names->name_NTrees, MAX_STRING_LEN - 1, "NTrees"); // Total number of forests within the file.
        snprintf(metadata_names->name_totNHalos, MAX_STRING_LEN - 1, "totNHalos"); // Total number of halos within the file.
        snprintf(metadata_names->name_TreeNHalos, MAX_STRING_LEN - 1, "TreeNHalos"); // Number of halos per forest within the file.
        return EXIT_SUCCESS;
        
    case lhalo_binary: 
        fprintf(stderr, "If the file is binary then this function should never be called.  Something's gone wrong...");
        return EXIT_FAILURE;
        
    default:
        fprintf(stderr, "Your tree type has not been included in the switch statement for ``%s`` in file ``%s``.\n", __FUNCTION__, __FILE__);
        fprintf(stderr, "Please add it there.\n");
        ABORT(EXIT_FAILURE);
        
    }

    return EXIT_FAILURE;
}

static int32_t read_attribute_int(hid_t my_hdf5_file, char *groupname, char *attr_name, int *attribute)
{

    int32_t status;
    hid_t attr_id; 

    attr_id = H5Aopen_by_name(my_hdf5_file, groupname, attr_name, H5P_DEFAULT, H5P_DEFAULT);
    if (attr_id < 0) {
        fprintf(stderr, "Could not open the attribute %s in group %s\n", attr_name, groupname);
        return attr_id;
    }

    status = H5Aread(attr_id, H5T_NATIVE_INT, attribute);
    if (status < 0) {
        fprintf(stderr, "Could not read the attribute %s in group %s\n", attr_name, groupname);
        return status;
    }
    
    status = H5Aclose(attr_id);
    if (status < 0) {
        fprintf(stderr, "Error when closing the file.\n"); 
        H5Eprint(status, stderr);
        return status;
    }
   
    return EXIT_SUCCESS; 
}

static int32_t read_dataset(char *dataset_name, int32_t datatype, void *buffer, struct forest_info *forests_info)
{
    hid_t dataset_id;

    dataset_id = H5Dopen2(forests_info->hdf5_fp, dataset_name, H5P_DEFAULT);
    if (dataset_id < 0) {
        fprintf(stderr, "Error encountered when trying to open up dataset %s\n", dataset_name); 
        H5Eprint(dataset_id, stderr);
        return dataset_id;
    }

    switch (datatype)
        {

        case 0:
            H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer);  
            break;

        case 1:
            H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer);
            break;
        case 2:
            H5Dread(dataset_id, H5T_NATIVE_LLONG, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer);            
            break;
            
        default:
            fprintf(stderr, "Your data type has not been included in the switch statement for ``%s`` in file ``%s``.\n", __FUNCTION__, __FILE__);
            fprintf(stderr, "Please add it there.\n");
            ABORT(EXIT_FAILURE);

        }
  
    return EXIT_SUCCESS;
}
