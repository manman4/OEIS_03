def d(n)
  cnt = 0
  (1..n).each{|i| cnt += 1 if n % i == 0}
  cnt
end

def A303578(n)
  ary = [1]
  s, t = 1, 1
  while ary.size < n
    s += 1
    ds = d(s)
    ary << s if ds < t
    t = ds
  end
  ary
end

n = 10000
ary = A303578(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
