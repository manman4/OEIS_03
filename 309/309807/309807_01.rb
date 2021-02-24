def A(n)

  (0..n).to_a.permutation.select{|i| (1..n).all?{|j| i[j - 1] * (j + 1) >= i[j] * j}}

end

def B(n)

  (0..n).to_a.permutation.select{|i| (1..n).all?{|j| i[j - 1] * (j + 1) > i[j] * j}}

end

def A309807(n)

  (0..n).map{|i| p A(i).size} #- B(i)}

end

# p A309807(8)
# p (0..8).map{|i| p B(i).size}

p (1..6).to_a.permutation.select{|i| (1..5).all?{|j| i[j - 1] * (j + 2) > i[j] * (j + 1)}}
p (1..5).to_a.permutation.select{|i| (1..4).all?{|j| i[j - 1] * (j + 2) >= i[j] * (j + 1)}}
