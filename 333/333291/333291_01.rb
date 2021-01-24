def phi(n)
  s = 0
  (1..n).each{|i| s += 1 if i.gcd(n) == 1}
  s
end

def A(n)
  a = [0] + (1..n).map{|i| phi(i)}
  ary = []
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      s += j * a[j] if i.gcd(j) == 1
    }
    ary << s
  }
  ary
end

m = 10100
n = 10000
ary = A(m)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
