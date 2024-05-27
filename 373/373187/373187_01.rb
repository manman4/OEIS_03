def A000292(n)
  n * (n + 1) * (n + 2) / 6
end

def A(k, n)
  ary = [0]
  (1..n).each{|i|
    j = A000292(i)
    j += ary[i / k] if i % k == 0
    ary << j
  }
  ary[1..-1]
end
p A(4, 47)