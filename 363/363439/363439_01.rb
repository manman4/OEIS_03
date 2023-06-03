def A(n, k)
  # a(0) = 1; a(n) = (1/n) * Sum_{j=1..n} ( Sum_{d|j} d * k^(j/d) * a(d-1) ) * a(n-j)
  a = [1]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j| 
      t = 0
      (1..j).each{|d|
        if j % d == 0
          t += d * k ** (j/ d) * a[d - 1]
        end
      }
      s += t * a[i - j]
    }
    a[i] = s / i
  }
  a
end

n = 1000
ary = A(n, 3)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}