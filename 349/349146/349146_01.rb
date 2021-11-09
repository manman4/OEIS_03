def A(n)
  return 1 if n == 0
  cnt = 0
  (1..n).to_a.repeated_permutation(n){|i|
    cnt += 1 if (1..n).inject(0){|s, j| s + 1 / i[j - 1].to_r}.denominator == 1
  }
  cnt
end

def A349146(n)
  (0..n).map{|i| A(i)}
end

p A349146(6)
