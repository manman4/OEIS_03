def A(n)

  (1..n).to_a.permutation.select{|i| (1..n).map{|j| i[j - 1] / j.to_r}.uniq.size == n}.size

end

def A333082(n)

  (0..n).map{|i| A(i)}

end

p A333082(11)

# a = []
# (0..12).each{|i|
#   (0..i).each{|j|
#     p a << A(j,i).size
#   }
# }
# p a