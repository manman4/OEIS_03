def A(k, n)
  s = 0
  (1..n).each{|i|
    s += (-1) ** ((i + n / i) % 2) * i ** k if n % i == 0
  }
  s
end

def B(k, n)
  (1..n).map{|i| A(k, i)}
end

n = 10000
ary = B(9, n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
