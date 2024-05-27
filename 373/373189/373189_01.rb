def A000217(n)
  n * (n + 1) / 2
end

def A(k, n)
  ary = [0]
  (1..n).each{|i|
    j = A000217(i)
    j += ary[i / k] if i % k == 0
    ary << j
  }
  ary[1..-1]
end
p A(4, 56)