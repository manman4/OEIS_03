# Numbers k such that 5 is the first digit of 2^k.
def A(k, n)
  ary = []
  s = 1
  cnt = 0
  while ary.size < n
    ary << cnt if s.to_s[0] == k.to_s
    s *= 2
    cnt += 1
  end
  ary
end

n = 10000
ary = A(5, n)
(1..ary.size).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}