def s(k, n)
  s = 0
  (1..n).each{|i| s += i ** (i ** k * n / i) if n % i == 0}
  s
end

def A308676(n)
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << s(i - j, j)
    }
  }
  ary
end

n = 9
ary = A308676(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
