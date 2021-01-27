def A(k, n)
  cnt = 0
  (1..n - 1).to_a.repeated_permutation(k){|a|
    if (0..k - 1).all?{|i| (0..i).inject(0){|s, j| s + a[j] * n ** (i - j)} % (i + 1) == 0}
      cnt += 1
    end
  }
  cnt
end
def A324019(n)
  (2..n).map{|i| (1..i - 1).map{|j| A(j, i)}}.flatten
end
p A324019(8)
