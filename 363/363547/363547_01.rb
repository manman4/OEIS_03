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

def A363547(n)
  ary = A(n, 2)
  a = [1, 1]
  (2..n).each{|i|
    # Sum_{k=0..2} (-1)^k * binomial(2,k) * ary[n-k]
    a << ary[i] - 2 * ary[i - 1] + ary[i - 2]
  }
  a
end

n = 1100
m = 1000
ary = A363547(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}