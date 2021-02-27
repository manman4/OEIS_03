def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A303580(n)
  ary = [1]
  s, t = 1, 1
  while ary.size < n
    s += 1
    ds = phi(s)
    ary << s if ds < t
    t = ds
  end
  ary
end

n = 10000
ary = A303580(n)
(1..n).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
