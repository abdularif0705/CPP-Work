#ifndef a5_random_provided_hpp_
#define a5_random_provided_hpp_

//===========================================================================

#include <random>
#include <vector>
#include <algorithm>

//===========================================================================

template <typename MersenneEngine = std::mt19937>
MersenneEngine make_random_mersenne_engine()
{
  using namespace std;

  // Use std::random_device for random data...
  random_device rd;
  vector<typename MersenneEngine::result_type> data;

  // Populate random_data with enough random data for the random engine's state_size...
  data.reserve(MersenneEngine::state_size);
  generate_n(
    back_inserter(data),
    MersenneEngine::state_size,
    [&rd](){ return rd(); }
  );

  // Fill seed_seq with seed value from data
  seed_seq seeds(begin(data), end(data));

  // Return the engine...
  return MersenneEngine(seeds);
}

//===========================================================================

#endif // #ifndef a5_random_provided_hpp_
