
## Parallelism in Modern C++ (C++17/20/23)

### C++17: Parallel STL Algorithms

One of the major milestones in C++17 was the extension of the Standard Library’s algorithms to support parallel execution. Many algorithms (e.g., **std::for_each**, **std::transform**, **std::reduce**, and others) gained overloads that accept an *execution policy* parameter from the `<execution>` header. For example:

```cpp
#include <algorithm>
#include <execution>
#include <vector>

std::vector<double> inVec = /* ... */;
std::vector<double> outVec(inVec.size());

// Multiply each element by 2 in parallel:
std::transform(std::execution::par, inVec.begin(), inVec.end(), outVec.begin(),
               [](double v) { return v * 2.0; });
```

Here, the policy `std::execution::par` tells the implementation that the algorithm may execute in parallel. Another policy, `std::execution::par_unseq`, additionally allows vectorization. This high-level approach enables an almost “drop-in” change to existing STL algorithms, improving readability and portability.

### C++20: Coroutines, Executors, and Enhanced Concurrency

C++20 added coroutines—which are powerful for handling asynchronous work and lazy computations—as well as improvements in the concurrency model:
  
- **Coroutines:** They simplify the expression of asynchronous tasks by allowing functions to suspend and resume their execution. This can be used to build task systems or asynchronous pipelines without having to manage threads explicitly.
  
- **Executors & Future Enhancements:** Although not fully standardized at the time of C++20’s finalization, proposals and early implementations (in libraries like HPX) offer standardized executors. These executors abstract the notion of “where” and “how” tasks execute, thereby providing a unified interface to schedule work in parallel.
  
- **Improved Ranges and <execution>:** The ranges library along with execution policies now work more seamlessly, enabling composition of algorithms that handle parallelism in a more modular way.

### C++23 and Beyond: Continuing to Evolve Parallelism

While C++23 is still emerging in practice (and some parallel enhancements are still under active discussion in the committees), you will see:
  
- **Refinements in the execution model:** Greater control on task scheduling and finer-grained control over reductions and accumulations.
  
- **Further integration with low-level parallelism concepts:** Features such as better support for executors and proposals that might standardize aspects of the sender/receiver model for asynchronous execution are expected to continue bridging the gap between low-level control and high-level, portable abstractions.

---

## Application Example: Parallel Matrix Multiplication

Consider a simple matrix multiplication task. In a hand-rolled matrix multiplication function, you might have nested loops where each dot product calculation can be performed independently. With C++17 parallel algorithms, one can express this operation at a high level without manually spawning threads:

1. **Using Parallel STL:**  
   You could use `std::transform_reduce` to compute individual dot products concurrently. For each row of the first matrix and each column of the second, you parallelize the dot product computation:
   
   ```cpp
   #include <vector>
   #include <execution>
   #include <numeric>
   #include <functional>
   
   using Matrix = std::vector<std::vector<double>>;
   
   double dot_product(const std::vector<double>& row,
                      const std::vector<double>& col) {
       return std::transform_reduce(std::execution::par,
                                    row.begin(), row.end(),  // elements of the row
                                    col.begin(),              // corresponding column element
                                    0.0,                      // initial value
                                    std::plus<>(),
                                    std::multiplies<>());
   }
   
   Matrix multiply(const Matrix& A, const Matrix& B) {
       size_t m = A.size();
       size_t n = B[0].size();
       Matrix C(m, std::vector<double>(n, 0));
       // Assume B is transposed for easier column access or extract columns as needed.
       for (size_t i = 0; i < m; ++i) {
           std::transform(std::execution::par, B[0].begin(), B[0].end(),
                          C[i].begin(),
                          [&](size_t j) {
                              // Compute dot product between A[i] and column j of B.
                              // You might need to extract column j into a vector.
                              std::vector<double> col_j;
                              for (size_t k = 0; k < A[i].size(); ++k)
                                  col_j.push_back(B[k][j]);
                              return dot_product(A[i], col_j);
                          });
       }
       return C;
   }
   ```
   In this simplified example the use of **std::transform** and **std::transform_reduce** with the `par` execution policy expresses parallelism without explicit thread management.

2. **Considerations:**  
   Since matrix multiplication is often memory-bound as well as computation-bound, low-level optimizations like cache blocking, loop tiling, and explicit vectorization (often achieved by libraries such as BLAS) might outperform a naïve parallel algorithm. Still, the advantage of the C++ standard approach is its integration with the language and STL, easing maintenance and portability.

