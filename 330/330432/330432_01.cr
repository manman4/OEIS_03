def a(n)
  cnt = 0
  (2..n - 1).to_a.permutations{|i| 
    cnt += 1 if (2..n - 1).all?{|j| i[j - 2] * j >= n}
  }
  cnt
end

def a330432(n)

  (0..n).map{|i| p a(i)}

end

p a330432(15)
