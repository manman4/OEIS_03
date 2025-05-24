# A(n,k) = Product_{j=0..n-1} (n-k*j).
def f(n, k)
  return 1 if n == 0
  (0..n - 1).inject(1){|s, i| s * (n - k * i)}
end

def A(n)
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << f(j, i - j + 1)
    } 
  }
  ary
end

n = 139
ary = A(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
