//
// Created by PikachuHy on 2023/3/25.
//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <pscm/Number.h>
#include <pscm/Pair.h>
#include <pscm/Scheme.h>
#include <pscm/Str.h>
#include <pscm/Symbol.h>
#include <pscm/scm_utils.h>
using namespace pscm;
using namespace doctest;

TEST_CASE("testing 1.3.4, Evaluation examples") {
  Scheme scm;
  Cell ret;
  ret = scm.eval("(* 5 8)");
  CHECK(ret == 40);
}

TEST_CASE("testing 1.3.4, Evaluation examples, with register machine") {
  Scheme scm(true);
  Cell ret;
  ret = scm.eval("(* 5 8)");
  CHECK(ret == 40);
}

TEST_CASE("testing 2.2, Whitespace and comments") {
  Scheme scm;
  Cell ret;
  ret = scm.eval(R"(
;;; The FACT procedure computes the factorial
;;; of a non-negative integer.
(define fact
  (lambda (n)
    (if (= n 0)
      1         ;Base case: return 1
      (* n (fact (- n 1))))))
)");
  CHECK(ret == Cell::none());
  ret = scm.eval("(fact 0)");
  CHECK(ret == 1);
  ret = scm.eval("(fact 1)");
  CHECK(ret == 1);
  ret = scm.eval("(fact 2)");
  CHECK(ret == 2);
  ret = scm.eval("(fact 3)");
  CHECK(ret == 6);
}

TEST_CASE("testing 2.2, Whitespace and comments, with register machine") {
  Scheme scm(true);
  Cell ret;
  ret = scm.eval(R"(
;;; The FACT procedure computes the factorial
;;; of a non-negative integer.
(define fact
  (lambda (n)
    (if (= n 0)
      1         ;Base case: return 1
      (* n (fact (- n 1))))))
)");
  CHECK(ret == Cell::none());
  ret = scm.eval("fact");
  REQUIRE(ret.is_proc());
  ret = scm.eval("(fact 0)");
  CHECK(ret == 1);
  ret = scm.eval("(fact 1)");
  CHECK(ret == 1);
  ret = scm.eval("(fact 2)");
  CHECK(ret == 2);
  ret = scm.eval("(fact 3)");
  CHECK(ret == 6);
}

TEST_CASE("testing 4.1.2, Literal expressions") {
  Scheme scm;
  Cell ret;
  ret = scm.eval("(quote a)");
  CHECK(ret == "a"_sym);
  ret = scm.eval("(quote #(a b c))");
  Cell::Vec vec;
  auto a = "a"_sym;
  auto b = "b"_sym;
  auto c = "c"_sym;
  vec.emplace_back(&a);
  vec.emplace_back(&b);
  vec.emplace_back(&c);
  CHECK(ret == vec);
  ret = scm.eval("(quote (+ 1 2))");
  auto plus = "+"_sym;
  auto one = "1"_num;
  auto two = "2"_num;
  CHECK(ret == cons(&plus, cons(&one, cons(&two, nil))));

  ret = scm.eval("'a");
  CHECK(ret == "a"_sym);
  ret = scm.eval("'#(a b c)");
  CHECK(ret == vec);
  ret = scm.eval("'()");
  CHECK(ret == nil);
  ret = scm.eval("'(+ 1 2)");
  CHECK(ret == cons(&plus, cons(&one, cons(&two, nil))));
  ret = scm.eval("'(quote a)");
  auto quote = "quote"_sym;
  CHECK(ret == pscm::list(&quote, &a));
  ret = scm.eval("''a");
  CHECK(ret == pscm::list(&quote, &a));

  ret = scm.eval("'\"abc\"");
  pscm::String abc("abc");
  CHECK(ret == Cell(&abc));
  ret = scm.eval("\"abc\"");
  CHECK(ret == Cell(&abc));
  ret = scm.eval("'145932");
  CHECK(ret == 145932);
  ret = scm.eval("145932");
  CHECK(ret == 145932);
  ret = scm.eval("'#t");
  CHECK(ret == Cell::bool_true());
  ret = scm.eval("#t");
  CHECK(ret == Cell::bool_true());
}

TEST_CASE("testing 4.1.2, Literal expressions, with register machine") {
  Scheme scm(true);
  Cell ret;
  ret = scm.eval("(quote a)");
  CHECK(ret == "a"_sym);
  ret = scm.eval("(quote #(a b c))");
  Cell::Vec vec;
  auto a = "a"_sym;
  auto b = "b"_sym;
  auto c = "c"_sym;
  vec.emplace_back(&a);
  vec.emplace_back(&b);
  vec.emplace_back(&c);
  CHECK(ret == vec);
  ret = scm.eval("(quote (+ 1 2))");
  auto plus = "+"_sym;
  auto one = "1"_num;
  auto two = "2"_num;
  CHECK(ret == cons(&plus, cons(&one, cons(&two, nil))));

  ret = scm.eval("'a");
  CHECK(ret == "a"_sym);
  ret = scm.eval("'#(a b c)");
  CHECK(ret == vec);
  ret = scm.eval("'()");
  CHECK(ret == nil);
  ret = scm.eval("'(+ 1 2)");
  CHECK(ret == cons(&plus, cons(&one, cons(&two, nil))));
  ret = scm.eval("'(quote a)");
  auto quote = "quote"_sym;
  CHECK(ret == pscm::list(&quote, &a));
  ret = scm.eval("''a");
  CHECK(ret == pscm::list(&quote, &a));

  ret = scm.eval("'\"abc\"");
  pscm::String abc("abc");
  CHECK(ret == Cell(&abc));
  ret = scm.eval("\"abc\"");
  CHECK(ret == Cell(&abc));
  ret = scm.eval("'145932");
  CHECK(ret == 145932);
  ret = scm.eval("145932");
  CHECK(ret == 145932);
  ret = scm.eval("'#t");
  CHECK(ret == Cell::bool_true());
  ret = scm.eval("#t");
  CHECK(ret == Cell::bool_true());
}

