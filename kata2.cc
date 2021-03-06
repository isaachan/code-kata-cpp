// Code Kata 2: Binary chop
//
// $Id: kata2.cc,v 1.5 2004/02/05 16:21:44 mathie Exp $
//
// $Log: kata2.cc,v $
// Revision 1.5  2004/02/05 16:21:44  mathie
// * Implement Tuesday's algorithm - the recursive case.
// * Templatise the test code instead of that disgusting preprocessor
//   hack.
//
// Revision 1.4  2004/02/05 15:45:21  mathie
// * Link with the boost unit test library rather than including it
//   wholesale.  Makes for faster compile times...
//
// Revision 1.3  2004/02/02 11:58:18  mathie
// * Added implementation notes for Monday.
//
// Revision 1.2  2004/02/02 11:44:34  mathie
// * Templatize the chop functions so that they accept a generic iterator.
// * Test with both an array and a vector to make sure the templatizing
//   worked.
// * Turn the test function into a macro so we can trivially generate test
//   functions for each day's implementation.
// * Add a linear search as a placeholder for Tuesday's implementation,
//   just to verify the test framework.
//
// Revision 1.1  2004/02/02 09:54:56  mathie
// Initial revision
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataTwo.rdoc
//
// Each implementation of the binary chop algorithm takes the form:
//   chop(int val, iterator begin, iterator end) -> iterator
// where val is the integer to search for, begin is an iterator pointing
// to the start of the list, end is an iterator pointing to
// one-past-the-end and the returned value is an iterator pointing to
// the found item (or end if it's not found).  This is a slight
// C++ification of the functor definition in the problem, but seems
// like a good idea to me...

#include <boost/test/unit_test.hpp>

#include <vector>

using boost::unit_test_framework::test_suite;
using namespace std;

// Monday is the recursive case
// Implementation notes: Today was very straightforward - I spent longer
// trying (and failing) to figure out how to templatize the test
// function.  The only complication was retaining the past-the-end
// failure return value if the chop narrowed down the search scope to
// (begin, middle].  I first implemented the specific case of handling
// arrays before figuring out the appropriate template incantation to
// get it to work with generic iterators.
class monday
{
 public:
  template<class Iter>
  const Iter operator()(const int val, Iter begin, const Iter end)
  {
    // Failure case
    if (begin == end) {
      return end;
    }

    const Iter middle = begin + (end - begin) / 2;
    if (val == *middle) {
      // Success!
      return middle;
    } else if (val > *middle) {
      // Somewhere in the left of centre.
      return operator()(val, middle + 1, end);
    } else {
      // Somewhere to the right of centre.  This is where it gets slightly
      // icky -- we have to explicitly test for failure here because
      // 'middle' is not one-past-the-end of the calling function it's,
      // well, slap bang in the middle.
      const Iter result = operator()(val, begin, middle);
      if (result == middle) {
        return end;
      }
      return result;
    }
  }
};

// Tuesday is the iterative case.
// Implementation notes: I figured out how to templatise the test
// functionality, and spent longer doing that than implementing today's
// algorithm.  The only problem I had was a little carelessness trying
// to calculate 'middle'.  Once I sneaked past the compiler, it worked
// first time. :-)
class tuesday 
{
 public:
  template<class Iter>
  const Iter operator()(const int val, Iter begin, const Iter end)
  {
    Iter newbegin = begin;
    Iter newend = end;
    while(newbegin != newend) {
      Iter middle = newbegin + (newend - newbegin) / 2;
      if(*middle == val) {
        return middle;
      } else if (*middle < val) {
        newbegin = middle + 1;
      } else {
        newend = middle;
      }
    }
    return end;
  }
};

