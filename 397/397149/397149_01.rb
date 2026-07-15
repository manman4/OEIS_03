def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(1) = 1; a(n) = Sum_{i, j, k, l>=1 and i+j+k+l=n} a(i)*a(j)*a(k)*a(l) + Sum_{r=1..floor(n/3)} (-1)^(r+1) * binomial(n-2*r,r) * a(n-2*r).
def a(n)
  ary = [0, 1]
  (2..n).each{|m|
    sum = 0
    (1..m-3).each{|i|
      (1..m-2-i).each{|j|
        (1..m-1-i-j).each{|k|
          l = m - i - j - k
          sum += ary[i] * ary[j] * ary[k] * ary[l]
        }
      } 
    }
    ary << (1..m / 3).inject(sum){|s, r| s + (-1) ** (r + 1) * ncr(m - 2 * r, r) * ary[m - 2 * r]}
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
