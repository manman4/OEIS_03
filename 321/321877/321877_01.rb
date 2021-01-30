def s0(n)
  s = 0
  1.step(n, 2){|i| s += i if n % i == 0}
  s
end

def s(k, n)
  s = 0
  (1..n).each{|i| s += i ** k * s0(n / i) if n % i == 0}
  s
end

def A(k, n)
  a_ary = [1]
  (1..n).map{|i| s(k, i)}
  a = [0] + (1..n).map{|i| s(k, i)}
  (1..n).each{|i| a_ary << (1..i).inject(0){|s, j| s += a[j] * a_ary[-j]} / i}
  a_ary
end

def A321877(n)
  a = []
  # ’Êí‚Æ‚Í1‚¸‚ê‚é
  (0..n).each{|i| a << A(i + 1, n - i)}
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end

n = 139
ary = A321877(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
