def s(k, n)
  s = 0
  (1..n).each{|i| s += k ** (i - 1) if n % i == 0}
  s
end

def A308674(n)
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << s(i - j, j)
    }
  }
  ary
end

n = 140
ary = A308674(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}