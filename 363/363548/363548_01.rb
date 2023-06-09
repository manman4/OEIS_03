def A(n, k)
  # a(0) = 1; a(n) = (1/n) * Sum_{j=1..n} ( k + Sum_{d|j} d * a(d-1) ) * a(n-j)
  a = [1]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j| 
      t = k
      (1..j).each{|d|
        if j % d == 0
          t += d * a[d - 1]
        end
      }
      s += t * a[i - j]
    }
    a[i] = s / i
  }
  a
end

def A363548(n)
  ary = A(n, 3)
  a = [1, 1, 5]
  (3..n).each{|i|
    # Sum_{k=0..3} (-1)^k * binomial(3,k) * ary[n-k]
    a << ary[i] - 3 * ary[i - 1] + 3 * ary[i - 2] - ary[i - 3]
  }
  a
end

n = 1100
m = 1000
ary = A363548(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}