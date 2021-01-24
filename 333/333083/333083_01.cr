def a(n)
  cnt = 0
  (1..n).to_a.each_permutation{|i| 
    cnt += 1 if (1..n).map{|j| i[j - 1] * j}.uniq.size == n
  }
  cnt
end

def a333083(n)

  (0..n).map{|i| p a(i)}

end

p a333083(12)