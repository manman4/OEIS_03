def s(k, n)
  s = 0
  (1..n).each{|i| s += i if n % i == 0 && i % k == 0}
  s
end

def A(ary, n)
  a_ary = [1]
  a = [0] + (1..n).map{|i| ary.inject(0){|s, j| s += j[1] * s(j[0], i)}}
  (1..n).each{|i| a_ary << (1..i).inject(0){|s, j| s -= a[j] * a_ary[-j]} / i}
  a_ary
end

def B(k, n)
  A([[2, 5 * k], [1, -2 * k], [4, -2 * k]], n)
end

def A319574(n)
  a = []
  ary = 
  (0..n).each{|i| a << B(i, n - i)}
  ary = []
  (0..n).each{|i|
    ary1 = []
    (0..i).each{|j|
      ary1 << a[i - j][j]
    }
    ary += ary1.reverse
  }
  ary
end

n = 139
ary = A319574(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
