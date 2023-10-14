def A(n)
  ary = Array.new(n, 0)
  (1..n).to_a.permutation{|i|
    ary[i[-1] - 1] += 1 if (1..n).inject(0){|s, j| s + j / i[j - 1].to_r}.denominator == 1
  }
  ary
end

def A349277(n)
  (1..n).map{|i| A(i)}.flatten
end

p A349277(12)
