def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A303579(n)
  ary = []
  s, t = 1, 1
  cnt = 1
  while ary.size < n
    s += 1
    ds = phi(s)
    if ds < t
      ary << cnt
      cnt = 1
    else
      cnt += 1
    end
    t = ds
  end
  ary
end

n = 10000
ary = A303579(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
