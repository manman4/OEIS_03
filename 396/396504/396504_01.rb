#!/usr/bin/env ruby
# frozen_string_literal: true

# Square array A(n,k), read by antidiagonals downward.
# A(n,k) = sum over 0 = x_0 <= x_1 <= ... <= x_{k-1} <= x_k = n of
#          product over j = 0..k-1 of
#            2 * (x_j + 1) * (4*x_{j+1} - 2*x_j + 2)^(x_{j+1} - x_j - 1)
#            * binomial(x_{j+1}, x_j)
#
# This version uses the simple recursion:
#   a(n, k) = sum_{x=0..n} a(x, k-1) * weight(x, n)
# with a(n, 0) = 1 if n = 0, otherwise 0.

DEFAULT_N = 15
DEFAULT_K = 3

def ncr(n, r)
  return 0 if r.negative? || r > n
  return 1 if r == 0 || r == n

  r = [r, n - r].min
  (1..r).inject(1){|acc, i| acc * (n - r + i) / i}
end

def weight(x, n)
  return 1 if x == n

  2 * (x + 1) * ncr(n, x) * (4 * n - 2 * x + 2)**(n - x - 1)
end

def a(n, k, memo = Hash.new { |h, key| h[key] = {} })
  return n.zero? ? 1 : 0 if k.zero?
  return memo[k][n] if memo[k].key?(n)

  sum = 0
  0.upto(n) do |x|
    sum += a(x, k - 1, memo) * weight(x, n)
  end

  memo[k][n] = sum
end

n = 10
(0..n).each{|i|
  (0..i).each{|j|
    print a(j, i - j)
    print ', '
  }
}