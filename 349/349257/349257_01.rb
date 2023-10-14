def A(n)
  max = 0
  (1..n).to_a.permutation{|i|
    m = (1..n).inject(0){|s, j| s + j / i[j - 1].to_r}
    if m.denominator == 1
      max = m if max < m
    end
  }
  max.to_i
end

def A349257(n)
  (0..n).map{|i| A(i)}
end

p A349257(8)
