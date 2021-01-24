def A(n)

  (1..n).to_a.permutation.select{|i| (1..n).map{|j| i[j - 1] * j}.uniq.size == n}.size

end

def A333083(n)

  (0..n).map{|i| p A(i)}

end

p A333083(11)

# a = []
# (0..12).each{|i|
#   (0..i).each{|j|
#     p a << A(j,i).size
#   }
# }
# p a