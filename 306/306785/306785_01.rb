require 'prime'

def A(n)
  a, b, c, d = 4, 0, 0, 3
  cnt = 0
  ary = []
  while ary.size < n
    a, b, c, d = b, c, d, a + b
    cnt += 1
p [cnt, ary]
    if cnt.prime? && a % (cnt * cnt) == 0
      ary << cnt
    end
  end
  ary
end

n = 10
p ary = A(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
