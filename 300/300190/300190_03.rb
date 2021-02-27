require 'prime'

def A(n)
  ary = [1] + Array.new(n - 1, 0)
  (1..n).each{|i|
    i1 = 2 * i
    a = ary.clone
    (0..n - 1).each{|j| a[(j + i1) % n] += ary[j]}
    ary = a
  }
  ary[(n * (n + 1) / 2) % n] / 2
end
def A300190(n)
  (1..n).map{|i| A(i)}
end
p A300190(100)

p (2..9).map{|i| A(2 ** i).prime_division[0][1]}