---

## Comparison: Modern C++ Parallelism vs. OpenMP

### **Programming Model and Abstraction**

- **Modern C++ Parallelism:**  
  - **Standardized and High-Level:** Inserting an execution policy parameter into STL algorithms abstracts away thread management and synchronizes data access using the safety of C++’s type system and standard library components.  
  - **Maintainability & Portability:** Code written using `std::execution::par` or similar is portable across compilers that support the standard and typically integrates better with C++’s existing design patterns.
  
- **OpenMP:**  
  - **Directive-Based:** Uses pragmas (e.g., `#pragma omp parallel for`) which instruct the compiler to parallelize code regions.  
  - **Low-Level Control:** Provides more tuning options such as scheduling (static, dynamic, guided), explicit thread affinity, nesting controls, and fine control over synchronization primitives.  
  - **Maturity and Performance:** OpenMP has been around for decades and is highly optimized on many compilers. Experts can squeeze extra performance out of critical loops, making it favorable in HPC contexts.

### **Ease of Use and Integration**

- **Modern C++ Approach:**  
  - **Minimal Code Changes:** Changing a sequential algorithm to a parallel one may be as simple as adding an execution policy.  
  - **Automatic and Composable:** Abstractions like parallel algorithms can be composed with other modern C++ features (lambdas, ranges, coroutines) with little boilerplate.
  
- **OpenMP Approach:**  
  - **Explicit Parallel Regions:** Requires careful annotation of loops and code regions, explicit specification of data scoping (private, firstprivate, reduction clauses) which adds verbosity.
  - **Platform/Compiler Dependency:** While widely supported, OpenMP is still a non-standard extension and may behave differently across compilers.

### **Performance Considerations**

- **Modern C++ Parallel Algorithms:**  
  - **Good “Out of the Box” Performance:** In many cases, particularly for operations that fit the parallel algorithm mold (independent computations per element), performance can scale with available cores.
  - **Abstraction Overhead:** The abstraction layer may introduce slight overhead or less fine-tuned control over memory accesses compared to hand-tuned code.
  
- **OpenMP:**  
  - **Fine-Tuning for Maximum Performance:** Expert users can often exceed the performance of generic parallel algorithms by configuring scheduling, managing thread pools, and optimizing memory alignment.
  - **Best for Critical Loops:** In compute-bound loops (such as optimized matrix multiplication in HPC), OpenMP’s pragmas may result in better performance when the code is carefully tuned.

---

## How to compile with specific standard ? 

If you choose use a specific version of C++, include the parameter on compilation process
``-std:c++17  -std:c++19 -std:c++23`` 


## Which Is “Better”?

There is no universally superior choice—it depends on your requirements:

- **For Portability and Maintainability:** Modern C++ parallel features (C++17/20/23) are highly attractive. They are standardized, integrate with the STL and modern programming paradigms, and reduce boilerplate.
  
- **For Fine-Grained Performance Tuning in Critical Applications:** OpenMP often provides the extra level of control (thread scheduling, affinity, specialized reduction support) that can lead to higher performance when exploited fully.

- **For Matrix Multiplication and Similar Domains:** In a practical setting, if you’re writing your own matrix multiplication, a parallel algorithm built with modern C++ might be “good enough” and easier to maintain. However, for high-performance or production code, you would often rely on established vendor-optimized libraries (BLAS, Intel MKL, OpenBLAS) which may internally use OpenMP or other techniques to achieve top performance.

---

## Conclusion

Modern C++ has introduced a variety of features to support parallel operation:
  
- **C++17** brought parallel STL algorithms with execution policies, making it easy to parallelize common operations.  
- **C++20** further enhanced concurrency via coroutines and work on executors, simplifying asynchronous programming.  
- **C++23** continues this trend with further refinements and proposals that promise even more flexible parallelism.

These high-level, standardized features ease development and improve portability compared to OpenMP, which—through compiler directives—offers more control but at the expense of verbosity and potential portability issues. In contexts like matrix multiplication, modern C++ parallel algorithms can deliver substantial performance improvements in a much cleaner way, while OpenMP might be preferred in performance-critical or highly tuned HPC applications.

Each approach has its strengths. For many new projects, leveraging the standard parallel algorithms can be highly attractive, but if you need to squeeze every last bit of performance, particularly in specialized domains, OpenMP still remains a very robust choice.