template<class chop_fn>
void test_chop() 
{
  chop_fn fn;
  const int t1[] = {};
  const size_t t1_size = sizeof(t1) / sizeof(t1[0]);
  const int *t1_end = t1 + t1_size;
  BOOST_CHECK_EQUAL(t1_end, fn(3, t1, t1_end));

  vector<int> t1v;
  BOOST_CHECK(t1v.end() == fn(3, t1v.begin(), t1v.end()));

  const int t2[] = { 1 };
  const size_t t2_size = sizeof(t2) / sizeof(t2[0]);
  const int *t2_end = t2 + t2_size;
  BOOST_CHECK(t2_end == fn(3, t2, t2_end));
  BOOST_CHECK(t2[0] == *fn(1, t2, t2_end));

  vector<int> t2v;
  t2v.push_back(1);
  BOOST_CHECK(t2v.end() == fn(3, t2v.begin(), t2v.end()));
  BOOST_CHECK(t2v[0] == *fn(1, t2v.begin(), t2v.end()));

  const int t3[] = { 1, 3, 5 };
  const size_t t3_size = sizeof(t3) / sizeof(t3[0]);
  const int *t3_end = t3 + t3_size;
  BOOST_CHECK(t3[0] == *fn(1, t3, t3_end));
  BOOST_CHECK(t3[1] == *fn(3, t3, t3_end));
  BOOST_CHECK(t3[2] == *fn(5, t3, t3_end));
  BOOST_CHECK(t3_end == fn(0, t3, t3_end));
  BOOST_CHECK(t3_end == fn(2, t3, t3_end));
  BOOST_CHECK(t3_end == fn(4, t3, t3_end));
  BOOST_CHECK(t3_end == fn(6, t3, t3_end));

  vector<int> t3v;
  t3v.push_back(1);
  t3v.push_back(3);
  t3v.push_back(5);
  BOOST_CHECK(t3v[0] == *fn(1, t3v.begin(), t3v.end()));
  BOOST_CHECK(t3v[1] == *fn(3, t3v.begin(), t3v.end()));
  BOOST_CHECK(t3v[2] == *fn(5, t3v.begin(), t3v.end()));
  BOOST_CHECK(t3v.end() == fn(0, t3v.begin(), t3v.end()));
  BOOST_CHECK(t3v.end() == fn(2, t3v.begin(), t3v.end()));
  BOOST_CHECK(t3v.end() == fn(4, t3v.begin(), t3v.end()));
  BOOST_CHECK(t3v.end() == fn(6, t3v.begin(), t3v.end()));

  const int t4[] = { 1, 3, 5, 7 };
  const size_t t4_size = sizeof(t4) / sizeof(t4[0]);
  const int *t4_end = t4 + t4_size;
  BOOST_CHECK(t4[0] == *fn(1, t4, t4_end));
  BOOST_CHECK(t4[1] == *fn(3, t4, t4_end));
  BOOST_CHECK(t4[2] == *fn(5, t4, t4_end));
  BOOST_CHECK(t4[3] == *fn(7, t4, t4_end));
  BOOST_CHECK(t4_end == fn(0, t4, t4_end));
  BOOST_CHECK(t4_end == fn(2, t4, t4_end));
  BOOST_CHECK(t4_end == fn(4, t4, t4_end));
  BOOST_CHECK(t4_end == fn(6, t4, t4_end));
  BOOST_CHECK(t4_end == fn(8, t4, t4_end));

  vector<int> t4v;
  t4v.push_back(1);
  t4v.push_back(3);
  t4v.push_back(5);
  t4v.push_back(7);
  BOOST_CHECK(t4v[0] == *fn(1, t4v.begin(), t4v.end()));
  BOOST_CHECK(t4v[1] == *fn(3, t4v.begin(), t4v.end()));
  BOOST_CHECK(t4v[2] == *fn(5, t4v.begin(), t4v.end()));
  BOOST_CHECK(t4v[3] == *fn(7, t4v.begin(), t4v.end()));
  BOOST_CHECK(t4v.end() == fn(0, t4v.begin(), t4v.end()));
  BOOST_CHECK(t4v.end() == fn(2, t4v.begin(), t4v.end()));
  BOOST_CHECK(t4v.end() == fn(4, t4v.begin(), t4v.end()));
  BOOST_CHECK(t4v.end() == fn(6, t4v.begin(), t4v.end()));
  BOOST_CHECK(t4v.end() == fn(8, t4v.begin(), t4v.end()));
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 2: Binary Chop");
  t->add(BOOST_TEST_CASE(&test_chop<monday>));
  t->add(BOOST_TEST_CASE(&test_chop<tuesday>));
  return t;
}
