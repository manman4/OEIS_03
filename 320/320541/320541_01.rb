def A(k, n)
  s = 0
  (1..n).each{|i|
    (1..k).each{|j|
      s += (n + 1 - i) * (k + 1 - j) if i.gcd(j) == 1
    }
  }
  s
end

def A320541(n)
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << A(j, i)
    }
  }
  ary
end

n = 140
ary = A320541(n)
(1..ary.size).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