TEST_CASE("testing 4.1.3, Procedure calls") {
  auto f = [](Scheme& scm) {
    Cell ret;
    ret = scm.eval("(+ 3 4)");
    CHECK(ret == 7);
    ret = scm.eval("((if #f + *) 3 4)");
    CHECK(ret == 12);
  };
  {
    Scheme scm;
    f(scm);
  }
  {
    Scheme scm(true);
    f(scm);
  }
}

TEST_CASE("testing 4.1.4, Lambda expressions") {
  auto f = [](Scheme& scm) {
    Cell ret;
    ret = scm.eval("(lambda (x) (+ x x))");
    CHECK(ret.is_proc());
    ret = scm.eval("((lambda (x) (+ x x)) 4)");
    CHECK(ret == 8);
    ret = scm.eval(R"(
(define reverse-subtract
  (lambda (x y) (- y x)))
)");
    REQUIRE(ret == Cell::none());
    ret = scm.eval("(reverse-subtract 7 10)");
    CHECK(ret == 3);
    ret = scm.eval(R"(
(define add4
  (let ((x 4))
    (lambda (y) (+ x y))))
)");
    REQUIRE(ret == Cell::none());
    ret = scm.eval("(add4 6)");
    CHECK(ret == 10);

    ret = scm.eval("((lambda x x) 3 4 5 6)");
    CHECK(ret == list(3, 4, 5, 6));
    ret = scm.eval("((lambda (x y . z) z) 3 4 5 6)");
    CHECK(ret == list(5, 6));
  };
  {
    Scheme scm;
    f(scm);
  }
  {
    Scheme scm(true);
    f(scm);
  }
}

TEST_CASE("testing 4.1.5, conditionals") {
  auto f = [](Scheme& scm) {
    Cell ret;
    ret = scm.eval("(if (> 3 2) 'yes 'no)");
    CHECK(ret == "yes"_sym);
    ret = scm.eval("(if (> 2 3) 'yes 'no)");
    CHECK(ret == "no"_sym);
    ret = scm.eval(R"(
(if (> 3 2)
    (- 3 2)
    (+ 3 2))
)");
    CHECK(ret == 1);
  };
  {
    Scheme scm;
    f(scm);
  }
  {
    Scheme scm(true);
    f(scm);
  }
}

TEST_CASE("testing 4.1.6, Assignments") {
  auto f = [](Scheme& scm) {
    Cell ret;
    ret = scm.eval("(define x 2)");
    REQUIRE(ret == Cell::none());
    ret = scm.eval("x");
    CHECK(ret == 2);
    ret = scm.eval("(+ x 1)");
    CHECK(ret == 3);
    ret = scm.eval("(set! x 4)");
    CHECK(ret == Cell::none());
    ret = scm.eval("(+ x 1)");
    CHECK(ret == 5);
  };
  {
    Scheme scm;
    f(scm);
  }
  {
    Scheme scm(true);
    f(scm);
  }
}

TEST_CASE("testing 4.2.1, Conditionals") {
  auto f = [](Scheme& scm) {
    Cell ret;
    ret = scm.eval(R"(
(cond ((> 3 2) 'greater)
      ((< 3 2) 'less))
)");
    CHECK(ret == "greater"_sym);
    ret = scm.eval(R"(
(cond ((> 3 3) 'greater)
      ((< 3 3) 'less)
      (else 'equal))
)");
    CHECK(ret == "equal"_sym);
    ret = scm.eval(R"(
(cond ((assv 'b '((a 1) (b 2))) => cadr)
      (else #f))
)");
    CHECK(ret == 2);
  };
  {
    Scheme scm;
    f(scm);
  }
  {
    Scheme scm(true);
    f(scm);
  }
}

TEST_CASE("testing 6.3, Pairs and lists") {
  auto f = [](Scheme& scm) {
    Cell ret;
    ret = scm.eval("(define x (list 'a 'b 'c))");
    REQUIRE(ret == Cell::none());
    ret = scm.eval("(define y x)");
    REQUIRE(ret == Cell::none());
    ret = scm.eval("y");
    auto a = "a"_sym;
    auto b = "b"_sym;
    auto c = "c"_sym;
    CHECK(ret == list(&a, &b, &c));
    ret = scm.eval("(list? y)");
    CHECK(ret == Cell::bool_true());
    ret = scm.eval("(set-cdr! x 4)");
    REQUIRE(ret == Cell::none());
    auto num4 = "4"_num;
    ret = scm.eval("x");
    CHECK(ret == Cell(cons(&a, &num4)));
    ret = scm.eval("(eqv? x y)");
    CHECK(ret == Cell::bool_true());
    ret = scm.eval("y");
    CHECK(ret == cons(&a, &num4));
    ret = scm.eval("(list? y)");
    CHECK(ret == Cell::bool_false());
    ret = scm.eval("(set-cdr! x x)");
    REQUIRE(ret == Cell::none());
    ret = scm.eval("(list? x)");
    CHECK(ret == Cell::bool_false());
    ret = scm.eval("(assv 5 '((2 3) (5 7) (11 13)))");
    CHECK(ret == list(5, 7));
  };
  {
    Scheme scm;
    f(scm);
  }
  {
    Scheme scm(true);
    f(scm);
  }
}