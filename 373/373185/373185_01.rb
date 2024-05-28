def A(k, n)
  ary = [0]
  (1..n).each{|i|
    j = i + 1
    j += ary[i / k] if i % k == 0
    ary << j
  }
  ary[1..-1]
end
p A(4, 90)