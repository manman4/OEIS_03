# E.g.f. B(x) satisfies B'(x) = 1 + B^l(x),
# where B^l(x) denotes the l-th iterate of B.
#
# a(n, k, l) = n! * [x^n] B^k(x)
# a(n, 0, l) = 0^(n - 1)
# a(n, k, l) = a(n, k - 1, l) + sum(j=1..n-1,
#   binomial(n - 1, j) * a(j, k + l - 1, l) * a(n - j, k - 1, l)
# ) for k > 0.
#
# This script computes a(n, k, l) by dynamic programming.

def binomial(n, k)
  k = [k, n - k].min
  return 1 if k <= 0

  num = 1
  den = 1
  1.upto(k){|i|
    num *= n - k + i
    den *= i
  }
  num / den
end

def build_table(n_max, k_max, l)
  k_limit = ->(n){k_max + (n_max - n) * l}
  a = Array.new(n_max + 1){|n| Array.new(k_limit.call(n) + 1, 0)}

  0.upto(k_limit.call(1)){|k|
    a[1][k] = 1
  }

  2.upto(n_max){|n|
    a[n][0] = 0
    1.upto(k_limit.call(n)){|k|
      total = a[n][k - 1]
      1.upto(n - 1){|j|
        total += binomial(n - 1, j) * a[j][k + l - 1] * a[n - j][k - 1]
      }
      a[n][k] = total
    }
  }

  a
end

def a_number(n, k, l)
  build_table(n, k, l)[n][k]
end

def a_vector(n_max, k = 1, l = 1)
  table = build_table(n_max, k, l)
  (1..n_max).map{|n| table[n][k]}
end

if __FILE__ == $PROGRAM_NAME
  n_max = (ARGV[0] || 10).to_i
  k = (ARGV[1] || 1).to_i
  l = (ARGV[2] || 5).to_i

  puts a_vector(n_max, k, l).join(", ")
end
