def A(k, n)
  (1..n).inject(0){|s, i| s + (-1) ** ((i + 1) % 2) * (n / i) ** k}
end

def A344726(n)
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << A(i - j + 1, j)
    }
  }
  ary
end

n = 140
ary = A344726(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
