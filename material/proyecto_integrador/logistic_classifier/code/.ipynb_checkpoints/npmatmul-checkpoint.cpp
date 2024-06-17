#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <omp.h>
#include <utility>

namespace py = pybind11;


py::tuple matrix_multiply(py::array_t<double,py::array::c_style> A,
				    py::array_t<double,py::array::c_style> B) {
  //Read the numpy array buffers
  py::buffer_info A_buf = A.request();
  py::buffer_info B_buf = B.request();

  if(A_buf.ndim != 2 || B_buf.ndim != 2)
    throw std::runtime_error("Number of dimensions must be two");
  if(A_buf.shape[1] != B_buf.shape[0])
    throw std::runtime_error("Matrix dimensions do not match for multiplication");

  int nrows = A_buf.shape[0];
  int ncols = B_buf.shape[1];
  int nn = A_buf.shape[1];

  //Create a numpy array bufferto store the result
  py::array_t<double> result = py::array_t<double>({nrows,ncols});
  result[py::make_tuple(py::ellipsis())] = 0.f; //initialize to zeros
  py::buffer_info res_buf = result.request();

  //Create pointers to the buffers
  double *ptr_A = static_cast<double *>(A_buf.ptr);
  double *ptr_B = static_cast<double *>(B_buf.ptr);
  double *ptr_res = static_cast<double *>(res_buf.ptr);

  double start = omp_get_wtime();
    
  //Implement matrix multiplication

    
  double stop = omp_get_wtime();
  double time = stop-start;
    
  return py::make_tuple(std::move(result),time);
}


/*
py::tuple matrix_multiply_omp(py::array_t<double,py::array::c_style> A,
					py::array_t<double,py::array::c_style> B) {
  //Read the numpy array buffers
  py::buffer_info A_buf = A.request();
  py::buffer_info B_buf = B.request();

  if(A_buf.ndim != 2 || B_buf.ndim != 2)
    throw std::runtime_error("Number of dimensions must be two");
  if(A_buf.shape[1] != B_buf.shape[0])
    throw std::runtime_error("Matrix dimensions do not match for multiplication");

  int nrows = A_buf.shape[0];
  int ncols = B_buf.shape[1];
  int nn = A_buf.shape[1];

  //Create a numpy array bufferto store the result
  py::array_t<double> result = py::array_t<double>({nrows,ncols});
  result[py::make_tuple(py::ellipsis())] = 0.f; //initialize to zeros
  py::buffer_info res_buf = result.request();

  //Create pointers to the buffers
  double *ptr_A = static_cast<double *>(A_buf.ptr);
  double *ptr_B = static_cast<double *>(B_buf.ptr);
  double *ptr_res = static_cast<double *>(res_buf.ptr);

  omp_set_num_threads(8);

  double start = omp_get_wtime();
 
  //Perform the matrix multiplication and parallelize with OMP



  double stop = omp_get_wtime();
  double time = stop-start;
  
  return py::make_tuple(std::move(result),time);
}
*/

//Pybind11 module macro that creates an entry point that will be invoked
// when the python interpreter imports an extension module.
PYBIND11_MODULE(npmatmul, m) {
  m.doc() = "Pybind11 module that performs numpy array multiplication";
  m.def("matrix_multiply",&matrix_multiply,"Function that multiplies two matrices");
  //m.def("matrix_multiply_omp",&matrix_multiply_omp,"Function that multiplies two matrices. Uses OMP");
}
