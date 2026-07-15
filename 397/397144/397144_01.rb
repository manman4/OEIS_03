def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(1) = 1; a(n) = Sum_{i, j, k>=1 and i+j+k=n} a(i)*a(j)*a(k) + Sum_{r=1..floor(n/2)} (-1)^(r+1) * binomial(n-r,r) * a(n-r).
def a(n)
  ary = [0, 1]
  (2..n).each{|m|
    sum = 0
    (1..m-2).each{|i|
      (1..m-1-i).each{|j|
        k = m - i - j
        sum += ary[i] * ary[j] * ary[k]
      } 
    }
    ary << (1..m / 2).inject(sum){|s, r| s + (-1) ** (r + 1) * ncr(m - r, r) * ary[m - r]}
  }
  ary
end

n = 40
p ary = a(n)
(1..n).each{|i| 
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
