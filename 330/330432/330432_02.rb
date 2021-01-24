# def A(n)
#   cnt = 0
#   (2..n - 1).to_a.permutation{|i| 
#     cnt += 1 if (2..n - 1).all?{|j| i[j - 2] * j >= n}
#   }
#   cnt
# end

# def A330432(n)

#   (0..n).map{|i| p A(i)}

# end

# p A330432(15)


def A(n)

  (2..n - 1).to_a.permutation.select{|i| (2..n - 1).all?{|j| i[j - 2] * j >= n}}.size

end

def A330432(n)

  (0..n).map{|i| A(i)}

end

p A330432(10)