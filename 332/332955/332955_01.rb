def A(n)
  (1..n).to_a.permutation.select{|i| (1..n - 1).all?{|j| i[j - 1] * (j + 1) > i[j] * j}}.size
end

def A332955(n)
  a = (0..n).map{|i| A(i)}
  [1] + (1..n).map{|i| a[i] - a[i - 1]}
end

p A332955(11)