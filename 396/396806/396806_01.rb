# E.g.f. A(x) satisfies A(x) = x * exp(A^l(x)),
# where A^l(x) denotes the l-th iterate of A.
#
# a(n, k, l) = n! * [x^n] A^k(x)
# a(1, k, l) = 1
# a(n, k, l) = 1 / (n - 1) * sum(i=1..n-1,
#   i * binomial(n, i) * a(n - i, k, l) * sum(j=1..k, a(i, j + l - 1, l)))
#
# This script computes a(n, k, l) by dynamic programming.

def binomial(n, k)
  k = [k, n - k].min
  return 1 if k <= 0

  num = 1
  den = 1
  1.upto(k) do |i|
    num *= n - k + i
    den *= i
  end
  num / den
end

def build_table(n_max, k_max, l)
  k_limit = ->(n) { k_max + (n_max - n) * (l - 1) }
  a = Array.new(n_max + 1) { |n| Array.new(k_limit.call(n) + 1, 0) }

  0.upto(k_limit.call(1)) do |k|
    a[1][k] = 1
  end

  2.upto(n_max) do |n|
    1.upto(k_limit.call(n)) do |k|
      total = 0
      1.upto(n - 1) do |i|
        inner = 0
        1.upto(k) do |j|
          inner += a[i][j + l - 1]
        end
        total += i * binomial(n, i) * a[n - i][k] * inner
      end
      a[n][k] = total / (n - 1)
    end
  end

  a
end

def a_number(n, k, l)
  build_table(n, k, l)[n][k]
end

def seq(n_max, k, l)
  table = build_table(n_max, k, l)
  (1..n_max).map { |n| table[n][k] }
end

if __FILE__ == $PROGRAM_NAME
  n_max = (ARGV[0] || 8).to_i
  k = (ARGV[1] || 1).to_i
  l = (ARGV[2] || 1).to_i

  puts seq(n_max, k, l).join(", ")
end