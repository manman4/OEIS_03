require 'prime'
def A(n)
  (1..n).each{|a|
    (a + 1..n).each{|b|
      return a if a * a + a * b + b * b == n * n
    }
  }
end
def A349772(n)
  ary = []
  i = 1
  while ary.size < n
    ary << A(i) if i.prime? && i % 6 == 1
    i += 1
  end
  ary
end
p A349772(100)