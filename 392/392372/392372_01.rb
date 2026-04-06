def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# a(0) = 1; a(n) = Sum_{i=0..n-1} (i^2+2*n-1) * binomial(n-1,i) * a(i)*a(n-1-i) + Sum_{i, j, k>=0 and i+j+k=n-2} (i*j+i-j^2-2*j) * (n-1)!/(i!*j!*k!) * a(i)*a(j)*a(k).
def A(n)
  ary = [1]
  (1..n).each{|m|
    s = 0
    (0..m - 1).each{|i|
      s += (i ** 2 + 2 * m - 1) * ncr(m - 1, i) * ary[i] * ary[m - 1 - i]
    }
    (0..m - 2).each{|i|
      (0..m - 2 - i).each{|j|
        k = m - 2 - i - j
        s += (i * j + i - j ** 2 - 2 * j) * f(m - 1) / (f(i) * f(j) * f(k)) * ary[i] * ary[j] * ary[k]
      }
    }
    ary << s
  }
  ary
end

n = 260
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
