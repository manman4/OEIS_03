def A(n)
  str = n.to_s(2)
  (str + str.reverse).to_i
end

def A342036(n)
  (0..n).map{|i| A(i)}
end

p A342036(25)
