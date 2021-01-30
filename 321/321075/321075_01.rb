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

def A321075(n)
  A(6, 2, 3, 1, 11, n)
end

m = 10100
n = 10000
ary = A321075(m)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
