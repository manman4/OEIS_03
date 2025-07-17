def A(n)
  m = n
  n.to_s.split('').map(&:to_i).each{|i|
    m /= i if i != 0 && m % i == 0
  }
  m
end

def A382895(n)
  (1..n).map{|i| A(i)}
end

n = 10000
ary = A382895(n)
(1..ary.size).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}