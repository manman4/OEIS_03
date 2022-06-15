require 'prime'

def A(ary)
  y = ary.size
  x = ary[0]
  a = (0..y - 1).map{|i| [1] * ary[i] + [0] * (x - ary[i])}
  (0..x - 1).map{|i| (0..y - 1).inject(0){|s, j| s + a[j][i]}}
end

def B(n)
  n.prime_division.map{|i| [i[0]] * i[1]}.flatten
end

def C(n)
  A(B(n).reverse).inject(:*)
end

def A354969(n)
  (2..n).map{|i| C(i)}
end

p A354969(100)
