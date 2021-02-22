# x����J�n��y�悵��p/q
def A(x, y, p, q, base, n)
  ary = [x]
  s = x
  n.times{|i|
    m = base ** (i + 1)
    (0..base - 1).each{|j|
      k = j * m + s
      if (q * k ** y - p) % (m * base) == 0
        ary << j
        s = k
        break
      end
    }
  }
  ary
end

def A309818(n)
  A(1, 3, -987654321, 10 ** 9 - 1, 10, n)
end

m = 10100
ary = A309818(m)
(0..10000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
