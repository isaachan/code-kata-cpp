// Code Kata 6: Anagrams
//
// $Id: kata6.cc,v 1.7 2004/02/09 11:27:33 mathie Exp $
//
// $Log: kata6.cc,v $
// Revision 1.7  2004/02/09 11:27:33  mathie
// * Refactor test_dictionary::find_largest_group().
// * Implement test_dictionary::find_longgest_anagram().
//
// Revision 1.6  2004/02/09 09:53:32  mathie
// * Change anagrams::iterator to store a pointer (rather than a reference)
//   to its anagrams container to save implementing an operator=().
// * Refactor the dictionary-based tests into a class so they can share the
//   generated anagrams class.
// * Implement a test to find the largest group of anagrams - the results
//   have been verified by hacking around with grep and cut.
//
// Revision 1.5  2004/02/09 07:29:42  mathie
// * More whitespace cleanup...
// * Implement anagrams::size() correctly (I can't believe that took me two
//   days to get right!).
// * Test cases to verify that the parsed dictionaries produce the expected
//   number of results.
//
// Revision 1.4  2004/02/07 16:55:12  mathie
// * Tidy up whitespace.
// * Implement an iterator for anagrams in terms of the map it's composed
//   with.
// * Use Std C++ Library algorithms where possible.
// * Add to the test_anagrams() function to verify the result is correct.
//
// Still some work required to tidy things up...
//
// Revision 1.3  2004/02/07 09:09:47  mathie
// * Normalise case, ignore non-alpha characters (instead of assert()ing on
//   them).
// * When output is generated, ignore words in map which have no anagrams.
// * Test functions to generate anagram lists from dictionary files.
//
// Revision 1.2  2004/02/07 07:04:15  mathie
// * Very basic implementation so far - a class to encapsulate word
//   'equivalence' (ie they share all the same characters) and a class
//   which will collate equivalent words.  The interface to the collation
//   class needs work to make it useful.
//
// Revision 1.1  2004/02/05 16:37:36  mathie
// Initial revision
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataSix.rdoc

#include <boost/test/unit_test.hpp>
#include <boost/compose.hpp>
#include <boost/shared_ptr.hpp>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <iterator>
#include <iostream>
#include <fstream>

using boost::unit_test_framework::test_suite;
using boost::compose_f_gx;
using boost::shared_ptr;

using namespace std;

class word_rep
{
  unsigned int char_count[26];
  unsigned int *char_count_begin, *char_count_end;
  const string word;

 public:
  word_rep(const string& word)
    : char_count_begin(char_count),
      char_count_end(char_count + sizeof(char_count)/sizeof(*char_count)),
      word(word)
  {
    fill(char_count_begin, char_count_end, 0);
    for(string::const_iterator it = word.begin(); it != word.end(); it++) {
      if(!isalpha(*it)) {
        BOOST_MESSAGE("Word '" << word << "' contains non-alpha characters.  "
                      << "Ignoring them...");
        continue;
      }
      char_count[*it - (islower(*it) ? 'a' : 'A')]++;
    }
  }

  // Copy-constructor, required to explicitly copy char_count.
  word_rep(const word_rep& c)
    : char_count_begin(char_count),
      char_count_end(char_count + sizeof(char_count)/sizeof(*char_count)),
      word(c.word)
  {
    copy(c.char_count_begin, c.char_count_end, char_count_begin);
  }

  const string& operator()() const
  {
    return word;
  }

  bool operator==(const word_rep& rhs) const
  {
    return equal(char_count_begin, char_count_end, rhs.char_count_begin);
  }

  bool operator!=(const word_rep& rhs) const
  {
    return !operator==(rhs);
  }

  bool operator<(const word_rep& rhs) const
  {
    return lexicographical_compare(char_count_begin, char_count_end,
                                   rhs.char_count_begin, rhs.char_count_end);
  }
  friend ostream& operator<<(ostream& s, const word_rep& w)
  {
    s << w.word;
    return s;
  }

};

class anagrams
{
 public:
  typedef const string& const_reference;
  typedef set<string> word_list;
  friend ostream& operator<<(ostream& s, const anagrams::word_list& l);

 private:
  typedef map<word_rep, word_list> anagram_list;

  anagram_list al;

 public:
  class iterator : public std::iterator<std::bidirectional_iterator_tag,
                                        word_list, ptrdiff_t>
  {
    const anagrams* al;
    anagram_list::const_iterator it;

  public:
    iterator(const anagrams& a, const anagram_list::const_iterator& i)
      : al(&a), it(i)
    {
    }

    bool operator==(const iterator& x) const
    {
      return it == x.it;
    }
    bool operator!=(const iterator& x) const
    {
      return !(*this == x);
    }
    const word_list& operator*() const
    {
      return it->second;
    }
    const word_list* operator->() const
    {
      return &it->second;
    }

    iterator& operator++()
    {
      do {
        ++it;
      } while(it->second.size() < 2 && *this != al->end());
      return *this;
    }
    iterator operator++(int)
    {
      iterator tmp = *this;
      ++*this;
      return tmp;
    }
    iterator& operator--()
    {
      do {
        --it;
      } while(it->second.size() < 2 && *this != al->begin());
      return *this;
    }
    iterator operator--(int)
    {
      iterator tmp = *this;
      --*this;
      return tmp;
    }
  };

