require 'prime'

def d(n)
  sum = 1
  pq = n.prime_division
  pq.each{|a| sum *= (1 + a[1])}
  sum
end

n = 1000000000
m = 10000
cnt = 1
a, b, c = 1, 1r / 2, 1r / 2
(3..n).each{|i|
  j = a + b + c
  if j.denominator == 1
    print cnt
    print ' '
    puts i
    cnt += 1
    break if cnt > m
  end
  a, b, c = b, c, 1r / d(i + 1)
}
