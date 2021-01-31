require 'prime'

def f(n)
  return true if n == 1
  ary = n.prime_division.to_a
  return true if ary.map{|j| j[1]} == Array.new(ary.size, 1)
  false
end

def A(k, n)
  p_ary = Prime.each(10 ** 7).to_a
  ary = []
  cnt = 0
  while ary.size < n
    i = p_ary[cnt]
    flag = true
    m = 0
    while flag && (m < k)
      m += 1
      if (i - m) < 2
        flag = false
        break
      end
      if f(i - m)
        flag = false
        break
      end
    end
    ary << i if flag
    cnt += 1
  end
  ary
end

n = 10000
ary = A(3, n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