  void insert(const string& w)
  {
    al[word_rep(w)].insert(w);
  }
  void push_back(const string& w)
  {
    insert(w);
  }

  iterator begin() const
  {
    return iterator(*this, al.begin());
  }

  iterator end() const
  {
    return iterator(*this, al.end());
  }

  size_t size() const
  {
    return count_if(begin(), end(),
                    compose_f_gx(bind2nd(greater<size_t>(), 1),
                                 mem_fun_ref(&word_list::size)));
  }

  friend ostream& operator<<(ostream& s, const anagrams& a)
  {
    // FIXME: Why doesn't the copy() version work?
    // copy(a.begin(), a.end(), ostream_iterator<word_list>(s, "\n"));
    for(iterator it = a.begin(); it != a.end(); it++) {
      s << *it << "..." << endl;
    }
    return s;
  }
};

ostream& operator<<(ostream& s, const anagrams::word_list& l)
{
  set<string>::const_iterator j = l.begin();
  s << *j;
  while(++j != l.end()) {
    s << " " << *j;
  }
  return s;
}

// Test functions
void test_word_rep()
{
  word_rep kinship("kinship"), pinkish("pinkish"), enlist("enlist"),
    inlets("inlets"), listen("listen"), silent("silent"), boaster("boaster"),
    boaters("boaters"), borates("borates"), fresher("fresher"),
    refresh("refresh"), sinks("sinks"), skins("skins"), knits("knits"),
    stink("stink"), rots("rots"), sort("sort");

  BOOST_CHECK_EQUAL(kinship(), "kinship");
  BOOST_CHECK_EQUAL(pinkish(), "pinkish");
  BOOST_CHECK_EQUAL(enlist(), "enlist");
  BOOST_CHECK_EQUAL(inlets(), "inlets");
  BOOST_CHECK_EQUAL(listen(), "listen");
  BOOST_CHECK_EQUAL(silent(), "silent");
  BOOST_CHECK_EQUAL(boaster(), "boaster");
  BOOST_CHECK_EQUAL(boaters(), "boaters");
  BOOST_CHECK_EQUAL(borates(), "borates");
  BOOST_CHECK_EQUAL(fresher(), "fresher");
  BOOST_CHECK_EQUAL(refresh(), "refresh");
  BOOST_CHECK_EQUAL(sinks(), "sinks");
  BOOST_CHECK_EQUAL(skins(), "skins");
  BOOST_CHECK_EQUAL(knits(), "knits");
  BOOST_CHECK_EQUAL(stink(), "stink");
  BOOST_CHECK_EQUAL(rots(), "rots");
  BOOST_CHECK_EQUAL(sort(), "sort");

  // All permutations of equal words from the above list.
  BOOST_CHECK(kinship == pinkish);
  BOOST_CHECK(pinkish == kinship);

  BOOST_CHECK(enlist == inlets);
  BOOST_CHECK(enlist == listen);
  BOOST_CHECK(enlist == silent);
  BOOST_CHECK(inlets == enlist);
  BOOST_CHECK(inlets == listen);
  BOOST_CHECK(inlets == silent);
  BOOST_CHECK(listen == enlist);
  BOOST_CHECK(listen == inlets);
  BOOST_CHECK(listen == silent);
  BOOST_CHECK(silent == enlist);
  BOOST_CHECK(silent == inlets);
  BOOST_CHECK(silent == listen);

  BOOST_CHECK(boaster == boaters);
  BOOST_CHECK(boaster == borates);
  BOOST_CHECK(boaters == borates);
  BOOST_CHECK(boaters == boaster);
  BOOST_CHECK(borates == boaster);
  BOOST_CHECK(borates == boaters);

  BOOST_CHECK(fresher == refresh);
  BOOST_CHECK(refresh == fresher);

  BOOST_CHECK(sinks == skins);
  BOOST_CHECK(skins == sinks);

  BOOST_CHECK(knits == stink);
  BOOST_CHECK(stink == knits);

  BOOST_CHECK(rots == sort);
  BOOST_CHECK(sort == rots);

  // Check some words which are not equal
  BOOST_CHECK(kinship != inlets);
  BOOST_CHECK(knits != skins);

  // Check some orderings (required if want nice fast searching with a
  // map or somesuch).
  BOOST_CHECK(!(stink < knits));
  BOOST_CHECK(stink < skins);
  BOOST_CHECK(rots < knits);
}

