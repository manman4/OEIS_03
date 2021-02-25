def s(k, n)
  s = 0
  (1..n).each{|i| s += i ** (k + n) if n % i == 0}
  s
end

def A(k, n)
  s_ary = [0] + (1..n).map{|i| s(k, i)}
end

def A308504(n)
  a = []
  (0..n).each{|i| a << A(i, n - i)}
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << a[i - j][j]
    }
  }
  ary
end

n = 140
ary = A308504(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
