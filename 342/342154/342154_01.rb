def A(k, n)
  m = n ** k
  cnt = 0
  (1..Math.sqrt(m / 2)).each{|i|
    j = m - i * i
    cnt += 1 if Math.sqrt(j).to_i ** 2 == j && j > 0
  }
  cnt
end

# p (0..100).map{|i| A(2, i)}
# p (0..100).map{|i| A(3, i)}
# p (0..100).map{|i| A(4, i)}
p (0..105).map{|i| A(5, i)}
# p (0..100).map{|i| A(6, i)}
# p (0..100).map{|i| A(7, i)}

# (2..7).each{|i|
#   p (1..100).select{|j| A(i, j) > 0}
# }

# (1..3125).each{|i|
#   j = 3125 - i * i
#   if j > 0
#     if Math.sqrt(j).to_i ** 2 == j
#       p [i, Math.sqrt(j).to_i]
#     end
#   end
# }