void test_anagrams()
{
  const string l[] = { "kinship", "pinkish",
                       "enlist", "inlets", "listen", "silent",
                       "boaster", "boaters", "borates",
                       "fresher", "refresh",
                       "sinks", "skins",
                       "knits", "stink",
                       "rots", "sort" };
  const size_t l_sz = sizeof l / sizeof *l;
  anagrams a;

  copy(l, l + l_sz, back_inserter(a));

  for(anagrams::iterator it = a.begin(); it != a.end(); it++) {
    word_rep first(*it->begin());
    for(set<string>::const_iterator jt = it->begin(); jt != it->end(); jt++) {
      BOOST_CHECK(first == word_rep(*jt));
    }
  }
}

template<typename F>
struct first_element_functor
  : public unary_function<F, typename F::value_type>
{
  typename first_element_functor<F>::result_type operator()(typename first_element_functor<F>::argument_type x) const
  {
    return *x.begin();
  }
};

class test_dictionary
{
  anagrams a;
  const string in_file;
  const string out_file;
  const size_t expected_groups;
  const string expected_largest_group;
  const string expected_longest_anagram;

 public:
  test_dictionary(const string& in_file, const string& out_file,
                  size_t expected_groups, const string& expected_largest_group,
                  const string& expected_longest_anagram)
    : in_file(in_file), out_file(out_file), expected_groups(expected_groups),
      expected_largest_group(expected_largest_group),
      expected_longest_anagram(expected_longest_anagram)
  {
  }

  void load()
  {
    BOOST_MESSAGE("Retrieving dictionary " << in_file << "...");
    ifstream f(in_file.c_str());
    copy(istream_iterator<string>(f), istream_iterator<string>(),
         back_inserter(a));

    BOOST_CHECK_EQUAL(a.size(), expected_groups);
  }

  void write_out()
  {
    ofstream f(out_file.c_str());
    f << a;
  }

  void find_largest_group()
  {
    anagrams::iterator l = a.begin();
    for(anagrams::iterator it = a.begin();
        it != a.end();
        it = find_if(++it, a.end(),
                     compose_f_gx(bind2nd(greater<size_t>(), l->size()),
                                  mem_fun_ref(&anagrams::word_list::size)))) {
      l = it;
    }
    cout << "Largest group (" << in_file << "): ";
    copy(l->begin(), l->end(), ostream_iterator<string>(cout, " "));
    cout << endl;
    BOOST_CHECK_EQUAL(word_rep(expected_largest_group), word_rep(*l->begin()));
  }

  void find_longest_anagram()
  {
    anagrams::iterator l = a.begin();
    for(anagrams::iterator it = a.begin();
        it != a.end();
        it = find_if(++it, a.end(),
                     compose_f_gx(bind2nd(greater<size_t>(),
                                          l->begin()->size()),
                                  compose_f_gx(mem_fun_ref(&string::size),
                                               first_element_functor<anagrams::word_list>())))) {
      l = it;
    }
    cout << "Longest anagram (" << in_file << "): ";
    copy(l->begin(), l->end(), ostream_iterator<string>(cout, " "));
    cout << endl;

    BOOST_CHECK_EQUAL(word_rep(expected_longest_anagram),
                      word_rep(*l->begin()));
  }
};

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 6: Anagrams");
  t->add(BOOST_TEST_CASE(test_word_rep));
  t->add(BOOST_TEST_CASE(test_anagrams));

  shared_ptr<test_dictionary> kata_dict(new test_dictionary("wordlist.txt",
                                                            "wordlist.out",
                                                            2530,
                                                            "spear",
                                                            "algorithmically"));
  shared_ptr<test_dictionary> main_dict(new test_dictionary("/usr/share/dict/words",
                                                            "maindict.out",
                                                            15048,
                                                            "organ",
                                                            "cholecystoduodenostomy"));
  t->add(BOOST_CLASS_TEST_CASE(&test_dictionary::load, kata_dict));
  t->add(BOOST_CLASS_TEST_CASE(&test_dictionary::write_out, kata_dict));
  t->add(BOOST_CLASS_TEST_CASE(&test_dictionary::find_largest_group,
                               kata_dict));
  t->add(BOOST_CLASS_TEST_CASE(&test_dictionary::find_longest_anagram,
                               kata_dict));
  t->add(BOOST_CLASS_TEST_CASE(&test_dictionary::load, main_dict));
  t->add(BOOST_CLASS_TEST_CASE(&test_dictionary::write_out, main_dict));
  t->add(BOOST_CLASS_TEST_CASE(&test_dictionary::find_largest_group,
                               main_dict));
  t->add(BOOST_CLASS_TEST_CASE(&test_dictionary::find_longest_anagram,
                               main_dict));

  return t;
}
