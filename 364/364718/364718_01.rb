require 'prime'

def power0(a, n)
  return 1 if n == 0
  k = power0(a, n >> 1)
  k *= k
  return k if n & 1 == 0
  return k * a
end

# x > 0
def d(i)
  sum = 1
  pq = i.prime_division
  pq.each{|a| sum *= 1 + a[1]}
  sum
end

def A(n)
  ary = []
  i = 1
  while ary.size < n
    # d(k) > d(k+1) > d(k+2)
    if d(i) > d(i + 1) && d(i + 1) > d(i + 2)
      ary << i
    end
    i += 1
  end
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}