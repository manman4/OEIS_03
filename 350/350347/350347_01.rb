require 'prime'
def A(n)
  (1..n).each{|a|
    (a + 1..n).each{|b|
      return b if a * a + a * b + b * b == n * n
    }
  }
end
def A350347(n)
  ary = []
  i = 1
  while ary.size < n
    ary << A(i) if i.prime? && i % 6 == 1
    i += 1
  end
  ary
end

n = 1000
ary = A350347(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}