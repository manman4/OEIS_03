# x����J�n��y�悵��p/q
def A(x, y, p, q, base, n)
  s = x
  n.times{|i|
    m = base ** (i + 1)
    (0..base - 1).each{|j|
      k = j * m + s
      if (q * k ** y - p) % (m * base) == 0
        s = k
        break
      end
    }
  }
  s
end

def A309641(n)
  str = A(7, 3, -11, 3, 10, n).to_s.reverse
  (0..n).map{|i| str[i].to_i}
end

m = 10100
ary = A309641(m)
(0..10000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
