def a(n)
  cnt = 0
  (1..n).to_a.each_permutation{|i|
    cnt += 1 if (1..n - 1).all?{|j| i[j - 1] * (j + 1) > i[j] * j}
  }
  cnt
end

def a330432(n)
  (0..n).map{|i| p a(i)}
end

p a330432(15)
