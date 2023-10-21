def a(n)
  ary = Array.new(n, 0)
  (1..n).to_a.each_permutation{|a|
    ary[(1..n).map{|i| (a[a[i - 1] - 1] - i).abs}.max] += 1
  }
  ary
end

(2..12).each{|i| p a(i)[1]}

# 0
# 0
# 2
# 6
# 24
# 80
# 312
# 1152
# 4616
# 18384
# 77280
