/* Area:		ffi_call, closure_call
   Purpose:		Check double arguments in structs.
   Limitations:	none.
   PR:			none.
   Originator:	Blake Chaffin 6/23/2007	*/

/* { dg-do run } */

#include "ffitest.h"

typedef struct Dbls {
	double x;
	double y;
} Dbls;

void
closure_test_fn(Dbls p)
{
	printf("%.1f %.1f\n", p.x, p.y);
}

void
closure_test_gn(ffi_cif* cif __UNUSED__, void* resp __UNUSED__,
		void** args, void* userdata __UNUSED__)
{
	closure_test_fn(*(Dbls*)args[0]);
}

int main(int argc __UNUSED__, char** argv __UNUSED__)
{
	ffi_cif cif;

#ifndef USING_MMAP
	static ffi_closure cl;
#endif

	ffi_closure*	pcl;
	ffi_type*		cl_arg_types[1];

#ifdef USING_MMAP
	pcl = allocate_mmap(sizeof(ffi_closure));
#else
	pcl = &cl;
#endif

	ffi_type	ts1_type;
	ffi_type*	ts1_type_elements[4];

	ts1_type.size = 0;
	ts1_type.alignment = 0;
	ts1_type.type = FFI_TYPE_STRUCT;
	ts1_type.elements = ts1_type_elements;

	ts1_type_elements[0] = &ffi_type_double;
	ts1_type_elements[1] = &ffi_type_double;
	ts1_type_elements[2] = NULL;

	cl_arg_types[0] = &ts1_type;

	Dbls arg = { 1.0, 2.0 };

	/* Initialize the cif */
	CHECK(ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1,
				 &ffi_type_void, cl_arg_types) == FFI_OK);

	CHECK(ffi_prep_closure(pcl, &cif, closure_test_gn, NULL) == FFI_OK);

	((void*(*)(Dbls))(pcl))(arg);
	/* { dg-output "1.0 2.0\n" } */

	closure_test_fn(arg);
	/* { dg-output "1.0 2.0\n" } */

	return 0;
}
