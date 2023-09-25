#include <algorithm>    // e.g., std::for_each, std::sort, std::find, std::reverse_copy
#include <cstddef>      // e.g., std::size_t
#include <functional>   // e.g., std::greater
#include <fstream>      // e.g., std::ifstream
#include <iterator>     // e.g., std::iterator_traits, std::forward_iterator, std::back_inserter
#include <iostream>     // e.g., std::cout, std::clog
#include <map>          // e.g., std::map
#include <set>          // e.g., std::set
#include <string>       // e.g., std::string
#include <vector>       // e.g., std::vector

using namespace std;

template<std::forward_iterator FwdIter>
std::ostream &output(
    std::ostream &os,
    FwdIter first,
    FwdIter last,
    bool const with_indices = false
) {
  using count_type = typename std::iterator_traits<FwdIter>::difference_type;
  count_type index{};  // initialize to 0
  count_type rindex = std::distance(first, last);

  std::for_each(first, last, [&](auto const &value) {
    os << value;
    if (with_indices) {
      os << " (" << index << ':' << -rindex << ')';
    }
    os << ' ';
    ++index;
    --rindex;
  });
  return os;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <text_file_path>\n";
    return 1;
  }

  string filepath(argv[1]);
  ifstream infile(filepath);
  if (!infile.is_open()) {
    cerr << "Error: Unable to open file " << filepath << '\n';
    return 1;
  }

  map<string, size_t> freq_hist;
  vector<string> words;
  string word;
  while (infile >> word) {
    freq_hist[word]++;
    words.push_back(word);
  }

  if (words.empty()) {
    cerr << "There is no data. Aborting.\n";
    return 2;
  }

  // cout << "Words in file:\n";
  // output(cout, words.begin(), words.end(), true) << '\n';
  //
  // cout << "\nFrequency histogram:\n";
  // for (const auto &[word, count]: freq_hist) {
  //   cout << word << ": " << count << '\n';
  // }

  // Compute the inverse frequency histogram
  map<size_t, set<string>> inverse_freq_hist;
  auto i = freq_hist.begin(), i_end = freq_hist.end();
  for (; i != i_end; ++i) {
    inverse_freq_hist.insert({i->second, {}});
    inverse_freq_hist[i->second].insert(i->first);
  }

  // cout << "\nInverse frequency histogram:\n";
  cout << "frequency ";
  for (const auto &[count, words]: inverse_freq_hist) {
    cout << count << " occurs with these strings: ";
    output(cout, words.begin(), words.end()) << '\n';
  }

  // Determine the value in the inverse frequency histogram that has the smallest std::set .size() value
  auto smallest_entry = std::min_element(
      inverse_freq_hist.begin(),
      inverse_freq_hist.end(),
      [](const auto& a, const auto& b) {
        return a.second.size() < b.second.size();
      }
  );

  if (smallest_entry == inverse_freq_hist.end()) {
    cerr << "Error: Inverse frequency histogram is empty\n";
    return 3;
  }

  // Output the result
  // cout << "\nSmallest set of words with largest frequency (" << smallest_entry->second.size() << " words, frequency " << smallest_entry->first << "):\n";
  // output(cout, smallest_entry->second.begin(), smallest_entry->second.end()) << '\n';


  vector<string> revwords;
  std::reverse_copy(words.begin(), words.end(), std::back_inserter(revwords));

  // Extract the smallest and largest words in the std::set with the largest frequency
  const std::set<std::string>& largest_set = smallest_entry->second;
  auto largest_pos = std::prev(largest_set.end());
  auto smallest_pos = largest_set.begin();

  // Output the smallest and largest words
  std::cout << "lo value: " << *smallest_pos << '\n';
  std::cout << "hi value: " << *largest_pos << '\n';
  cout << "All words reversed: ";
  output(cout, words.rbegin(), words.rend(), true) << '\n';


  // Finding the First Position "lo" Appears in revwords + an Adjustment
  auto lopos = std::find(revwords.begin(), revwords.end(), *smallest_pos);
  auto hipos = std::find(std::make_reverse_iterator(revwords.end()), std::make_reverse_iterator(lopos), *largest_pos);

  // if (lopos == revwords.end()) {
  //   cerr << "Error: Could not find the position of the smallest word in revwords.\n";
  //   return 4;
  // }
  //
  // if (hipos == std::make_reverse_iterator(lopos)) {
  //   cerr << "Error: Could not find the position of the largest word in revwords.\n";
  //   return 4;
  // }

  // Adjust lopos to account for the reverse order
  lopos = revwords.end() - (lopos - revwords.begin()) - 1;

  // Adjust hipos to account for the reverse order
  hipos = std::make_reverse_iterator(std::prev(hipos.base()));

  // cout << "All words reversed with [" << *lopos << ',' << *hipos << ") sorted: ";
  // std::sort(lopos, hipos);
  // output(cout, lopos, hipos) << '\n';

  // std::cout << "\nPosition of \"" << *smallest_pos << "\" in reverse order: " << std::distance(revwords.begin(), lopos) << '\n';
  // std::cout << "\nPosition of \"" << *largest_pos << "\" in reverse order: " << std::distance(revwords.begin(), hipos) << '\n';

  // Sort the range in descending order
  std::sort(lopos, hipos.base(), std::greater<std::string>());

  // Output the sorted range
  // std::cout << "\nSorted range in descending order:\n";
  // output(std::cout, lopos, hipos.base(), true) << '\n';

  return 0;
}
