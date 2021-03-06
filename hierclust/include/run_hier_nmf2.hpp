// Copyright 2014 Georgia Institute of Technology
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "tree.hpp"
#include "clust.hpp"
#include "terms.hpp"
#include "random.hpp"
#include "assignments.hpp"
#include "sparse_matrix.hpp"

//-----------------------------------------------------------------------------
template <typename T>
bool RunHierNmf2(const unsigned int m,
                 const unsigned int n,
                 const SparseMatrix<T>& A,
                 std::vector<T>& buf_a,
                 std::vector<unsigned int>& assignments,
                 std::vector<unsigned int>& assignments_flat,
                 std::vector<float>& probabilities,
                 std::vector<int>& term_indices,
                 Tree<T>& tree,
                 ClustStats& stats,
                 const ClustOptions& clust_opts,
                 Random& rng)
{
    // W and H buffer for flat clustering
    std::vector<T> buf_w(m*clust_opts.num_clusters);
    std::vector<T> buf_h(clust_opts.num_clusters*n);

    Result result = Result::OK;

    if (A.Size() > 0)
    {
        result = ClustSparse(clust_opts, A,
                             &buf_w[0], &buf_h[0], tree, stats, rng);
    }
    else
    {
        result = Clust(clust_opts, &buf_a[0], m, // ldim_a == m
                       &buf_w[0], &buf_h[0], tree, stats, rng);
    }
    
    if (clust_opts.flat)
    {
        // compute flat clustering assignments and top terms
        unsigned int k = clust_opts.num_clusters;
        ComputeFuzzyAssignments(probabilities, &buf_h[0], k, k, n);
        ComputeAssignments(assignments_flat, &buf_h[0], k, k, n);
        TopTerms(clust_opts.maxterms, &buf_w[0], m, m, k, term_indices);        
    }
 
    return (Result::OK == result);
}
