def A(k, n)
  s = 0
  (1..n).each{|i|
    if n % i == 0
      s += (-1) ** (i % 2 - 1)* i ** k
    end
  }
  s
end

def B(k, n)
  (1..n).map{|i| A(k, i)}
end

n = 10000
ary = B(7, n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
