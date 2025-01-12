def A(m, n)
  a = Array.new(m, 1)
  ary = [1]
  while ary.size < n
    i = a[1..-1].inject(:*) + 1
    break if i % a[0] > 0
    a = *a[1..-1], i / a[0]
    ary << a[0]
  end
  ary
end

def A072713(n)
  A(5, n)
end

n = 20
# 1からnまで
ary = A072713(n + 4)
(1..n).each{|i|
  # a(n) = b(n+1) * b(n+2) * b(n+3) * b(n+4) where b is A072713.
  j = ary[i] * ary[i + 1] * ary[i + 2] * ary[i + 3]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
