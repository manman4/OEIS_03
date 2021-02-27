require 'prime'
def A(n)
  ary = [1] + Array.new(n - 1, 0)
  (1..n).each{|i|
    it = i * (i + 1)
    a = ary.clone
    (0..n - 1).each{|j| a[(j + it) % n] += ary[j]}
    ary = a
  }
  ary[(n * (n + 1) * (n + 2) / 6) % n] / 2
end

n = 2048
p A(n).prime_division
