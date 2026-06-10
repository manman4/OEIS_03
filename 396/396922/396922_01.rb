# E.g.f. A(x) satisfies A(x) = x * exp(A^l(x)),
# where A^l(x) denotes the l-th iterate of A.
#
# a(n, k, l) = n! * [x^n] A^k(x)
# a(1, k, l) = 1
# a(n, k, l) = 1 / (n - 1) * sum(i=1..n-1,
#   i * binomial(n, i) * a(n - i, k, l) * sum(j=1..k, a(i, j + l - 1, l)))
#
# This script computes a(n, k, l) by dynamic programming.

def ncr(n, r)
  r = [r, n - r].min
  return 1 if r <= 0

  num = 1
  den = 1
  1.upto(r){|i|
    num *= n - r + i
    den *= i
  }
  num / den
end

def build_table(n_max, k_max, l)
  k_limit = ->(n) {k_max + (n_max - n) * (l - 1)}
  a = Array.new(n_max + 1){|n| Array.new(k_limit.call(n) + 1, 0)}

  0.upto(k_limit.call(1)){|k|
    a[1][k] = 1
  }

  2.upto(n_max){|n|
    1.upto(k_limit.call(n)){|k|
      total = 0
      1.upto(n - 1){|i|
        inner = 0
        1.upto(k){|j|
          inner += a[i][j + l - 1]
        }
        total += i * ncr(n, i) * a[n - i][k] * inner
      }
      a[n][k] = total / (n - 1)
    }
  }

  a
end

def a_number(n, k, l)
  build_table(n, k, l)[n][k]
end

def seq(n_max, k, l)
  table = build_table(n_max, k, l)
  (1..n_max).map{|n| table[n][k]}
end

n_max = 50
k = 1
l = 6
ary = seq(n_max, k, l)
(1..n_max).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}