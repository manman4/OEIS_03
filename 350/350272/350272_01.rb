def A(n)
  ary = Array.new(n, 0)
  [1, -1].repeated_permutation(n - 1){|i|
    ary[(2..n).inject(1){|s, j| s + i[j - 2] * j} % n] += 1
  }
  ary
end

def A350272(n)
  (1..n).map{|i| A(i)}.flatten
end

p A350272(12)

