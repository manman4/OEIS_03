def A347594(n)
  s = 1
  ary = [1]
  (1..n).each{|i|
    j = i * i + s * s
    k = Math.sqrt(j).floor + 1
    s = k * k - j
    ary << s
  }
  ary
end
p A347594(63)
