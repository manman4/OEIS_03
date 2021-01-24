def A(k,n)

  (1..n).to_a.permutation.select{|i| (1..n - 1).all?{|j| i[j - 1] * (j + k + 1) >= i[j] * (j + k)}}

end

def A309807(k,n)

  (0..n).map{|i| p A(k,i)}

end

p A309807(1,5)

# a = []
# (0..12).each{|i|
#   (0..i).each{|j|
#     p a << A(j,i).size
#   }
# }
# p a